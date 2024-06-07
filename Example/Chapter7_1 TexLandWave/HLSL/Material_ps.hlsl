#include "Basic.hlsli"
float4 ps(VertexOut pin) : SV_Target
{
    //从纹理中提取此像素的漫反射反照率，并将其与常量缓冲区中的反照率相乘
    float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC) * gDiffuseAlbedo;
   
    // Interpolating normal can unnormalize it, so renormalize it.
    // 对法线插值可能导致其非规范化，因此需要再次对它进行规范化处理
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    // 光线经表面上一点反射到观察点这一方向上的向量
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// Indirect lighting.
    // 间接光照
    float4 ambient = gAmbientLight * diffuseAlbedo;

    // 直接光照
    const float shininess = 1.0f - gRoughness;
    Material mat = { diffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

    // Common convention to take alpha from diffuse material.
    // 从漫反射材质获取alpha值
    litColor.a = diffuseAlbedo.a;

    return litColor;
    
}