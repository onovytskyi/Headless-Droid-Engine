#include "Engine/Config/Bootstrap.h"

#include "Engine/Engine/Utils/ResourceLoader.h"

#include "Engine/Debug/Assert.h"
#include "Engine/Debug/Log.h"
#include "Engine/Engine/Memory/EngineMemoryInterface.h"
#include "Engine/Foundation/Memory/Utils.h"
#include "Engine/Framework/File/Utils.h"
#include "Engine/Framework/Utils/CommandBuffer.h"
#include "Engine/Framework/Utils/CommandBufferReader.h"

#include "Externals/fast_obj/fast_obj.h"
#include "Externals/meshoptimizer/meshoptimizer.h"
#include "Externals/stbimage/stb_image.h"

namespace hd
{
    namespace util
    {
        static const uint32_t MESH_MAGIC_NUMBER = 0x12FEED21;

        MaterialResouce::MaterialResouce(allocator_type allocator)
            : DiffuseColor{}
            , AmbientColor{}
            , SpecularColor{}
            , SpecularPower{}
            , RefractionIndex{}
            , DiffuseTexture{ allocator }
            , NormalTexture{ allocator }
            , RoughnessTexture{ allocator}
            , MetalnessTexture{ allocator }
        {

        }

        MaterialResouce::MaterialResouce(MaterialResouce const& other, allocator_type const& allocator)
            : DiffuseColor{ other.DiffuseColor }
            , AmbientColor{ other.AmbientColor }
            , SpecularColor{ other.SpecularColor }
            , SpecularPower{ other.SpecularPower }
            , RefractionIndex{ other.RefractionIndex }
            , DiffuseTexture{ other.DiffuseTexture, allocator }
            , NormalTexture{ other.NormalTexture, allocator }
            , RoughnessTexture{ other.RoughnessTexture, allocator }
            , MetalnessTexture{ other.MetalnessTexture, allocator }
        {

        }

        MaterialResouce::MaterialResouce(MaterialResouce&& other, allocator_type const& allocator)
            : DiffuseColor{ other.DiffuseColor }
            , AmbientColor{ other.AmbientColor }
            , SpecularColor{ other.SpecularColor }
            , SpecularPower{ other.SpecularPower }
            , RefractionIndex{ other.RefractionIndex }
            , DiffuseTexture{ std::move(other.DiffuseTexture), allocator }
            , NormalTexture{ std::move(other.NormalTexture), allocator }
            , RoughnessTexture{ std::move(other.RoughnessTexture), allocator }
            , MetalnessTexture{ std::move(other.MetalnessTexture), allocator }
        {

        }

        MeshResource::MeshResource(allocator_type allocator)
            : Vertices{ allocator }
            , Indices{ allocator }
            , MaterialIndex{}
        {

        }

        MeshResource::MeshResource(MeshResource const& other, allocator_type const& allocator)
            : Vertices{ other.Vertices, allocator }
            , Indices{ other.Indices, allocator }
            , MaterialIndex{ other.MaterialIndex }
        {

        }

        MeshResource::MeshResource(MeshResource&& other, allocator_type const& allocator)
            : Vertices{ std::move(other.Vertices), allocator }
            , Indices{ std::move(other.Indices), allocator }
            , MaterialIndex{ other.MaterialIndex }
        {

        }

