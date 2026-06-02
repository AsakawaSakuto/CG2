#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
Texture2D<float> gMaskTexture : register(t1); // ディゾルブ用のマスク（ノイズ）テクスチャ
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // マスクテクスチャから値をサンプリング
    float mask = gMaskTexture.Sample(gSampler, input.texcoord);

    // 【1. 2値抜き（ディゾルブ消滅処理）】
    // サンプリングした値が閾値（Threshold）以下ならピクセルを破棄して描画スキップ
    if (mask <= gDissolveParams.threshold)
    {
        discard;
    }

    // 元の画像をサンプリングしてベースカラーにする
    output.color = gTexture.Sample(gSampler, input.texcoord);

    // 【2. エッジの簡易検出と色付け】
    // 閾値（threshold）から 閾値+エッジ幅（threshold + edgeRange）までの間を 0.0 ～ 1.0 に補間
    // 1.0 から引くことで、閾値に一番近い部分（＝境界線のギリギリ）ほど値が 1.0 に近づく
    float edge = 1.0f - smoothstep(gDissolveParams.threshold, gDissolveParams.threshold + gDissolveParams.edgeRange, mask);

    // エッジ強度に基づいて、指定されたエッジカラー（RGB）を加算
    output.color.rgb += edge * gDissolveParams.edgeColor;

    return output;
}