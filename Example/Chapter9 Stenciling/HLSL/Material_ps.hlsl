#include "Basic.hlsli"
float4 ps(VertexOut pin) : SV_Target
{
    //从纹理中提取此像素的漫反射反照率，并将其与常量缓冲区中的反照率相乘
    float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC) * gDiffuseAlbedo;
  
	// Discard pixel if texture alpha < 0.1.  We do this test as soon 
	// as possible in the shader so that we can potentially exit the
	// shader early, thereby skipping the rest of the shader code.
    // 抛弃alpha值在0.1以下的像素。我们应在像素着色器中尽早执行此测试
    // 以尽快检测出满足条件的像素并退出着色器，从而跳过后续对该像素的处理
    #ifdef ALPHA_TEST
	clip(diffuseAlbedo.a - 0.1f);
#endif
    
    // Interpolating normal can unnormalize it, so renormalize it.
    // 对法线插值可能导致其非规范化，因此需要再次对它进行规范化处理
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    // 光线经表面上一点反射到观察点这一方向上的向量
    float3 toEyeW = gEyePosW - pin.PosW;
    float distToEye = length(toEyeW);
    toEyeW /= distToEye; // normalize

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
    
    // 雾效的实现
#ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogAmount);
#endif
    
    // Common convention to take alpha from diffuse material.
    // 从漫反射材质获取alpha值
    litColor.a = diffuseAlbedo.a;

    return litColor;
    
}