        void LoadRawMesh(std::pmr::u8string const& filePath, std::pmr::vector<MeshResource>& outMeshes, std::pmr::vector<MaterialResouce>& outMaterials)
        {
            // #HACK Fast obj only supports ASCII in file path, consider rewrite it to use wide chars
            // #Optimization use our allocator with fast obj
            fastObjMesh* mesh = fast_obj_read(reinterpret_cast<char const*>(filePath.c_str()));
            hdAssert(mesh != nullptr, u8"Fast obj failed to load mesh file %s.", filePath.c_str());

            outMaterials.resize(mesh->material_count);
            for (uint32_t materialIdx = 0; materialIdx < outMaterials.size(); ++materialIdx)
            {
                MaterialResouce& material = outMaterials[materialIdx];
                fastObjMaterial& materialData = mesh->materials[materialIdx];

                material.DiffuseColor.w = materialData.d;
                material.SpecularPower = materialData.Ns;
                material.RefractionIndex = materialData.Ni;

                material.AmbientColor.x = materialData.Ka[0];
                material.AmbientColor.y = materialData.Ka[1];
                material.AmbientColor.z = materialData.Ka[2];
                material.AmbientColor.w = 1.0f;

                material.DiffuseColor.x = materialData.Kd[0];
                material.DiffuseColor.y = materialData.Kd[1];
                material.DiffuseColor.z = materialData.Kd[2];

                material.SpecularColor.x = materialData.Ks[0];
                material.SpecularColor.y = materialData.Ks[1];
                material.SpecularColor.z = materialData.Ks[2];

                if (materialData.map_Kd.path)
                {
                    material.DiffuseTexture = reinterpret_cast<char8_t*>(materialData.map_Kd.path);
                }

                if (materialData.map_bump.path)
                {
                    material.NormalTexture = reinterpret_cast<char8_t*>(materialData.map_bump.path);
                }

                if (materialData.map_Ns.path)
                {
                    material.RoughnessTexture = reinterpret_cast<char8_t*>(materialData.map_Ns.path);
                }

                if (materialData.map_Ka.path)
                {
                    material.MetalnessTexture = reinterpret_cast<char8_t*>(materialData.map_Ka.path);
                }
            }

            outMeshes.resize(mesh->group_count);
            for (uint32_t submeshIdx = 0; submeshIdx < outMeshes.size(); ++submeshIdx)
            {
                MeshResource& submesh = outMeshes[submeshIdx];
                fastObjGroup& submeshData = mesh->groups[submeshIdx];

                uint64_t totalIndices = 0;
                uint32_t firstFace = submeshData.face_offset;
                uint32_t nextToLastFace = firstFace + submeshData.face_count;

                submesh.MaterialIndex = mesh->face_materials[firstFace];

                for (uint32_t faceIdx = firstFace; faceIdx < nextToLastFace; ++faceIdx)
                {
                    hdAssert(submesh.MaterialIndex == mesh->face_materials[faceIdx]);
                    totalIndices += uint64_t(3) * (mesh->face_vertices[faceIdx] - 2);
                }

                MeshResourceVertex* vertices = hdAllocate(mem::Scratch(), MeshResourceVertex, totalIndices);
                uint64_t vertexOffset = 0;
                uint64_t indexOffset = submeshData.index_offset;
                for (uint32_t faceIdx = firstFace; faceIdx < nextToLastFace; ++faceIdx)
                {
                    for (uint32_t faceVertexIdx = 0; faceVertexIdx < mesh->face_vertices[faceIdx]; ++faceVertexIdx)
                    {
                        fastObjIndex combinedIndex = mesh->indices[indexOffset + faceVertexIdx];

                        // triangulate polygon on the fly; offset-3 is always the first polygon vertex
                        if (faceVertexIdx >= 3)
                        {
                            vertices[vertexOffset + 0] = vertices[vertexOffset - 3];
                            vertices[vertexOffset + 1] = vertices[vertexOffset - 1];
                            vertexOffset += 2;
                        }

                        vertices[vertexOffset].Position = { mesh->positions[combinedIndex.p * 3 + 0],
                                                            mesh->positions[combinedIndex.p * 3 + 1],
                                                            mesh->positions[combinedIndex.p * 3 + 2] };

                        vertices[vertexOffset].Normal = { mesh->normals[combinedIndex.n * 3 + 0],
                                                          mesh->normals[combinedIndex.n * 3 + 1],
                                                          mesh->normals[combinedIndex.n * 3 + 2] };

                        vertices[vertexOffset].UV = { mesh->texcoords[combinedIndex.t * 2 + 0],
                                                      mesh->texcoords[combinedIndex.t * 2 + 1] };

                        vertexOffset++;
                    }

                    indexOffset += mesh->face_vertices[faceIdx];
                }

                uint32_t* remapTable = hdAllocate(mem::Scratch(), uint32_t, totalIndices);
                size_t totalVertices = meshopt_generateVertexRemap(remapTable, nullptr, totalIndices, vertices, totalIndices, sizeof(MeshResourceVertex));

                hdAssert(totalIndices <= std::numeric_limits<uint32_t>::max());
                submesh.Indices.Resize(totalIndices);
                meshopt_remapIndexBuffer(submesh.Indices.Data(), nullptr, totalIndices, remapTable);

                hdAssert(totalVertices <= std::numeric_limits<uint32_t>::max());
                submesh.Vertices.Resize(totalVertices);
                meshopt_remapVertexBuffer(submesh.Vertices.Data(), vertices, totalIndices, sizeof(MeshResourceVertex), remapTable);
            }

            fast_obj_destroy(mesh);
        }

