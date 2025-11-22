#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);        // 元のカラー
Texture2D<float> gDepthTexture : register(t1);    // Depth
SamplerState gSamplerLinear : register(s0);       // カラー用
SamplerState gSamplerPoint : register(s1);        // Depth用 Point

// フォグ効果用パラメータ
cbuffer FogParams : register(b0)
{
    float4x4 projectionInverse;   // P^-1（NDC→View変換用）
    float3 fogColor;              // フォグの色
    float fogStart;               // フォグ開始距離
    float fogEnd;                 // フォグ終了距離
    float fogDensity;             // フォグの濃度
    float2 padding;               // パディング
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 元のカラーを取得
    float3 srcColor = gTexture.Sample(gSamplerLinear, input.texcoord).rgb;
    
    // Depthを取得（PointSampler使用）
    float ndcDepth = gDepthTexture.Sample(gSamplerPoint, input.texcoord);
    
    // NDC → Viewに逆変換してView空間のZ値を取得
    float4 viewPos = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), projectionInverse);
    float viewZ = viewPos.z / viewPos.w;
    
    // カメラからの距離を計算（View空間のZ値の絶対値）
    float distance = abs(viewZ);
    
    // 線形フォグファクターを計算
    // fogStart以下では0（フォグなし）、fogEnd以上では1（完全にフォグ）
    float fogFactor = saturate((distance - fogStart) / (fogEnd - fogStart));
    
    // フォグ濃度を適用
    fogFactor *= fogDensity;
    
    // 元の色とフォグ色を線形補間
    output.color.rgb = lerp(srcColor, fogColor, fogFactor);
    output.color.a = 1.0f;
    
    return output;
}
