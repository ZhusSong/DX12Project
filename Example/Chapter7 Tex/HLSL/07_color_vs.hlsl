#include"Basic.hlsli"

VertexOut vs(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;
	
    // Transform to world space.
    // 将顶点变换到世界空间
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    // 假设这里进行的是等比缩放，否则这里需要使用的是世界矩阵的逆转置矩阵
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);

    // Transform to homogeneous clip space.
    // 将顶点变换到齐次裁剪空间
    vout.PosH = mul(posW, gViewProj);

    return vout;
}