        void CookMesh(std::pmr::u8string const& meshPath, std::pmr::u8string const& cookedMeshPath)
        {
            hdLogInfo(u8"Mesh \"%\" cooking to \"%\" started.", meshPath.c_str(), cookedMeshPath.c_str());
            hdAssert(file::FileExist(meshPath), u8"Mesh source file % doesn't exist.", meshPath.c_str());

            ScopedScratchMemory scopedScratch{};

            std::pmr::vector<MeshResource> meshes{ &mem::Scratch() };
            std::pmr::vector<MaterialResouce> materials{ &mem::Scratch() };

            LoadRawMesh(meshPath, meshes, materials);

            util::CommandBuffer memoryFileStream{ mem::MB(32) };

            uint32_t& magicNumber = memoryFileStream.Write<uint32_t>();
            magicNumber = MESH_MAGIC_NUMBER;

            // Write materials array
            uint32_t& materialCountFile = memoryFileStream.Write<uint32_t>();
            materialCountFile = uint32_t(materials.size());
            for (uint32_t materialIdx = 0; materialIdx < materialCountFile; ++materialIdx)
            {
                math::Vectorf4& diffuseColor = memoryFileStream.Write<math::Vectorf4>();
                diffuseColor = materials[materialIdx].DiffuseColor;

                math::Vectorf4& ambientColor = memoryFileStream.Write<math::Vectorf4>();
                ambientColor = materials[materialIdx].AmbientColor;

                math::Vectorf3& specularColor = memoryFileStream.Write<math::Vectorf3>();
                specularColor = materials[materialIdx].SpecularColor;

                float& specularPower = memoryFileStream.Write<float>();
                specularPower = materials[materialIdx].SpecularPower;

                float& refractionIndex = memoryFileStream.Write<float>();
                refractionIndex = materials[materialIdx].RefractionIndex;

                // #TODO pack texture data into cooked binary
                size_t& diffuseTexturePathLength = memoryFileStream.Write<size_t>();
                diffuseTexturePathLength = materials[materialIdx].DiffuseTexture.size();
                if (diffuseTexturePathLength > 0)
                {
                    char8_t* diffuseTexturePath = &memoryFileStream.Write<char8_t>(diffuseTexturePathLength);
                    memcpy_s(diffuseTexturePath, diffuseTexturePathLength, materials[materialIdx].DiffuseTexture.data(), diffuseTexturePathLength);
                }

                size_t& normalTexturePathLength = memoryFileStream.Write<size_t>();
                normalTexturePathLength = materials[materialIdx].NormalTexture.size();
                if (normalTexturePathLength > 0)
                {
                    char8_t* normalTexturePath = &memoryFileStream.Write<char8_t>(normalTexturePathLength);
                    memcpy_s(normalTexturePath, normalTexturePathLength, materials[materialIdx].NormalTexture.data(), normalTexturePathLength);
                }

                size_t& roughnessTexturePathLength = memoryFileStream.Write<size_t>();
                roughnessTexturePathLength = materials[materialIdx].RoughnessTexture.size();
                if (roughnessTexturePathLength > 0)
                {
                    char8_t* roughnessTexturePath = &memoryFileStream.Write<char8_t>(roughnessTexturePathLength);
                    memcpy_s(roughnessTexturePath, roughnessTexturePathLength, materials[materialIdx].RoughnessTexture.data(), roughnessTexturePathLength);
                }

                size_t& metalnessTexturePathLength = memoryFileStream.Write<size_t>();
                metalnessTexturePathLength = materials[materialIdx].MetalnessTexture.size();
                if (metalnessTexturePathLength > 0)
                {
                    char8_t* metalnessTexturePath = &memoryFileStream.Write<char8_t>(metalnessTexturePathLength);
                    memcpy_s(metalnessTexturePath, metalnessTexturePathLength, materials[materialIdx].MetalnessTexture.data(), metalnessTexturePathLength);
                }
            }

            // Write mesh subobjects
            uint32_t& meshCountFile = memoryFileStream.Write<uint32_t>();
            meshCountFile = uint32_t(meshes.size());
            for (uint32_t meshIdx = 0; meshIdx < meshCountFile; ++meshIdx)
            {
                uint32_t& vertexCount = memoryFileStream.Write<uint32_t>();
                vertexCount = uint32_t(meshes[meshIdx].Vertices.Size());
                MeshResourceVertex* vertices = &memoryFileStream.Write<MeshResourceVertex>(vertexCount);
                memcpy_s(vertices, sizeof(MeshResourceVertex) * vertexCount, meshes[meshIdx].Vertices.Data(), sizeof(MeshResourceVertex) * vertexCount);

                uint32_t& indexCount = memoryFileStream.Write<uint32_t>();
                indexCount = uint32_t(meshes[meshIdx].Indices.Size());
                uint32_t* indices = &memoryFileStream.Write<uint32_t>(indexCount);
                memcpy_s(indices, sizeof(uint32_t) * indexCount, meshes[meshIdx].Indices.Data(), sizeof(uint32_t) * indexCount);

                uint32_t& materialIndex = memoryFileStream.Write<uint32_t>();
                materialIndex = meshes[meshIdx].MaterialIndex;
            }

            hdLogInfo(u8"[Data] Writing cooked file \"%\"", cookedMeshPath.c_str());
            file::WriteWholeFile(cookedMeshPath, memoryFileStream.GetBuffer().GetData(), memoryFileStream.GetBuffer().GetSize());

            hdLogInfo(u8"[Data] Mesh \"%\" cooking to \"%\" finished", meshPath.c_str(), cookedMeshPath.c_str());
        }

