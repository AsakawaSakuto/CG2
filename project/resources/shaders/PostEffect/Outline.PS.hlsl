#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);        // 元のカラー
Texture2D<float> gDepthTexture : register(t1);    // Depth
SamplerState gSamplerLinear : register(s0);       // カラー用
SamplerState gSamplerPoint : register(s1);        // Depth用 Point

// アウトライン効果用パラメータ
cbuffer OutlineParams : register(b0)
{
    float4x4 projectionInverse;   // P^-1
    float2 uvStepSize;            // (1/width, 1/height)
    float thickness;              // アウトラインの太さ
    float depthSensitivity;       // 深度の感度
    float3 outlineColor;          // アウトラインの色
    float padding;                // パディング
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// Prewitt Kernel（資料の値そのまま）
static const float kPrewittHorizontal[3][3] = {
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};

static const float kPrewittVertical[3][3] = {
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // Prewittフィルタ積み込み結果
    float2 difference = float2(0.0f, 0.0f);
    
   
    // Depth → View Z に変換して使う
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            float2 offset = float2((x - 1), (y - 1)) * uvStepSize * thickness;
            float2 fetchUV = input.texcoord + offset;
            
            // Depthをそのまま取る、PointSampler使用
            float ndcDepth = gDepthTexture.Sample(gSamplerPoint, fetchUV);
            
            // NDC → Viewに逆変換、資料の通りZだけ使う
            float4 viewPos = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), projectionInverse);
            float viewZ = viewPos.z / viewPos.w;
            
            // 横・縦方向 Prewitt
            difference.x += viewZ * kPrewittHorizontal[x][y];
            difference.y += viewZ * kPrewittVertical[x][y];
        }
    }
    
    // 差分の長さをweightとする
    float weight = length(difference) * depthSensitivity;
    weight = saturate(weight); // 0〜1
    
    // 元画像と合成
    float3 srcColor = gTexture.Sample(gSamplerLinear, input.texcoord).rgb;
    
    // アウトライン色を適用（weightが大きいほどアウトライン色が強くなる）
    output.color.rgb = lerp(srcColor, outlineColor, weight);
    output.color.a = 1.0f;
    
    return output;
}
