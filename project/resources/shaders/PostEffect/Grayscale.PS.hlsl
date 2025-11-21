#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<ColorEffectParams> gParams : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャからカラーをサンプリング
    float4 texColor = gTexture.Sample(gSampler, input.texcoord);
    
    // グレースケール変換
    float gray = dot(texColor.rgb, float3(0.299f, 0.587f, 0.114f));
    
    // パラメータの強度に応じて元の色とグレースケールをブレンド
    float3 finalColor = lerp(texColor.rgb, float3(gray, gray, gray), gParams.intensity);
    
    output.color = float4(finalColor, texColor.a);
    
    return output;
}