        void LoadMesh(char8_t const* fileName, std::pmr::vector<MaterialResouce>& outMaterials, std::pmr::vector<MeshResource>& outMeshes)
        {
            std::pmr::u8string meshFileName{ fileName, &mem::Scratch() };

            std::pmr::u8string cookedMeshFilePath{ &mem::Scratch() };
            file::ConvertToCookedPath(meshFileName, cookedMeshFilePath);
            std::pmr::u8string cookedFileExtension{ u8".bin", &mem::Scratch() };
            file::ReplaceExtension(cookedMeshFilePath, cookedFileExtension, cookedMeshFilePath);

#if defined(HD_ENABLE_RESOURCE_COOKING)
            std::pmr::u8string meshFilePath{ &mem::Scratch() };
            file::ConvertToMediaPath(meshFileName, meshFilePath);
            if (file::DestinationOlder(meshFilePath, cookedMeshFilePath))
            {
                CookMesh(meshFilePath, cookedMeshFilePath);
            }
#endif

            hdEnsure(file::FileExist(cookedMeshFilePath), u8"Mesh file % is not found.", cookedMeshFilePath.c_str());

            util::CommandBuffer memoryFileStream{ mem::MB(32) };

            file::ReadWholeFile(cookedMeshFilePath, memoryFileStream.GetBuffer());

            util::CommandBufferReader fileReader{ memoryFileStream };

            uint32_t& magic = fileReader.Read<uint32_t>();
            hdEnsure(magic == MESH_MAGIC_NUMBER, u8"Mesh file % corrupted.", cookedMeshFilePath.c_str());

            // Read materials array
            uint32_t& materialCount = fileReader.Read<uint32_t>();
            if (materialCount > 0)
            {
                outMaterials.resize(materialCount);
                for (uint32_t materialIdx = 0; materialIdx < materialCount; ++materialIdx)
                {
                    outMaterials[materialIdx].DiffuseColor = fileReader.Read<math::Vectorf4>();
                    outMaterials[materialIdx].AmbientColor = fileReader.Read<math::Vectorf4>();
                    outMaterials[materialIdx].SpecularColor = fileReader.Read<math::Vectorf3>();
                    outMaterials[materialIdx].SpecularPower = fileReader.Read<float>();
                    outMaterials[materialIdx].RefractionIndex = fileReader.Read<float>();

                    size_t& diffuseTexturePathLength = fileReader.Read<size_t>();
                    if (diffuseTexturePathLength > 0)
                    {
                        char8_t* diffusetTexturePath = &fileReader.Read<char8_t>(diffuseTexturePathLength);
                        outMaterials[materialIdx].DiffuseTexture.assign(diffusetTexturePath, diffuseTexturePathLength);
                    }

                    size_t& normalTexturePathLength = fileReader.Read<size_t>();
                    if (normalTexturePathLength > 0)
                    {
                        char8_t* normalTexturePath = &fileReader.Read<char8_t>(normalTexturePathLength);
                        outMaterials[materialIdx].NormalTexture.assign(normalTexturePath, normalTexturePathLength);
                    }

                    size_t& roughnessTexturePathLength = fileReader.Read<size_t>();
                    if (roughnessTexturePathLength > 0)
                    {
                        char8_t* roughnessTexturePath = &fileReader.Read<char8_t>(roughnessTexturePathLength);
                        outMaterials[materialIdx].RoughnessTexture.assign(roughnessTexturePath, roughnessTexturePathLength);
                    }

                    size_t& metalnessTexturePathLength = fileReader.Read<size_t>();
                    if (metalnessTexturePathLength > 0)
                    {
                        char8_t* metalnessTexturePath = &fileReader.Read<char8_t>(metalnessTexturePathLength);
                        outMaterials[materialIdx].MetalnessTexture.assign(metalnessTexturePath, metalnessTexturePathLength);
                    }
                }
            }

            // Write mesh subobjects
            uint32_t& meshCount = fileReader.Read<uint32_t>();
            if (meshCount > 0)
            {
                outMeshes.resize(meshCount);
                for (uint32_t meshIdx = 0; meshIdx < meshCount; ++meshIdx)
                {
                    outMeshes[meshIdx].Vertices.Resize(fileReader.Read<uint32_t>());
                    MeshResourceVertex* vertices = &fileReader.Read<MeshResourceVertex>(outMeshes[meshIdx].Vertices.Size());
                    memcpy_s(outMeshes[meshIdx].Vertices.Data(), outMeshes[meshIdx].Vertices.Size() * sizeof(MeshResourceVertex), vertices,
                        outMeshes[meshIdx].Vertices.Size() * sizeof(MeshResourceVertex));

                    outMeshes[meshIdx].Indices.Resize(fileReader.Read<uint32_t>());
                    uint32_t* indices = &fileReader.Read<uint32_t>(outMeshes[meshIdx].Indices.Size());
                    memcpy_s(outMeshes[meshIdx].Indices.Data(), outMeshes[meshIdx].Indices.Size() * sizeof(uint32_t), indices, outMeshes[meshIdx].Indices.Size() * sizeof(uint32_t));

                    outMeshes[meshIdx].MaterialIndex = fileReader.Read<uint32_t>();
                }
            }
        }

