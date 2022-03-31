#ifndef _HLSL_PBR_UTILS_
#define _HLSL_PBR_UTILS_

static const float PI = 3.14159265f;

struct MaterialPBR
{
    float3 Albedo;
    float Roughness;
    float Metalness;
    float3 FresnelR0;
};

float GGX_PartialGeometry(float cosThetaN, float alpha)
{
    float cosTheta_sqr = saturate(cosThetaN * cosThetaN);
    float tan2 = (1 - cosTheta_sqr) / cosTheta_sqr;
    float GP = 2 / (1 + sqrt(1 + alpha * alpha * tan2));
    return GP;
}

float GGX_Distribution(float cosThetaNH, float alpha)
{
    float alpha2 = alpha * alpha;
    float NH_sqr = saturate(cosThetaNH * cosThetaNH);
    float den = NH_sqr * alpha2 + (1.0 - NH_sqr);
    return alpha2 / (PI * den * den);
}

float3 FresnelSchlick(float3 F0, float cosThetaNL)
{
    return F0 + (1.0 - F0) * pow(1.0 - saturate(cosThetaNL), 5.0);
}

float3 CookTorrance_GGX(float3 n, float3 l, float3 v, MaterialPBR m)
{
    n = normalize(n);
    v = normalize(v);
    l = normalize(l);
    float3 h = normalize(v + l);
    //precompute dots
    float NL = dot(n, l);
    if (NL <= 0.0) return 0.0;
    float NV = dot(n, v);
    if (NV <= 0.0) return 0.0;
    float NH = dot(n, h);
    float HV = dot(h, v);

    //precompute roughness square
    float roug_sqr = m.Roughness * m.Roughness;

    //calc coefficients
    float G = GGX_PartialGeometry(NV, roug_sqr) * GGX_PartialGeometry(NL, roug_sqr);
    float D = GGX_Distribution(NH, roug_sqr);
    float3 F = FresnelSchlick(m.FresnelR0, HV);

    //mix
    float3 specK = G * D * F * 0.25 / NV;
    float3 diffK = saturate(1.0 - F);
    return max(0.0, m.Albedo * diffK * NL / PI + specK);
}

#endif