#include "Engine/Config/Bootstrap.h"

#include "Engine/Engine/Render/MeshCollection.h"
#include "Engine/Engine/Utils/ResourceLoader.h"
#include "Engine/Framework/Graphics/Device.h"
#include "Engine/Framework/Graphics/GraphicCommands.h"
#include "Engine/Framework/Math/Math.h"
#include "Engine/Framework/Memory/AllocationScope.h"
#include "Engine/Framework/Memory/FrameworkMemoryInterface.h"

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

        MeshCollection::MeshCollection(mem::AllocationScope& allocationScope, gfx::Device& device, util::CommandBuffer& graphicsCommands, util::BufferArray<MaterialData> const& materials,
            util::BufferArray<MeshData> const& meshes)
            : m_Device{ device }
            , m_Materials{}
            , m_Meshes{}
            , m_Indices{}
            , m_Vertices{}
            , m_DrawData{ allocationScope, meshes.GetSize() }
        {
            gfx::GraphicCommandsStream commandStream{ graphicsCommands };

            m_Materials = m_Device.CreateBuffer(uint32_t(materials.GetSize()), sizeof(MaterialData), uint32_t(gfx::BufferFlags::ShaderResource));
            commandStream.UpdateBuffer(m_Materials, 0, (void*)materials.GetData(), materials.GetSize() * sizeof(MaterialData));

            uint32_t indexCount{};
            uint32_t vertexCount{};
            for (MeshData const& meshData : meshes)
            {
                indexCount += meshData.IndexCount;
                vertexCount += meshData.VertexCount;
            }

            m_Indices = m_Device.CreateBuffer(indexCount, sizeof(uint32_t), uint32_t(gfx::BufferFlags::ShaderResource));
            m_Vertices = m_Device.CreateBuffer(vertexCount, sizeof(util::MeshResourceVertex), uint32_t(gfx::BufferFlags::ShaderResource));

            mem::AllocationScope scratchScope{ mem::GetScratchAllocator() };

            util::BufferArray<MeshInfo> gpuMeshInfo{ scratchScope, meshes.GetSize() };
            gpuMeshInfo.ResizeToMax();

            uint32_t startVertex{};
            uint32_t startIndex{};
            for (uint32_t meshIdx = 0; meshIdx < meshes.GetSize(); ++meshIdx)
            {
                gpuMeshInfo[meshIdx].StartIndex = startIndex;
                gpuMeshInfo[meshIdx].StartVertex = startVertex;
                gpuMeshInfo[meshIdx].IndexCount = meshes[meshIdx].IndexCount;
                gpuMeshInfo[meshIdx].MaterialIndex = meshes[meshIdx].MaterialIndex;

                commandStream.UpdateBuffer(m_Indices, startIndex * sizeof(uint32_t), meshes[meshIdx].Indices, meshes[meshIdx].IndexCount * sizeof(uint32_t));
                commandStream.UpdateBuffer(m_Vertices, startVertex * sizeof(util::MeshResourceVertex), meshes[meshIdx].Vertices, meshes[meshIdx].VertexCount * sizeof(util::MeshResourceVertex));

                MeshDrawData drawData{ gpuMeshInfo[meshIdx].IndexCount, MaterialType::Opaque };
                m_DrawData.Add(drawData);

                startIndex += meshes[meshIdx].IndexCount;
                startVertex += meshes[meshIdx].VertexCount;
            }

            m_Meshes = m_Device.CreateBuffer(uint32_t(meshes.GetSize()), sizeof(MeshInfo), uint32_t(gfx::BufferFlags::ShaderResource));
            commandStream.UpdateBuffer(m_Meshes, 0, gpuMeshInfo.GetData(), gpuMeshInfo.GetSize() * sizeof(MeshInfo));
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
            return uint32_t(m_DrawData.GetSize());
        }

        uint32_t MeshCollection::GetMeshIndexCount(uint32_t meshIndex) const
        {
            return m_DrawData[meshIndex].IndexCount;
        }
    }
}