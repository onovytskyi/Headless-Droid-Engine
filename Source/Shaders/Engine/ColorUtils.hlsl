#ifndef _HLSL_COLOR_UTILS_
#define _HLSL_COLOR_UTILS_

float PiecewiseLinearToSRGB(float component)
{
    if (component <= 0.0031308)
    {
        return component * 12.92;
    }
    else
    {
        return 1.055 * pow(component, 1.0 / 2.4) - 0.055;
    }
}

float PiecewiseSRGBToLinear(float component)
{
    if (component <= 0.04045)
    {
        return component / 12.92;
    }
    else
    {
        return pow((component + 0.055) / 1.055, 2.4);
    }
}

float3 SRGBToLinear(float3 color)
{
    return float3(
        PiecewiseSRGBToLinear(color.r),
        PiecewiseSRGBToLinear(color.g),
        PiecewiseSRGBToLinear(color.b));
}

#endif