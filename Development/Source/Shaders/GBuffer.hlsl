#include "../Development/Source/Shaders/Engine/Common.hlsl"
#include "../Development/Source/Shaders/Engine/ColorUtils.hlsl"
#include "../Development/Source/Shaders/Engine/GeometryUtils.hlsl"

struct FrameData
{
    float4x4 World;
    float4x4 WorldInvTrans;
    float4x4 WorldViewProj;
    float3 EyePositionWorld;
};

struct MaterialInfo
{
    float4 DiffuseColor;
    float4 AmbientColor;
    float3 SpecularColor;

    float SpecularPower;
    float FresnelR0;

    uint DiffuseTextureIndex;
    uint NormalTextureIndex;
    uint RoughnessTextureIndex;
    uint MetallnessTextureIndex;
};

struct MeshInfo
{
    uint StartIndex;
    uint StartVertex;
    uint IndexCount;
    uint MaterialIndex;
};

struct Vertex
{
    float3 Position;
    float3 Normal;
    float2 UV;
};

struct ShaderVarying
{
    float3 World : User0;
    float4 Position : SV_Position;
    float3 Normal : User1;
    float2 UV : TexCoord0;
};

struct ShaderOutput
{
    float4 Surface0 : SV_Target0; // Albedo, Albedo, Albedo, Unused
    float4 Surface1 : SV_Target1; // Fresnel, Roughness, Metalness, Unused
    float4 Surface2 : SV_Target2; // Normal, Normal, Normal, Unused
};

uint GetFrameDataBufferIdx()
{
    return GetIndexLiteralHack(g_Constants.Value0);
}

uint GetMaterialInfoBufferIdx()
{
    return GetIndexLiteralHack(g_Constants.Value1);
}

uint GetMeshInfoBufferIdx()
{
    return GetIndexLiteralHack(g_Constants.Value2);
}

uint GetIndexBufferIdx()
{
    return GetIndexLiteralHack(g_Constants.Value3);
}

uint GetVertexBufferIdx()
{
    return GetIndexLiteralHack(g_Constants.Value4);
}

uint GetMeshIdx()
{
    return g_Constants.Value5;
}

void MainVS(in uint VertexID : SV_VertexID, out ShaderVarying Output)
{
    ConstantBuffer<FrameData> frameConstants = ResourceDescriptorHeap[GetFrameDataBufferIdx()];
    StructuredBuffer<MeshInfo> meshes = ResourceDescriptorHeap[GetMeshInfoBufferIdx()];
    StructuredBuffer<uint> indices = ResourceDescriptorHeap[GetIndexBufferIdx()];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[GetVertexBufferIdx()];

    MeshInfo meshInfo = meshes[GetMeshIdx()];

    uint index = indices[meshInfo.StartIndex + VertexID];
    Vertex vertex = vertices[meshInfo.StartVertex + index];

    Output.World = mul(vertex.Position, (float3x3)frameConstants.World);
    Output.Position = mul(float4(vertex.Position, 1.0f), frameConstants.WorldViewProj);
    Output.Normal = mul(vertex.Normal, (float3x3)frameConstants.WorldInvTrans);
    Output.UV = vertex.UV;
}

void MainPS(ShaderVarying Input, out ShaderOutput Output)
{
    ConstantBuffer<FrameData> frameConstants = ResourceDescriptorHeap[GetFrameDataBufferIdx()];
    StructuredBuffer<MeshInfo> meshes = ResourceDescriptorHeap[GetMeshInfoBufferIdx()];
    StructuredBuffer<MaterialInfo> materials = ResourceDescriptorHeap[GetMaterialInfoBufferIdx()];

    float3 normal = normalize(Input.Normal);
    float3 eyeDirection = normalize(frameConstants.EyePositionWorld - Input.World);

    uint materialIdx = meshes[GetMeshIdx()].MaterialIndex;
    MaterialInfo material = materials[materialIdx];

    Output.Surface0 = 1.0;
    Output.Surface1 = 0.0;
    Output.Surface2 = 0.0;

    if (material.DiffuseTextureIndex != -1)
    {
        Texture2D<float4> diffuseTexture = ResourceDescriptorHeap[material.DiffuseTextureIndex];
        float4 albedoSample = diffuseTexture.Sample(g_AnisotropicSampler, Input.UV);
        clip(albedoSample.a - 0.5);
        Output.Surface0.xyz = SRGBToLinear(albedoSample.rgb);
    }

    Output.Surface0.xyz *= material.DiffuseColor.rgb;

    if (material.NormalTextureIndex != -1)
    {
        Texture2D<float4> normalTexture = ResourceDescriptorHeap[material.NormalTextureIndex];
        float3 normalSample = 0.0;
        normalSample.xy = normalTexture.Sample(g_AnisotropicSampler, Input.UV).xy;
        normalSample.xy = 2.0 * normalSample.xy - 1.0;
        normalSample = ExpandNormal(normalSample.xy);

        float3x3 tangentToWorldMatrix = CotangentFrame(normal, -eyeDirection, Input.UV);
        normal = normalize(mul(normalSample, tangentToWorldMatrix));
    }

    Output.Surface2.xyz = EncodeNormal(normal);

    Output.Surface1.x = material.FresnelR0;

    if (material.RoughnessTextureIndex != -1)
    {
        Texture2D<float> roughnessTexture = ResourceDescriptorHeap[material.RoughnessTextureIndex];
        Output.Surface1.y = roughnessTexture.Sample(g_AnisotropicSampler, Input.UV).r;
    }

    if (material.MetallnessTextureIndex != -1)
    {
        Texture2D<float> metallnessTexture = ResourceDescriptorHeap[material.MetallnessTextureIndex];
        Output.Surface1.z = metallnessTexture.Sample(g_AnisotropicSampler, Input.UV).r;
    }
}