#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct GaussianFilterParams
{
    float sigma;
    float3 padding;
};

ConstantBuffer<GaussianFilterParams> gParams : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// πの定義（HLSLでは定義されていないため自分で定義）
static const float PI = 3.14159265f;

// 2次元ガウス関数
float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // テクスチャサイズを取得してテクセルサイズを計算
    int width, height;
    gTexture.GetDimensions(width, height);
    float2 texelSize = float2(1.0f / (float) width, 1.0f / (float) height);

    float4 sum = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float weight = 0.0f;
    
    // 標準偏差σ (定数バッファから取得)
    float sigma = gParams.sigma;

    // 3x3 ガウシアンフィルタ（インデックスを -1 ～ 1 でループ）
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            // ガウス関数によって現在のピクセル（中心）からの距離に応じた重みを算出
            float w = gauss((float) x, (float) y, sigma);

            // テクスチャサンプリングのオフセット計算
            float2 offset = float2((float) x, (float) y) * texelSize;

            // 重みを掛けて色を加算
            sum += gTexture.Sample(gSampler, input.texcoord + offset) * w;

            // 正規化のために重みの合計を蓄積
            weight += w;
        }
    }

    // 畳み込み後の値を正規化（合計が1になるように調整）
    output.color = sum * rcp(weight);

    return output;
}