        void LoadImageDefault(char8_t const* filePath, PlainDataArray<std::byte>& outData, ImageResource& outImageDesc)
        {
            stbi_set_flip_vertically_on_load_thread(true);

            int width{};
            int height{};
            int channels{};
            uint8_t* data{};
            size_t pixelSizeInBytes{};

            if (stbi_is_hdr(reinterpret_cast<const char*>(filePath)))
            {
                data = reinterpret_cast<uint8_t*>(stbi_loadf((const char*)filePath, &width, &height, &channels, STBI_rgb_alpha));
                outImageDesc.Format = gfx::GraphicFormat::RGBA32Float;
                pixelSizeInBytes = 16; //Four elements four bytes each
            }
            else
            {
                data = reinterpret_cast<uint8_t*>(stbi_load((const char*)filePath, &width, &height, &channels, STBI_rgb_alpha));
                outImageDesc.Format = gfx::GraphicFormat::RGBA8UNorm;
                pixelSizeInBytes = 4; //Four elements one byte each
            }

            hdEnsure(data != nullptr, u8"Failed to load image %. Error: %", filePath, (const char8_t*)stbi_failure_reason());

            // mips and cubes are not supported by jpeg, png, hdr images
            outImageDesc.MipCount = 0;
            outImageDesc.IsCube = false;

            outImageDesc.Width = uint32_t(width);
            outImageDesc.Height = uint32_t(height);

            outData.Resize(pixelSizeInBytes * outImageDesc.Width * outImageDesc.Height);
            memcpy_s(outData.Data(), outData.Size(), data, outData.Size());

            stbi_image_free(data);
        }

