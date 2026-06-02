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
    output.color = gTexture.Sample(gSampler, input.texcoord);
    return output;
}

// ランダムノイズ効果用パラメータ（16バイトアライメント）
struct RandomNoiseParams
{
    float intensity; // ノイズの強度 (0.0 ～ 1.0)
    float time; // 時間経過（毎フレーム動かす用）
    float2 padding; // C++側の padding[2] に対応
};

// ランダムノイズ用定数バッファのバインド（例としてレジスタ b3 にバインド）
ConstantBuffer<RandomNoiseParams> gRandomNoiseParams : register(b3);