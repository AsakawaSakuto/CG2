#include "Object3d.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);
ConstantBuffer<Camera> gCamera : register(b3);
ConstantBuffer<PointLight> gPointLight : register(b4);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // UV変換 + テクスチャカラー取得
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    // 共通データ
    float3 N = normalize(input.normal);
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

    // === Directional Light ===
    float3 L1 = normalize(-gDirectionalLight.direction);
    float3 halfL1 = normalize(L1 + toEye);
    float NdotL1 = saturate(dot(N, L1));
    float NdotH1 = saturate(dot(N, halfL1));
    float diffuseFactor1 = pow(NdotL1 * 0.5f + 0.5f, 2.0f);
    float specularFactor1 = pow(NdotH1, gMaterial.shininess);

    // === Point Light ===
    float3 L2 = normalize(gPointLight.position - input.worldPosition);
    float distance = length(gPointLight.position - input.worldPosition);

    // 減衰計算（画像の式を反映）
    float attenuation = pow(saturate(-distance / gPointLight.radius + 1.0), gPointLight.decay);

    float3 halfL2 = normalize(L2 + toEye);
    float NdotL2 = saturate(dot(N, L2));
    float NdotH2 = saturate(dot(N, halfL2));
    float diffuseFactor2 = pow(NdotL2 * 0.5f + 0.5f, 2.0f);
    float specularFactor2 = pow(NdotH2, gMaterial.shininess);

    if (gMaterial.enableLighting != 0)
    {
        float3 diffuse1 = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * diffuseFactor1 * gDirectionalLight.intensity;
        float3 specular1 = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularFactor1;

        float3 diffuse2 = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * diffuseFactor2 * gPointLight.intensity * attenuation;
        float3 specular2 = gPointLight.color.rgb * gPointLight.intensity * specularFactor2 * attenuation;

        output.color.rgb = diffuse1 + specular1 + diffuse2 + specular2;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }

    if (output.color.a <= 0.5f)
    {
        discard;
    }

    return output;
}
