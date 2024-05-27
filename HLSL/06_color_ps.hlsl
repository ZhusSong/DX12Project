#include"06_basic.hlsli"
float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color;
}
