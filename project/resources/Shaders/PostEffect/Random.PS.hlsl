#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// 疑似乱数生成関数（スライド定番の UV から 0.0 ～ 1.0 の乱数を作るロジック）
float random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898f, 78.233f))) * 43758.5453f);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // 元の画像をサンプリング
    float4 baseColor = gTexture.Sample(gSampler, input.texcoord);

    // テクスチャ座標に時間（time）を掛け合わせることで、毎フレーム動くノイズにする
    // （※もし静止したザラザラ感にしたい場合は、+ gRandomNoiseParams.time を削除してください）
    float2 seed = input.texcoord + float2(gRandomNoiseParams.time, -gRandomNoiseParams.time);
    
    // 0.0 ～ 1.0 のノイズ値を取得
    float noise = random(seed);

    // ノイズを -0.5 ～ 0.5 の範囲に補正し、強度（intensity）を掛けて元の色に加算
    output.color.rgb = baseColor.rgb + (noise - 0.5f) * gRandomNoiseParams.intensity;
    output.color.a = baseColor.a; // アルファ値は維持

    return output;
}