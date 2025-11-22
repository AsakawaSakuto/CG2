#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ラジアルブラー用パラメータ
struct RadialBlurParams
{
    float centerX;
    float centerY;
    float strength;
    int sampleCount;
};

ConstantBuffer<RadialBlurParams> gParams : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    // ブラーの中心座標
    float2 kCenter = float2(gParams.centerX, gParams.centerY);
    uint kNumSamples = gParams.sampleCount;
    float kBlurWidth = gParams.strength;
    
    float2 direction = input.texcoord - kCenter;
    float3 outputColor = float3(0.0f, 0.0f, 0.0f);
    for (uint sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex)
    {
    // 現在のuvからさほど計算した方向にサンプリング点を進めながらサンプリングしていく
        float2 texcoord = input.texcoord + direction * kBlurWidth * float(sampleIndex);
        outputColor.rgb += gTexture.Sample(gSampler, texcoord).rgb;
    }
    
    // 平均化する
    outputColor.rgb *= rcp(float(kNumSamples));

    PixelShaderOutput output;
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;

}
