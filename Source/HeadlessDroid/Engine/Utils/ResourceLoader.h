#pragma once

#include "Framework/Graphics/GraphicsTypes.h"
#include "Framework/Math/Math.h"
#include "Framework/Memory/PlainDataArray.h"

namespace hd
{
    namespace util
    {
        struct MaterialResouce
        {
            using allocator_type = std::pmr::polymorphic_allocator<char>;

            explicit MaterialResouce(allocator_type allocator);
            MaterialResouce(MaterialResouce const& other, allocator_type const& allocator);
            MaterialResouce(MaterialResouce&& other, allocator_type const& allocator);
            MaterialResouce& operator=(MaterialResouce const& other) = default;
            MaterialResouce& operator=(MaterialResouce&& other) = default;

            math::Vectorf4 DiffuseColor;
            math::Vectorf4 AmbientColor;
            math::Vectorf3 SpecularColor;

            float SpecularPower;
            float RefractionIndex;

            std::pmr::u8string DiffuseTexture;
            std::pmr::u8string NormalTexture;
            std::pmr::u8string RoughnessTexture;
            std::pmr::u8string MetalnessTexture;
        };

        struct MeshResourceVertex
        {
            math::Vectorf3 Position;
            math::Vectorf3 Normal;
            math::Vectorf2 UV;
        };

        struct MeshResource
        {
            using allocator_type = std::pmr::polymorphic_allocator<char>;

            explicit MeshResource(allocator_type allocator);
            MeshResource(MeshResource const& other, allocator_type const& allocator);
            MeshResource(MeshResource&& other, allocator_type const& allocator);
            MeshResource& operator=(MeshResource const& other) = default;
            MeshResource& operator=(MeshResource&& other) = default;

            PlainDataArray<MeshResourceVertex> Vertices;
            PlainDataArray<uint32_t> Indices;
            uint32_t MaterialIndex;
        };

        struct ImageResource
        {
            uint32_t Width;
            uint32_t Height;
            uint32_t MipCount;
            bool	 IsCube;
            gfx::GraphicFormat Format;
        };

        void LoadMesh(char8_t const* fileName, std::pmr::vector<MaterialResouce>& outMaterials, std::pmr::vector<MeshResource>& outMeshes);
        void LoadImage(char8_t const* fileName, PlainDataArray<std::byte>& outData, ImageResource& outImageDesc);
    }
}