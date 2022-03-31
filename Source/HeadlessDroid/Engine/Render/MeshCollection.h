#pragma once

#include "Framework/Graphics/GraphicsTypes.h"
#include "Framework/Math/Math.h"
#include "Framework/Memory/PlainDataArray.h"

namespace hd
{
    class CommandBuffer;
	class Device;
    struct MeshResourceVertex;

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
            Vectorf4 DiffuseColor;
            Vectorf4 AmbientColor;
            Vectorf3 SpecularColor;

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

            PlainDataArray<MeshResourceVertex> Vertices;
            PlainDataArray<uint32_t> Indices;
            uint32_t MaterialIndex;
        };

        MeshCollection(Device& device, CommandBuffer& graphicsCommands, std::pmr::vector<MaterialData> const& materials, std::pmr::vector<MeshData> const& meshes);
        ~MeshCollection();

        hdNoncopyable(MeshCollection)

        BufferHandle GetMaterialsBuffer() const;
        BufferHandle GetMeshesBuffer() const;
        BufferHandle GetIndicesBuffer() const;
        BufferHandle GetVerticesBuffer() const;

        uint32_t GetMeshCount() const;
        uint32_t GetMeshIndexCount(uint32_t meshIndex) const;

    private:
        struct MeshDrawData
        {
            uint32_t IndexCount;
            MaterialType MeshMaterialType;
        };

        Device& m_Device;

        BufferHandle m_Materials;
        BufferHandle m_Meshes;
        BufferHandle m_Indices;
        BufferHandle m_Vertices;

        PlainDataArray<MeshDrawData> m_DrawData;
    };
}