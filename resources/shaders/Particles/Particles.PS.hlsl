#include "Particles.hlsli"

// PixelShader
// 実際に画面に撃つPixelの色を決めるShader
struct Material {
    float4 color;
    float4x4 uvTransform;
};

struct DirectionalLight {
    float4 color;     // ライトの色
    float3 direction; // ライトの向き
    float intensity;  // 輝度
    float3 padding;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

//srvのレジスタ－はt
Texture2D<float4> gTexture : register(t0);
//サンプラーのレジスターはs
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    float3 N = normalize(input.normal);
    float3 L = normalize(-gDirectionalLight.direction);

    float NdotL = saturate(dot(N, L));

    float4 texColor = gTexture.Sample(gSampler, input.texcoord);

    // HarfLambert
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    output.color = gMaterial.color * textureColor * input.color;
    if (output.color.a == 0.0) {
        discard;
    }
    
    return output;
}