#ifndef _HLSL_COMMON_
#define _HLSL_COMMON_

struct RootVariables
{
    uint Value0;
    uint Value1;
    uint Value2;
    uint Value3;
    uint Value4;
    uint Value5;
    uint Value6;
    uint Value7;
    uint Value8;
    uint Value9;

    uint Value10;
    uint Value11;
    uint Value12;
    uint Value13;
    uint Value14;
    uint Value15;
    uint Value16;
    uint Value17;
    uint Value18;
    uint Value19;

    uint Value20;
    uint Value21;
    uint Value22;
    uint Value23;
    uint Value24;
    uint Value25;
    uint Value26;
    uint Value27;
    uint Value28;
    uint Value29;

    uint Value30;
    uint Value31;
};

ConstantBuffer<RootVariables> g_Constants : register(b0, space0);

sampler g_PointClampSampler : register(s0, space0);
sampler g_LinearClampSampler : register(s1, space0);
sampler g_AnisotropicSampler : register(s2, space0);

static const uint INVALID_INDEX = -1;

// #HACK On NVIDIA indexing into ResourceDescriptorHeap using root constant directly lead to crash. Use this as a workaround.
uint GetIndexLiteralHack(uint index)
{
    if (index == INVALID_INDEX)
    {
        return index;
    }

    return (index << 1) >> 1;
}

#endif