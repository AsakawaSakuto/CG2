#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<VignetteParams> gParams : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャからカラーをサンプリング
    float4 texColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 画面中心からの距離を計算（0.0～1.0）
    float2 center = float2(0.5f, 0.5f);
    float dist = length(input.texcoord - center);
    
    // パラメータを使用してビネット効果を計算
    float vignette = smoothstep(gParams.radius, gParams.radius * gParams.smoothness, dist);
    vignette = 1.0f - (vignette * gParams.strength);
    
    // カラーにビネット効果を適用、アルファは保持
    output.color = float4(texColor.rgb * vignette, texColor.a);
    
    return output;
}
