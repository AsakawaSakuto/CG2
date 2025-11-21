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

    // RGB値を反転、アルファは保持
    output.color = float4(1.0f - texColor.rgb, texColor.a);
    
    return output;
}
