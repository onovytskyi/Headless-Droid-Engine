#pragma once

#include "Engine/Framework/Graphics/GraphicsTypes.h"
#include "Engine/Framework/Math/Math.h"
#include "Engine/Framework/Utils/BufferArray.h"

namespace hd
{
    namespace gfx
    {
        class Device;
    }

    namespace mem
    {
        class AllocationScope;
    }

    namespace util
    {
        class CommandBuffer;
        struct MeshResourceVertex;
    }

    namespace render
    {
        class MeshCollection
        {
        public:
            enum class MaterialType : uint32_t
            {
                Opaque,
                Transparent
            };

            struct MaterialData
            {
                math::Vectorf4 DiffuseColor;
                math::Vectorf4 AmbientColor;
                math::Vectorf3 SpecularColor;

                float SpecularPower;
                float FresnelR0;

                uint32_t DiffuseTextureIdx;
                uint32_t NormalTextureIdx;
                uint32_t RoughnessTextureIdx;
                uint32_t MetalnessTextureIdx;
            };

            struct MeshData
            {
                util::MeshResourceVertex* Vertices;
                uint32_t VertexCount;
                uint32_t* Indices;
                uint32_t IndexCount;
                uint32_t MaterialIndex;
            };

            MeshCollection(mem::AllocationScope& allocationScope, gfx::Device& device, util::CommandBuffer& graphicsCommands, util::BufferArray<MaterialData> const& materials,
                util::BufferArray<MeshData> const& meshes);
            ~MeshCollection();

            hdNoncopyable(MeshCollection)

            gfx::BufferHandle GetMaterialsBuffer() const;
            gfx::BufferHandle GetMeshesBuffer() const;
            gfx::BufferHandle GetIndicesBuffer() const;
            gfx::BufferHandle GetVerticesBuffer() const;

            uint32_t GetMeshCount() const;
            uint32_t GetMeshIndexCount(uint32_t meshIndex) const;

        private:
            struct MeshDrawData
            {
                uint32_t IndexCount;
                MaterialType MeshMaterialType;
            };

            gfx::Device& m_Device;

            gfx::BufferHandle m_Materials;
            gfx::BufferHandle m_Meshes;
            gfx::BufferHandle m_Indices;
            gfx::BufferHandle m_Vertices;

            util::BufferArray<MeshDrawData> m_DrawData;
        };
    }
}