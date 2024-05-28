#include"06_basic.hlsli"
float4 ps(VertexOut pin) : SV_Target
{
    return pin.Color;
}
