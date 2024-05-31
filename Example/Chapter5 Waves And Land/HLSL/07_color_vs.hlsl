#include"07_basic.hlsli"
VertexOut vs(VertexIn vin)
{
    VertexOut vout;
	
	// Transform to homogeneous clip space.
    // 将顶点变换到齐次剪裁空间
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(posW, gViewProj);
	
	// Just pass vertex color into the pixel shader.
    // 直接向像素着色器传递顶点的颜色数据
    vout.Color = vin.Color;
    
    return vout;
}