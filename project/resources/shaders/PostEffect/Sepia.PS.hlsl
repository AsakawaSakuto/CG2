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
    
    // セピア調変換
    float3 sepia;
    sepia.r = dot(texColor.rgb, float3(0.393f, 0.769f, 0.189f));
    sepia.g = dot(texColor.rgb, float3(0.349f, 0.686f, 0.168f));
    sepia.b = dot(texColor.rgb, float3(0.272f, 0.534f, 0.131f));
    
    // パラメータの強度に応じて元の色とセピアをブレンド
    float3 finalColor = lerp(texColor.rgb, sepia, gParams.intensity);
    
    output.color = float4(finalColor, texColor.a);
    
    return output;
}
