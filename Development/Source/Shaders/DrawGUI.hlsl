#include "../Development/Source/Shaders/Engine/Common.hlsl"

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

uint GetConstantBufferIdx()
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

void MainVS(in uint VertexID : SV_VertexID, out float4 Position : SV_Position, out float2 UV : TexCoord0, out uint MaterialIdx : User0, out float4 UserColor : User1)
{
    ConstantBuffer<FrameData> frameConstants = ResourceDescriptorHeap[GetConstantBufferIdx()];
    StructuredBuffer<uint> indices = ResourceDescriptorHeap[GetIndexBufferIdx()];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[GetVertexBufferIdx()];

    uint index = indices[GetBaseIndexIdx() + VertexID];
    Vertex vertex = vertices[GetBaseVertexIdx() + index];

    Position = mul(float4(vertex.Position, 0.0, 1.0), frameConstants.GuiProj);
    Position.z = Position.w;
    UV = vertex.UV;
    UserColor = DecompressColor(vertex.Color);
}

void MainPS(float4 Position : SV_Position, float2 UV : TexCoord0, uint MaterialIdx : User0, float4 UserColor : User1, out float4 Color : SV_Target0)
{
    float4 diffuseSample = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (GetTextureIdx() != INVALID_INDEX)
    {
        Texture2D<float4> sourceTexture = ResourceDescriptorHeap[GetTextureIdx()];
        diffuseSample = sourceTexture.Sample(g_LinearClampSampler, UV);
    }

    Color = UserColor * diffuseSample;
}