// 由于将Light数组放置在了渲染过程常量缓冲区，因此光源数量不能超过16
#define MaxLights 16
struct Light
{
    float3 Strength;
    float FalloffStart; // point/spot light only
    float3 Direction; // directional/spot light only
    float FalloffEnd; // point/spot light only
    float3 Position; // point light only
    float SpotPower; // spot light only
};
struct Material
{
    float4 DiffuseAlbedo;
    float3 FrresnelR0;
    float Shininess;
};
float CalcAttenuation(float d,float falloffStart,float falloffEnd)
{
    // Linear falloff.
    // 计算线性衰减
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}


// Schlick gives an approximation to Fresnel reflectance (see pg. 233 "Real-Time Rendering 3rd Ed.").
// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction.
// 使用石里克近似法计算反射率
// 参见Real-Time Rndering 3rd Ed page233
// R0=((n-1)/(n+1))^2,n为折射率
float3 SchlickFrensnel(float3 R0,float3 normal,float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));
    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);
    return reflectPercent;
}
// 计算光照反射模型
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
    // m由光泽度推导而来，而光泽度根据粗糙度求得 
    const float m = mat.Shininess * 256.0f;
    float3 halfVec = normalize(toEye + lightVec);
    
    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    float3 fresnelFactor = SchlickFrensnel(mat.FrresnelR0, halfVec, lightVec);
    
    
    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
    // 尽管我们进行的是LDR(low dynamic range，低动态范围)渲染，
    // 但spec(镜面反射)公式得到的结果仍会超出[0,1],因此现将其按比例缩小一些
    float3 specAlbedo = fresnelFactor * roughnessFactor;
    
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);
    return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

// Evaluates the lighting equation for directional lights.
// 实现方向光
float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
    // The light vector aims opposite the direction the light rays travel.
    // 光向量与光线传播方向相反
    float3 lightVec = -L.Direction;
    
    // Scale light down by Lambert's cosine law.
    // 通过朗伯余弦定理按比例降低光强
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;
    return BlinnPhong(lightStrength,lightVec,normal,toEye,mat);
}
// Evaluates the lighting equation for point lights.
// 实现点光
float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // The vector from the surface to the light.
    // 自表面指向光源的向量
    float3 lightVec = L.Position - pos;
    
    // The distance from surface to light.
    // 从表面到光源的距离
    float d = length(lightVec);
    
    // Range test.
    // 范围检测
    if (d>L.FalloffEnd)
        return 0.0f;
    
    // Normalize the light vector.
    // 规范化光向量
    lightVec /= d;
    
    // Scale light down by Lambert's cosine law.
    // 通过郎伯余弦定理按比例降低光强
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;
    
    // Attenuate light by distance.
    // 根据距离计算光衰减
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;
    
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// Evaluates the lighting equation for spot lights.
// 实现聚光灯
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    
    // The vector from the surface to the light.
    // 自表面指向光源的向量
    float3 lightVec = L.Position - pos;
    
    // The distance from surface to light.
    // 从表面到光源的距离
    float d = length(lightVec);
    
    // Range test.
    //范围检测
    if (d > L.FalloffEnd)
        return 0.0f;
    
    // Normalize the light vector.
    // 规范化光向量
    lightVec /= d;
    
    // Scale light down by Lambert's cosine law.
    // 通过郎伯余弦定理按比例降低光强
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;
    
    // Attenuate light by distance.
    // 根据距离计算光衰减
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;
    
    // Scale by spotlight
    // 根据聚光灯照明模型缩放光强
    float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
    lightStrength *= spotFactor;
    
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//计算场景中各种光的混和

float4 ComputeLighting(Light gLights[MaxLights], Material mat,
                       float3 pos, float3 normal, float3 toEye,
                       float3 shadowFactor)
{
    float3 result = 0.0f;

    int i = 0;

#if (NUM_DIR_LIGHTS > 0)
    for(i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, normal, toEye);
    }
#endif

#if (NUM_POINT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS+NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(gLights[i], mat, pos, normal, toEye);
    }
#endif

#if (NUM_SPOT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);
    }
#endif 

    return float4(result, 0.0f);
}