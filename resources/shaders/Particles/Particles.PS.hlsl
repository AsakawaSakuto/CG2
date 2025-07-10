#include "Particles.hlsli"

// マテリアル構造体 b0
struct Material {
    float4 color;
    float4x4 uvTransform;
};

// 平行光源 b2
struct DirectionalLight {
    float4 color;
    float3 direction;
    float intensity;
    float3 padding;
};

// 定数バッファ
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

// テクスチャとサンプラー
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 出力構造体
struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

// main
PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    float3 N = normalize(input.normal);
    float3 L = normalize(-gDirectionalLight.direction);
    float NdotL = saturate(dot(N, L));

    // HarfLambert
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    output.color = gMaterial.color * textureColor * input.color;
    if (output.color.a == 0.0) {
        discard;
    }
    return output;
}