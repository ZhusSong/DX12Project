#include"Basic.hlsli"
float4 ps(VertexOut pin) : SV_Target
{
 // Interpolating normal can unnormalize it, so renormalize it.
      // �Է��߲�ֵ���ܵ�����ǹ淶���������Ҫ�ٴζ������й淶������
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    // ���߾�������һ�㷴�䵽�۲����һ�����ϵ�����
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// Indirect lighting.
    // ��ӹ���
    float4 ambient = gAmbientLight * gDiffuseAlbedo;

    // ֱ�ӹ���
    const float shininess = 1.0f - gRoughness;
    Material mat = { gDiffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

    // Common convention to take alpha from diffuse material.
    // ����������ʻ�ȡalphaֵ
    litColor.a = gDiffuseAlbedo.a;

    return litColor;
}
