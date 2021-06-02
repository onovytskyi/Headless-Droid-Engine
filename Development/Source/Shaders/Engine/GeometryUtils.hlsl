#ifndef _HLSL_GEOMETRY_UTILS_
#define _HLSL_GEOMETRY_UTILS_

// Stolen here http://www.thetenthplanet.de/archives/1180
float3x3 CotangentFrame(float3 N, float3 p, float2 uv)
{
    // get edge vectors of the pixel triangle
    float3 dp1 = ddx(p);
    float3 dp2 = ddy(p);
    float2 duv1 = ddx(uv);
    float2 duv2 = ddy(uv);

    // solve the linear system
    float3 dp2perp = cross(dp2, N);
    float3 dp1perp = cross(N, dp1);
    float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    float3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // construct a scale-invariant frame
    float invmax = rsqrt(max(dot(T, T), dot(B, B)));
    return float3x3(T * invmax, B * invmax, N);
}

float3 ExpandNormal(float2 xyNormal)
{
    return float3(xyNormal.x, xyNormal.y, sqrt(1.0 - xyNormal.x * xyNormal.x - xyNormal.y * xyNormal.y));
}

float3 VSPositionFromDepth(float2 uv, float depth, float4x4 invProjection)
{
    // Get x/w and y/w from the viewport position
    float x = uv.x * 2 - 1;
    float y = (1 - uv.y) * 2 - 1;
    float4 vProjectedPos = float4(x, y, depth, 1.0);
    // Transform by the inverse projection matrix
    float4 vPositionVS = mul(vProjectedPos, invProjection);
    // Divide by w to get the view-space position
    return vPositionVS.xyz / vPositionVS.w;
}

float3 WSPositionFromDepth(float2 uv, float depth, float4x4 invProjection, float4x4 invView)
{
    float4 vPositionVS = float4(VSPositionFromDepth(uv, depth, invProjection), 1.0);
    return mul(vPositionVS, invView).xyz;
}

float3 EncodeNormal(float3 normal)
{
    return normal * 0.5 + 0.5;
}
float3 DecodeNormal(float3 normal)
{
    return normal * 2.0 - 1.0;
}
#endif