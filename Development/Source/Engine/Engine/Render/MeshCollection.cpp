#include "Engine/Config/Bootstrap.h"

#include "Engine/Engine/Memory/EngineMemoryInterface.h"
#include "Engine/Engine/Render/MeshCollection.h"
#include "Engine/Engine/Utils/ResourceLoader.h"
#include "Engine/Framework/Graphics/Device.h"
#include "Engine/Framework/Graphics/GraphicCommands.h"
#include "Engine/Framework/Math/Math.h"
#include "Engine/Framework/Memory/PlainDataArray.h"

namespace hd
{
    namespace render
    {
        struct MeshInfo
        {
            uint32_t StartIndex;
            uint32_t StartVertex;
            uint32_t IndexCount;
            uint32_t MaterialIndex;
        };

        struct FrameData
        {
            math::Matrix4x4 ViewProj;
        };

        MeshCollection::MeshData::MeshData(allocator_type allocator)
            : Vertices{ allocator }
            , Indices{ allocator }
            , MaterialIndex{}
        {

        }

        MeshCollection::MeshData::MeshData(MeshData const& other, allocator_type const& allocator)
            : Vertices{ other.Vertices, allocator }
            , Indices{ other.Indices, allocator }
            , MaterialIndex{ other.MaterialIndex }
        {

        }

        MeshCollection::MeshData::MeshData(MeshData&& other, allocator_type const& allocator)
            : Vertices{ std::move(other.Vertices), allocator }
            , Indices{ std::move(other.Indices), allocator }
            , MaterialIndex{ other.MaterialIndex }
        {

        }

        MeshCollection::MeshCollection(gfx::Device& device, util::CommandBuffer& graphicsCommands, std::pmr::vector<MaterialData> const& materials, std::pmr::vector<MeshData> const& meshes)
            : m_Device{ device }
            , m_Materials{}
            , m_Meshes{}
            , m_Indices{}
            , m_Vertices{}
            , m_DrawData{ &mem::General() }
        {
            m_DrawData.Resize(meshes.size());

            gfx::GraphicCommandsStream commandStream{ graphicsCommands };

            m_Materials = m_Device.CreateBuffer(uint32_t(materials.size()), sizeof(MaterialData), gfx::BufferFlagsBits::ShaderResource);
            commandStream.UpdateBuffer(m_Materials, 0, materials.data(), materials.size() * sizeof(MaterialData));

            uint32_t indexCount{};
            uint32_t vertexCount{};
            for (MeshData const& meshData : meshes)
            {
                indexCount += uint32_t(meshData.Indices.Size());
                vertexCount += uint32_t(meshData.Vertices.Size());
            }

            m_Indices = m_Device.CreateBuffer(indexCount, sizeof(uint32_t), gfx::BufferFlagsBits::ShaderResource);
            m_Vertices = m_Device.CreateBuffer(vertexCount, sizeof(util::MeshResourceVertex), gfx::BufferFlagsBits::ShaderResource);

            ScopedScratchMemory scopedScratch{};

            PlainDataArray<MeshInfo> gpuMeshInfo{ meshes.size(), &mem::Scratch() };

            uint32_t startVertex{};
            uint32_t startIndex{};
            for (uint32_t meshIdx = 0; meshIdx < meshes.size(); ++meshIdx)
            {
                gpuMeshInfo[meshIdx].StartIndex = startIndex;
                gpuMeshInfo[meshIdx].StartVertex = startVertex;
                gpuMeshInfo[meshIdx].IndexCount = uint32_t(meshes[meshIdx].Indices.Size());
                gpuMeshInfo[meshIdx].MaterialIndex = meshes[meshIdx].MaterialIndex;

                commandStream.UpdateBuffer(m_Indices, startIndex * sizeof(uint32_t), meshes[meshIdx].Indices.Data(), meshes[meshIdx].Indices.Size() * sizeof(uint32_t));
                commandStream.UpdateBuffer(m_Vertices, startVertex * sizeof(util::MeshResourceVertex), meshes[meshIdx].Vertices.Data(), 
                    meshes[meshIdx].Vertices.Size() * sizeof(util::MeshResourceVertex));

                m_DrawData[meshIdx] = { gpuMeshInfo[meshIdx].IndexCount, MaterialType::Opaque };

                startIndex += uint32_t(meshes[meshIdx].Indices.Size());
                startVertex += uint32_t(meshes[meshIdx].Vertices.Size());
            }

            m_Meshes = m_Device.CreateBuffer(uint32_t(meshes.size()), sizeof(MeshInfo), gfx::BufferFlagsBits::ShaderResource);
            commandStream.UpdateBuffer(m_Meshes, 0, gpuMeshInfo.Data(), gpuMeshInfo.Size() * sizeof(MeshInfo));

            // After initial setup those resources must be always in readable state.
            commandStream.UseAsReadableResource(m_Materials);
            commandStream.UseAsReadableResource(m_Meshes);
            commandStream.UseAsReadableResource(m_Indices);
            commandStream.UseAsReadableResource(m_Vertices);
        }

        MeshCollection::~MeshCollection()
        {
            m_Device.DestroyBuffer(m_Materials);
            m_Device.DestroyBuffer(m_Meshes);
            m_Device.DestroyBuffer(m_Indices);
            m_Device.DestroyBuffer(m_Vertices);
        }

        gfx::BufferHandle MeshCollection::GetMaterialsBuffer() const
        {
            return m_Materials;
        }

        gfx::BufferHandle MeshCollection::GetMeshesBuffer() const
        {
            return m_Meshes;
        }

        gfx::BufferHandle MeshCollection::GetIndicesBuffer() const
        {
            return m_Indices;
        }

        gfx::BufferHandle MeshCollection::GetVerticesBuffer() const
        {
            return m_Vertices;
        }

        uint32_t MeshCollection::GetMeshCount() const
        {
            return uint32_t(m_DrawData.Size());
        }

        uint32_t MeshCollection::GetMeshIndexCount(uint32_t meshIndex) const
        {
            return m_DrawData[meshIndex].IndexCount;
        }
    }
}