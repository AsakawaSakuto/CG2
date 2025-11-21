#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャからカラーをサンプリング
    float4 texColor = gTexture.Sample(gSampler, input.texcoord);
    
    // グレースケール変換（NTSC係数を使用）
    // 人間の目の感度に基づいた加重平均
    float gray = dot(texColor.rgb, float3(0.299f, 0.587f, 0.114f));
    
    // グレースケール値をRGBに適用、アルファは保持
    output.color = float4(gray, gray, gray, texColor.a);
    
    return output;
}