        struct DDS_PIXELFORMAT
        {
            uint32_t Size;
            uint32_t Flags;
            uint32_t FourCC;
            uint32_t RGBBitCount;
            uint32_t RBitMask;
            uint32_t GBitMask;
            uint32_t BBitMask;
            uint32_t ABitMask;
        };

        const uint32_t DDPF_ALPHAPIXELS = 0x00000001;
        const uint32_t DDPF_ALPHA = 0x00000002;
        const uint32_t DDPF_FOURCC = 0x00000004;
        const uint32_t DDPF_RGB = 0x00000040;
        const uint32_t DDPF_YUV = 0x00000200;
        const uint32_t DDPF_LUMINANCE = 0x00020000;

        struct DDS_HEADER
        {
            uint32_t Size;
            uint32_t Flags;
            uint32_t Height;
            uint32_t Width;
            uint32_t PitchOrLinearSize;
            uint32_t Depth;
            uint32_t MipMapCount;
            uint32_t Reserved1[11];
            DDS_PIXELFORMAT ddspf;
            uint32_t Caps;
            uint32_t Caps2;
            uint32_t Caps3;
            uint32_t Caps4;
            uint32_t Reserved2;
        };

        const uint32_t DDSD_CAPS = 0x00000001;
        const uint32_t DDSD_HEIGHT = 0x00000002;
        const uint32_t DDSD_WIDTH = 0x00000004;
        const uint32_t DDSD_PIXELFORMAT = 0x00001000;
        const uint32_t DDSD_MIPMAPCOUNT = 0x00020000;
        const uint32_t DDSD_DEPTH = 0x00800000;

        const auto DDSD_REQUIRED = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

        struct DDS_HEADER_DXT10
        {
            uint32_t dxgiFormat;
            uint32_t resourceDimension;
            uint32_t miscFlag;
            uint32_t arraySize;
            uint32_t miscFlags2;
        };

        const auto DDPF_NOT_SUPPORTED = DDPF_ALPHA | DDPF_YUV | DDPF_LUMINANCE;

