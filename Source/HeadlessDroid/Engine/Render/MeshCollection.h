#pragma once

#include "Framework/Graphics/GraphicsTypes.h"
#include "Framework/Math/Math.h"
#include "Framework/Memory/PlainDataArray.h"

namespace hd
{
    namespace gfx
    {
        class Device;
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
                using allocator_type = std::pmr::polymorphic_allocator<char>;

                explicit MeshData(allocator_type allocator);
                MeshData(MeshData const& other, allocator_type const& allocator);
                MeshData(MeshData&& other, allocator_type const& allocator);
                MeshData& operator=(MeshData const& other) = default;
                MeshData& operator=(MeshData&& other) = default;

                PlainDataArray<util::MeshResourceVertex> Vertices;
                PlainDataArray<uint32_t> Indices;
                uint32_t MaterialIndex;
            };

            MeshCollection(gfx::Device& device, util::CommandBuffer& graphicsCommands, std::pmr::vector<MaterialData> const& materials, std::pmr::vector<MeshData> const& meshes);
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

            PlainDataArray<MeshDrawData> m_DrawData;
        };
    }
}