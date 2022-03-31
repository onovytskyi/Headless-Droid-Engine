#include "../Source/Shaders/Shared/Common.hlsl"
#include "../Source/Shaders/Shared/PBRUtils.hlsl"
#include "../Source/Shaders/Shared/GeometryUtils.hlsl"

struct FrameData
{
    float4x4 ProjInv;
    float4x4 ViewInv;
    float3 EyePositionWorld;
    float SunIntensity;
    float3 SunDir;
};

uint GetFrameDataBufferIdx()
{
    return GetIndexLiteralHack(g_Constants.Value0);
}

uint GetSurface0TextureIdx()
{
    return GetIndexLiteralHack(g_Constants.Value1);
}

uint GetSurface1TextureIdx()
{
    return GetIndexLiteralHack(g_Constants.Value2);
}

uint GetSurface2TextureIdx()
{
    return GetIndexLiteralHack(g_Constants.Value3);
}

uint GetDepthTextureIdx()
{
    return GetIndexLiteralHack(g_Constants.Value4);
}

void MainPS(in float4 Position : SV_Position, in float2 UV : TexCoord0, out float4 Color : SV_Target0)
{
    ConstantBuffer<FrameData> frameConstants = ResourceDescriptorHeap[GetFrameDataBufferIdx()];
    Texture2D<float4> surface0 = ResourceDescriptorHeap[GetSurface0TextureIdx()]; // Albedo, Albedo, Albedo, Unused
    Texture2D<float4> surface1 = ResourceDescriptorHeap[GetSurface1TextureIdx()]; // Frestel, Roughness, Metalness, Unused
    Texture2D<float4> surface2 = ResourceDescriptorHeap[GetSurface2TextureIdx()]; // Normal, Normal, Normal, Unused
    Texture2D<float> depth = ResourceDescriptorHeap[GetDepthTextureIdx()];

    float4 surface0Sample = surface0.Sample(g_LinearClampSampler, UV);
    float4 surface1Sample = surface1.Sample(g_LinearClampSampler, UV);
    float4 surface2Sample = surface2.Sample(g_LinearClampSampler, UV);
    float3 wsPosition = WSPositionFromDepth(UV, depth.Sample(g_LinearClampSampler, UV).r, frameConstants.ProjInv, frameConstants.ViewInv);

    float3 normal = DecodeNormal(surface2Sample.xyz);
    float3 eyeDirection = normalize(frameConstants.EyePositionWorld - wsPosition);

    MaterialPBR pbrMaterial;
    pbrMaterial.Albedo = surface0Sample.xyz;
    pbrMaterial.Roughness = surface1Sample.y;
    pbrMaterial.Metalness = surface1Sample.z;
    pbrMaterial.FresnelR0 = surface1Sample.x;

    Color.rgb = CookTorrance_GGX(normal, -frameConstants.SunDir, eyeDirection, pbrMaterial)* frameConstants.SunIntensity;

    //Color.rgb *= SampleLightProbe(normal, EnvironmentTexture, BilinearSampler);

    //Color.rgb = Color.rgb / (Color.rgb + 1.0);

    Color.a = 1.0;

    //Color.rgb = surface0Sample.xyz;
}


