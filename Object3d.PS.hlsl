#include "Object3d.hlsli"

// PixelShader
// 実際に画面に撃つPixelの色を決めるShader
struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};

struct DirectionalLight
{
    float4 color; // ライトの色
    float3 direction; // ライトの向き
    float intensity; // 輝度
    float3 padding;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

//srvのレジスタ－はt
Texture2D<float4> gTexture : register(t0);
//サンプラーのレジスターはs
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float3 N = normalize(input.normal);
    float3 L = normalize(-gDirectionalLight.direction);

    float NdotL = saturate(dot(N, L));

    float4 texColor = gTexture.Sample(gSampler, input.texcoord);

    if (gMaterial.enableLighting != 0)
    {
        output.color = gMaterial.color * texColor * gDirectionalLight.color * NdotL * gDirectionalLight.intensity;
    }
    else
    {
        output.color = gMaterial.color * texColor;
    }
    
    return output;
}