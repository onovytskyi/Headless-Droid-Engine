#include "Engine/Config/Bootstrap.h"

#include "Engine/Engine/Utils/ResourceLoader.h"

#include "Engine/Debug/Assert.h"
#include "Engine/Debug/Log.h"
#include "Engine/Foundation/Memory/Utils.h"
#include "Engine/Framework/File/Utils.h"
#include "Engine/Framework/String/String.h"
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

        void LoadRawMesh(mem::AllocationScope& scratch, str::String const& filePath, MeshResource*& outMeshes, uint32_t& outMeshCount, MaterialResouce*& outMaterials, 
            uint32_t& outMaterialCount)
        {
            // #HACK Fast obj only supports ASCII in file path, consider rewrite it to use wide chars
            // #Optimization use our allocator with fast obj
            fastObjMesh* mesh = fast_obj_read(reinterpret_cast<char const*>(filePath.CStr()));
            hdAssert(mesh != nullptr, u8"Fast obj failed to load mesh file %s.", filePath.CStr());

            outMaterialCount = mesh->material_count;
            outMaterials = scratch.AllocatePODArray<MaterialResouce>(outMaterialCount);
            for (uint32_t materialIdx = 0; materialIdx < outMaterialCount; ++materialIdx)
            {
                MaterialResouce& material = outMaterials[materialIdx];
                material = {};
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
                    str::String path{ scratch, reinterpret_cast<char8_t*>(materialData.map_Kd.path) };
                    material.DiffuseTexture = path.Str();
                }

                if (materialData.map_bump.path)
                {
                    str::String path{ scratch, reinterpret_cast<char8_t*>(materialData.map_bump.path) };
                    material.NormalTexture = path.Str();
                }

                if (materialData.map_Ns.path)
                {
                    str::String path{ scratch, reinterpret_cast<char8_t*>(materialData.map_Ns.path) };
                    material.RoughnessTexture = path.Str();
                }

                if (materialData.map_Ka.path)
                {
                    str::String path{ scratch, reinterpret_cast<char8_t*>(materialData.map_Ka.path) };
                    material.MetalnessTexture = path.Str();
                }
            }

            outMeshCount = mesh->group_count;
            outMeshes = scratch.AllocatePODArray<MeshResource>(outMeshCount);
            for (uint32_t submeshIdx = 0; submeshIdx < outMeshCount; ++submeshIdx)
            {
                MeshResource& submesh = outMeshes[submeshIdx];
                submesh = {};
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

                MeshResourceVertex* vertices = scratch.AllocatePODArray<MeshResourceVertex>(totalIndices);
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

                uint32_t* remapTable = scratch.AllocatePODArray<uint32_t>(totalIndices);
                size_t totalVertices = meshopt_generateVertexRemap(remapTable, nullptr, totalIndices, vertices, totalIndices, sizeof(MeshResourceVertex));

                hdAssert(totalIndices <= std::numeric_limits<uint32_t>::max());
                submesh.IndexCount = uint32_t(totalIndices);
                submesh.Indices = scratch.AllocatePODArray<uint32_t>(submesh.IndexCount);
                meshopt_remapIndexBuffer(submesh.Indices, nullptr, totalIndices, remapTable);

                hdAssert(totalVertices <= std::numeric_limits<uint32_t>::max());
                submesh.VertexCount = uint32_t(totalVertices);
                submesh.Vertices = scratch.AllocatePODArray<MeshResourceVertex>(submesh.VertexCount);
                meshopt_remapVertexBuffer(submesh.Vertices, vertices, totalIndices, sizeof(MeshResourceVertex), remapTable);
            }

            fast_obj_destroy(mesh);
        }

        void CookMesh(mem::AllocationScope& scratch, str::String const& meshPath, str::String const& cookedMeshPath)
        {
            hdLogInfo(u8"Mesh \"%\" cooking to \"%\" started.", meshPath.CStr(), cookedMeshPath.CStr());
            hdAssert(file::FileExist(meshPath), u8"Mesh source file % doesn't exist.", meshPath.CStr());

            MeshResource* meshes{};
            uint32_t meshCount{};
            MaterialResouce* materials{};
            uint32_t materialCount{};

            LoadRawMesh(scratch, meshPath, meshes, meshCount, materials, materialCount);

            util::CommandBuffer memoryFileStream{ mem::MB(32) };

            uint32_t& magicNumber = memoryFileStream.Write<uint32_t>();
            magicNumber = MESH_MAGIC_NUMBER;

            // Write materials array
            uint32_t& materialCountFile = memoryFileStream.Write<uint32_t>();
            materialCountFile = materialCount;
            for (uint32_t materialIdx = 0; materialIdx < materialCount; ++materialIdx)
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
                diffuseTexturePathLength = materials[materialIdx].DiffuseTexture ? strlen(reinterpret_cast<char const*>(materials[materialIdx].DiffuseTexture)) : 0;
                if (diffuseTexturePathLength > 0)
                {
                    char8_t* diffuseTexturePath = &memoryFileStream.Write<char8_t>(diffuseTexturePathLength);
                    memcpy_s(diffuseTexturePath, diffuseTexturePathLength, materials[materialIdx].DiffuseTexture, diffuseTexturePathLength);
                }

                size_t& normalTexturePathLength = memoryFileStream.Write<size_t>();
                normalTexturePathLength = materials[materialIdx].NormalTexture ? strlen(reinterpret_cast<char const*>(materials[materialIdx].NormalTexture)) : 0;
                if (normalTexturePathLength > 0)
                {
                    char8_t* normalTexturePath = &memoryFileStream.Write<char8_t>(normalTexturePathLength);
                    memcpy_s(normalTexturePath, normalTexturePathLength, materials[materialIdx].NormalTexture, normalTexturePathLength);
                }

                size_t& roughnessTexturePathLength = memoryFileStream.Write<size_t>();
                roughnessTexturePathLength = materials[materialIdx].RoughnessTexture ? strlen(reinterpret_cast<char const*>(materials[materialIdx].RoughnessTexture)) : 0;
                if (roughnessTexturePathLength > 0)
                {
                    char8_t* roughnessTexturePath = &memoryFileStream.Write<char8_t>(roughnessTexturePathLength);
                    memcpy_s(roughnessTexturePath, roughnessTexturePathLength, materials[materialIdx].RoughnessTexture, roughnessTexturePathLength);
                }

                size_t& metalnessTexturePathLength = memoryFileStream.Write<size_t>();
                metalnessTexturePathLength = materials[materialIdx].MetalnessTexture ? strlen(reinterpret_cast<char const*>(materials[materialIdx].MetalnessTexture)) : 0;
                if (metalnessTexturePathLength > 0)
                {
                    char8_t* metalnessTexturePath = &memoryFileStream.Write<char8_t>(metalnessTexturePathLength);
                    memcpy_s(metalnessTexturePath, metalnessTexturePathLength, materials[materialIdx].MetalnessTexture, metalnessTexturePathLength);
                }
            }

            // Write mesh subobjects
            uint32_t& meshCountFile = memoryFileStream.Write<uint32_t>();
            meshCountFile = meshCount;
            for (uint32_t meshIdx = 0; meshIdx < meshCount; ++meshIdx)
            {
                uint32_t& vertexCount = memoryFileStream.Write<uint32_t>();
                vertexCount = meshes[meshIdx].VertexCount;
                MeshResourceVertex* vertices = &memoryFileStream.Write<MeshResourceVertex>(vertexCount);
                memcpy_s(vertices, sizeof(MeshResourceVertex) * vertexCount, meshes[meshIdx].Vertices, sizeof(MeshResourceVertex) * vertexCount);

                uint32_t& indexCount = memoryFileStream.Write<uint32_t>();
                indexCount = meshes[meshIdx].IndexCount;
                uint32_t* indices = &memoryFileStream.Write<uint32_t>(indexCount);
                memcpy_s(indices, sizeof(uint32_t) * indexCount, meshes[meshIdx].Indices, sizeof(uint32_t) * indexCount);

                uint32_t& materialIndex = memoryFileStream.Write<uint32_t>();
                materialIndex = meshes[meshIdx].MaterialIndex;
            }

            hdLogInfo(u8"[Data] Writing cooked file \"%\"", cookedMeshPath.CStr());
            file::WriteWholeFile(cookedMeshPath, memoryFileStream.GetBuffer().GetData(), memoryFileStream.GetBuffer().GetSize());

            hdLogInfo(u8"[Data] Mesh \"%\" cooking to \"%\" finished", meshPath.CStr(), cookedMeshPath.CStr());
        }

        void LoadMesh(mem::AllocationScope& scratch, char8_t const* fileName, mem::Buffer& outMaterials, mem::Buffer& outMeshes)
        {
            str::String meshFileName{ scratch, fileName };

            str::String cookedMeshFilePath{ scratch };
            file::ConvertToCookedPath(scratch, meshFileName, cookedMeshFilePath);
            str::String cookedFileExtension{ scratch, u8".bin" };
            file::ReplaceExtension(scratch, cookedMeshFilePath, cookedFileExtension, cookedMeshFilePath);

#if defined(HD_ENABLE_RESOURCE_COOKING)
            str::String meshFilePath{ scratch };
            file::ConvertToMediaPath(scratch, meshFileName, meshFilePath);
            if (file::DestinationOlder(meshFilePath, cookedMeshFilePath))
            {
                CookMesh(scratch, meshFilePath, cookedMeshFilePath);
            }
#endif

            hdEnsure(file::FileExist(cookedMeshFilePath), u8"Mesh file % is not found.", cookedMeshFilePath.CStr());

            util::CommandBuffer memoryFileStream{ mem::MB(32) };

            file::ReadWholeFile(scratch, cookedMeshFilePath, memoryFileStream.GetBuffer());

            util::CommandBufferReader fileReader{ memoryFileStream };

            uint32_t& magic = fileReader.Read<uint32_t>();
            hdEnsure(magic == MESH_MAGIC_NUMBER, u8"Mesh file % corrupted.", cookedMeshFilePath.CStr());

            // Read materials array
            uint32_t& materialCount = fileReader.Read<uint32_t>();
            if (materialCount > 0)
            {
                outMaterials.Resize(sizeof(MaterialResouce) * materialCount);
                MaterialResouce* materialsArray = outMaterials.GetDataAs<MaterialResouce*>();
                for (uint32_t materialIdx = 0; materialIdx < materialCount; ++materialIdx)
                {
                    materialsArray[materialIdx] = {};
                    materialsArray[materialIdx].DiffuseColor = fileReader.Read<math::Vectorf4>();
                    materialsArray[materialIdx].AmbientColor = fileReader.Read<math::Vectorf4>();
                    materialsArray[materialIdx].SpecularColor = fileReader.Read<math::Vectorf3>();
                    materialsArray[materialIdx].SpecularPower = fileReader.Read<float>();
                    materialsArray[materialIdx].RefractionIndex = fileReader.Read<float>();

                    size_t& diffuseTexturePathLength = fileReader.Read<size_t>();
                    if (diffuseTexturePathLength > 0)
                    {
                        materialsArray[materialIdx].DiffuseTexture = scratch.AllocatePODArray<char8_t>(diffuseTexturePathLength + 1);
                        char8_t* diffusetTexturePath = &fileReader.Read<char8_t>(diffuseTexturePathLength);
                        memcpy_s(materialsArray[materialIdx].DiffuseTexture, diffuseTexturePathLength, diffusetTexturePath, diffuseTexturePathLength);
                        materialsArray[materialIdx].DiffuseTexture[diffuseTexturePathLength] = 0;
                    }

                    size_t& normalTexturePathLength = fileReader.Read<size_t>();
                    if (normalTexturePathLength > 0)
                    {
                        materialsArray[materialIdx].NormalTexture = scratch.AllocatePODArray<char8_t>(normalTexturePathLength + 1);
                        char8_t* normalTexturePath = &fileReader.Read<char8_t>(normalTexturePathLength);
                        memcpy_s(materialsArray[materialIdx].NormalTexture, normalTexturePathLength, normalTexturePath, normalTexturePathLength);
                        materialsArray[materialIdx].NormalTexture[normalTexturePathLength] = 0;
                    }

                    size_t& roughnessTexturePathLength = fileReader.Read<size_t>();
                    if (roughnessTexturePathLength > 0)
                    {
                        materialsArray[materialIdx].RoughnessTexture = scratch.AllocatePODArray<char8_t>(roughnessTexturePathLength + 1);
                        char8_t* roughnessTexturePath = &fileReader.Read<char8_t>(roughnessTexturePathLength);
                        memcpy_s(materialsArray[materialIdx].RoughnessTexture, roughnessTexturePathLength, roughnessTexturePath, roughnessTexturePathLength);
                        materialsArray[materialIdx].RoughnessTexture[roughnessTexturePathLength] = 0;
                    }

                    size_t& metalnessTexturePathLength = fileReader.Read<size_t>();
                    if (metalnessTexturePathLength > 0)
                    {
                        materialsArray[materialIdx].MetalnessTexture = scratch.AllocatePODArray<char8_t>(metalnessTexturePathLength + 1);
                        char8_t* metalnessTexturePath = &fileReader.Read<char8_t>(metalnessTexturePathLength);
                        memcpy_s(materialsArray[materialIdx].MetalnessTexture, metalnessTexturePathLength, metalnessTexturePath, metalnessTexturePathLength);
                        materialsArray[materialIdx].MetalnessTexture[metalnessTexturePathLength] = 0;
                    }
                }
            }

            // Write mesh subobjects
            uint32_t& meshCount = fileReader.Read<uint32_t>();
            if (meshCount > 0)
            {
                outMeshes.Resize(sizeof(MeshResource) * meshCount);
                MeshResource* meshesArray = outMeshes.GetDataAs<MeshResource*>();
                for (uint32_t meshIdx = 0; meshIdx < meshCount; ++meshIdx)
                {
                    meshesArray[meshIdx] = {};

                    meshesArray[meshIdx].VertexCount = fileReader.Read<uint32_t>();
                    meshesArray[meshIdx].Vertices = scratch.AllocatePODArray<MeshResourceVertex>(meshesArray[meshIdx].VertexCount);
                    MeshResourceVertex* vertices = &fileReader.Read<MeshResourceVertex>(meshesArray[meshIdx].VertexCount);
                    memcpy_s(meshesArray[meshIdx].Vertices, meshesArray[meshIdx].VertexCount * sizeof(MeshResourceVertex), vertices, 
                        meshesArray[meshIdx].VertexCount * sizeof(MeshResourceVertex));

                    meshesArray[meshIdx].IndexCount = fileReader.Read<uint32_t>();
                    meshesArray[meshIdx].Indices = scratch.AllocatePODArray<uint32_t>(meshesArray[meshIdx].IndexCount);
                    uint32_t* indices = &fileReader.Read<uint32_t>(meshesArray[meshIdx].IndexCount);
                    memcpy_s(meshesArray[meshIdx].Indices, meshesArray[meshIdx].IndexCount * sizeof(uint32_t), indices, meshesArray[meshIdx].IndexCount * sizeof(uint32_t));

                    meshesArray[meshIdx].MaterialIndex = fileReader.Read<uint32_t>();
                }
            }
        }

        void LoadImageDefault(char8_t const* filePath, mem::Buffer& outData, ImageResource& outImageDesc)
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
            memcpy_s(outData.GetData(), outData.GetSize(), data, outData.GetSize());

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

        void LoadImageDDS(mem::Buffer const& fileData, mem::Buffer& outData, ImageResource& outImageDesc)
        {
            std::byte const* ddsBytesPointer = fileData.GetData();
            size_t ddsBytesSize = fileData.GetSize();

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
            memcpy_s(outData.GetData(), outData.GetSize(), ddsBytesPointer, ddsBytesSize);
        }

        void LoadImage(mem::AllocationScope& scratch, char8_t const* fileName, mem::Buffer& outData, ImageResource& outImageDesc)
        {
            mem::Buffer fileData{ scratch };
            str::String filePath{ scratch, fileName };
            file::ReadWholeFile(scratch, filePath, fileData);

            uint32_t* ddsHeaderPointer = fileData.GetDataAs<uint32_t*>();
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