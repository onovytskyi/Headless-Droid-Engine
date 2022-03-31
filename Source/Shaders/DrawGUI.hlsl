#include "../Source/Shaders/Shared/Common.hlsl"

struct FrameData
{
    float4x4 GuiProj;
};

struct Vertex
{
    float2 Position;
    float2 UV;
    uint Color;
};

uint GetFrameDataBufferIdx()
{
    return GetIndexLiteralHack(g_Constants.Value0);
}

uint GetIndexBufferIdx()
{
    return GetIndexLiteralHack(g_Constants.Value1);
}

uint GetVertexBufferIdx()
{
    return GetIndexLiteralHack(g_Constants.Value2);
}

uint GetBaseIndexIdx()
{
    return g_Constants.Value3;
}

uint GetBaseVertexIdx()
{
    return g_Constants.Value4;
}

uint GetTextureIdx()
{
    return GetIndexLiteralHack(g_Constants.Value5);
}

float4 DecompressColor(uint color)
{
    float4 result;

    result.a = ((color & 0xff000000) >> 24) / 255.0;
    result.b = ((color & 0xff0000) >> 16) / 255.0;
    result.g = ((color & 0xff00) >> 8) / 255.0;
    result.r = ((color & 0xff)) / 255.0;

    return result;
}

struct ShaderVarying
{
    float4 Position : SV_Position;
    float2 UV : TexCoord0;
    uint MaterialIdx : User0;
    float4 UserColor : User1;
};

void MainVS(in uint VertexID : SV_VertexID, out ShaderVarying Output)
{
    ConstantBuffer<FrameData> frameConstants = ResourceDescriptorHeap[GetFrameDataBufferIdx()];
    StructuredBuffer<uint> indices = ResourceDescriptorHeap[GetIndexBufferIdx()];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[GetVertexBufferIdx()];

    uint index = indices[GetBaseIndexIdx() + VertexID];
    Vertex vertex = vertices[GetBaseVertexIdx() + index];

    Output.Position = mul(float4(vertex.Position, 0.0, 1.0), frameConstants.GuiProj);
    Output.Position.z = Output.Position.w;
    Output.UV = vertex.UV;
    Output.UserColor = DecompressColor(vertex.Color);
}

void MainPS(ShaderVarying Input, out float4 Color : SV_Target0)
{
    float4 textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (GetTextureIdx() != INVALID_INDEX)
    {
        Texture2D<float4> sourceTexture = ResourceDescriptorHeap[GetTextureIdx()];
        textureColor = sourceTexture.Sample(g_LinearClampSampler, Input.UV);
    }

    Color = Input.UserColor * textureColor;
}