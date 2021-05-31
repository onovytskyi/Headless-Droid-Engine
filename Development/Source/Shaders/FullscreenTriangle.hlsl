

void MainVS(in uint VertexID : SV_VertexID, out float4 Position : SV_Position, out float2 UV : TexCoord0, out uint MaterialIdx : User0, out float4 UserColor : User1)
{
    Position.x = (float)(VertexID / 2) * 4.0 - 1.0;
    Position.y = (float)(VertexID % 2) * 4.0 - 1.0;
    Position.z = 0.0;
    Position.w = 1.0;

    UV.x = (float)(VertexID / 2) * 2.0;
    UV.y = 1.0 - (float)(VertexID % 2) * 2.0;

    MaterialIdx = -1;

    UserColor = (float4)1;
}