        const uint32_t DDS_CUBEMAP_POSITIVEX = 0x00000600;
        const uint32_t DDS_CUBEMAP_NEGATIVEX = 0x00000a00;
        const uint32_t DDS_CUBEMAP_POSITIVEY = 0x00001200;
        const uint32_t DDS_CUBEMAP_NEGATIVEY = 0x00002200;
        const uint32_t DDS_CUBEMAP_POSITIVEZ = 0x00004200;
        const uint32_t DDS_CUBEMAP_NEGATIVEZ = 0x00008200;

        const uint32_t DDS_CUBEMAP_ALLFACES = DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX | DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY | DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ;

        const uint32_t DDS_CUBEMAP = 0x00000200;

        void LoadImageDDS(PlainDataArray<std::byte> const& fileData, PlainDataArray<std::byte>& outData, ImageResource& outImageDesc)
        {
            std::byte const* ddsBytesPointer = fileData.Data();
            size_t ddsBytesSize = fileData.Size();

            hdEnsure(*reinterpret_cast<const uint32_t*>(ddsBytesPointer) == MakeFourCC('D', 'D', 'S', ' '));
            ddsBytesSize -= sizeof(uint32_t);
            ddsBytesPointer += sizeof(uint32_t);


            auto ddsHeader = reinterpret_cast<const DDS_HEADER*>(ddsBytesPointer);
            ddsBytesSize -= sizeof(DDS_HEADER);
            ddsBytesPointer += sizeof(DDS_HEADER);

            hdEnsure((ddsHeader->Flags & DDSD_REQUIRED) == DDSD_REQUIRED);

            outImageDesc.Width = ddsHeader->Width;
            outImageDesc.Height = ddsHeader->Height;

            if ((ddsHeader->Flags & DDSD_DEPTH) == DDSD_DEPTH)
            {
                // #TODO Implement DDS volume textures
                hdEnsure(false, u8"DDS Volume textures not implemented.");
            }

            if ((ddsHeader->Flags & DDSD_MIPMAPCOUNT) == DDSD_MIPMAPCOUNT)
            {
                outImageDesc.MipCount = ddsHeader->MipMapCount;
            }
            else
            {
                outImageDesc.MipCount = 1;
            }

            hdEnsure((ddsHeader->ddspf.Flags & DDPF_NOT_SUPPORTED) == 0, u8"Alpha only, YUV and Luminance DDS textures are not supported.");

            if ((ddsHeader->ddspf.Flags & DDPF_FOURCC) == DDPF_FOURCC)
            {
                if (ddsHeader->ddspf.FourCC == MakeFourCC('D', 'X', '1', '0'))
                {
                    auto ddsDx10Header = reinterpret_cast<const DDS_HEADER_DXT10*>(ddsBytesPointer);
                    ddsBytesSize -= sizeof(DDS_HEADER_DXT10);
                    ddsBytesPointer += sizeof(DDS_HEADER_DXT10);

                    // #TODO Implement DDS textures array
                    hdEnsure(false, u8"Texture arrays are not implemented.");
                }
                else
                {
                    switch (ddsHeader->ddspf.FourCC)
                    {
                    case MakeFourCC('D', 'X', 'T', '1'):
                    {
                        outImageDesc.Format = gfx::GraphicFormat::BC1Unorm;
                        break;
                    }

                    case MakeFourCC('D', 'X', 'T', '2'):
                    {
                        // #TODO Implement DXT2 texture compression loading
                        hdEnsure(false, u8"DXT2 texture compression loading is not implemented.");
                        break;
                    }

                    case MakeFourCC('D', 'X', 'T', '3'):
                    {
                        outImageDesc.Format = gfx::GraphicFormat::BC2Unorm;
                        break;
                    }

                    case MakeFourCC('D', 'X', 'T', '4'):
                    {
                        // #TODO Implement DXT4 texture compression loading
                        hdEnsure(false, u8"DXT4 texture compression loading is not implemented.");
                        break;
                    }

                    case MakeFourCC('D', 'X', 'T', '5'):
                    {
                        outImageDesc.Format = gfx::GraphicFormat::BC3Unorm;
                        break;
                    }

                    case MakeFourCC('B', 'C', '4', 'U'):
                    {
                        outImageDesc.Format = gfx::GraphicFormat::BC4Unorm;
                        break;
                    }

                    case MakeFourCC('B', 'C', '4', 'S'):
                    {
                        outImageDesc.Format = gfx::GraphicFormat::BC4Snorm;
                        break;
                    }

                    case MakeFourCC('B', 'C', '5', 'U'):
                    {
                        outImageDesc.Format = gfx::GraphicFormat::BC5Unorm;
                        break;
                    }

                    case MakeFourCC('B', 'C', '5', 'S'):
                    {
                        outImageDesc.Format = gfx::GraphicFormat::BC5Snorm;
                        break;
                    }

                    default:
                    {
                        hdEnsure(false, u8"Unknown DDS compressed format.");
                    }
                    }
                }
            }
            else if ((ddsHeader->ddspf.Flags & DDPF_RGB) == DDPF_RGB)
            {
                auto hasAlpha = (ddsHeader->ddspf.Flags & DDPF_ALPHAPIXELS) == DDPF_ALPHAPIXELS;

                switch (ddsHeader->ddspf.RGBBitCount)
                {
                case 32:
                {
                    if (hasAlpha)
                    {
                        if (ddsHeader->ddspf.RBitMask == 0x00ff0000 &&
                            ddsHeader->ddspf.GBitMask == 0x0000ff00 &&
                            ddsHeader->ddspf.BBitMask == 0x000000ff &&
                            ddsHeader->ddspf.ABitMask == 0xff000000)
                        {
                            outImageDesc.Format = gfx::GraphicFormat::BGRA8Unorm;
                        }
                        else
                        {
                            hdEnsure(false, u8"#Implement other 32 bit alpha textures.");
                        }
                    }
                    else
                    {
                        if (ddsHeader->ddspf.RBitMask == 0x00ff0000 &&
                            ddsHeader->ddspf.GBitMask == 0x0000ff00 &&
                            ddsHeader->ddspf.BBitMask == 0x000000ff &&
                            ddsHeader->ddspf.ABitMask == 0x00000000)
                        {
                            outImageDesc.Format = gfx::GraphicFormat::BGRX8Unorm;
                        }
                    }

                    break;
                }

                default:
                {
                    hdEnsure(false, u8"#Implement non 32 bit textures.");
                }
                }
            }
            else
            {
                hdEnsure(false, u8"Unknown DDS format.");
            }

            if ((ddsHeader->Caps2 & DDS_CUBEMAP) != 0)
            {
                hdEnsure((ddsHeader->Caps2 & DDS_CUBEMAP_ALLFACES) == DDS_CUBEMAP_ALLFACES);
                outImageDesc.IsCube = true;
            }
            else
            {
                outImageDesc.IsCube = false;
            }

            outData.Resize(ddsBytesSize);
            memcpy_s(outData.Data(), outData.Size(), ddsBytesPointer, ddsBytesSize);
        }

        void LoadImage(char8_t const* fileName, PlainDataArray<std::byte>& outData, ImageResource& outImageDesc)
        {
            PlainDataArray<std::byte> fileData{ &mem::Scratch() };
            std::pmr::u8string filePath{ fileName, &mem::Scratch() };
            file::ReadWholeFile(filePath, fileData);

            uint32_t* ddsHeaderPointer = reinterpret_cast<uint32_t*>(fileData.Data());
            if (*ddsHeaderPointer == MakeFourCC('D', 'D', 'S', ' '))
            {
                LoadImageDDS(fileData, outData, outImageDesc);
            }
            else
            {
                LoadImageDefault(fileName, outData, outImageDesc);
            }
        }
    }
}