#pragma once

#include "Engine/Framework/Graphics/GraphicsTypes.h"
#include "Engine/Framework/Math/Math.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
        class Buffer;
    }

    namespace util
    {
        struct MaterialResouce
        {
            math::Vectorf4 DiffuseColor;
            math::Vectorf4 AmbientColor;
            math::Vectorf3 SpecularColor;

            float SpecularPower;
            float RefractionIndex;

            char8_t* DiffuseTexture;
            char8_t* NormalTexture;
            char8_t* RoughnessTexture;
            char8_t* MetalnessTexture;
        };

        struct MeshResourceVertex
        {
            math::Vectorf3 Position;
            math::Vectorf3 Normal;
            math::Vectorf2 UV;
        };

        struct MeshResource
        {
            MeshResourceVertex* Vertices;
            uint32_t VertexCount;
            uint32_t* Indices;
            uint32_t IndexCount;
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

        void LoadMesh(mem::AllocationScope& scratch, char8_t const* fileName, mem::Buffer& outMaterials, mem::Buffer& outMeshes);
        void LoadImage(mem::AllocationScope& scratch, char8_t const* fileName, mem::Buffer& outData, ImageResource& outImageDesc);
    }
}