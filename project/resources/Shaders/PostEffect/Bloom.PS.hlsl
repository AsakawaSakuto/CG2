#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ブルーム用パラメータ
struct BloomParams
{
    float threshold;
    float intensity;
    float blurWidth;
    float padding;
};

ConstantBuffer<BloomParams> gParams : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// 輝度計算
float Luminance(float3 color)
{
    return dot(color, float3(0.299f, 0.587f, 0.114f));
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャのサイズを取得
    float2 texSize;
    gTexture.GetDimensions(texSize.x, texSize.y);
    
    // ピクセルサイズを計算
    float2 pixelSize = 1.0f / texSize;
    
    // 元の色を取得
    float4 originalColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 輝度を計算
    float luminance = Luminance(originalColor.rgb);
    
    // しきい値以上の明るい部分を抽出
    float3 bloom = float3(0.0f, 0.0f, 0.0f);
    if (luminance > gParams.threshold)
    {
        bloom = originalColor.rgb * (luminance - gParams.threshold);
    }
    
    // ブラー効果を適用（簡易的な9タップブラー）
    float3 blurredBloom = float3(0.0f, 0.0f, 0.0f);
    float blurAmount = gParams.blurWidth;
    
    // 9タップブラー（3x3）
    blurredBloom += gTexture.Sample(gSampler, input.texcoord + float2(-pixelSize.x, -pixelSize.y) * blurAmount).rgb * 0.0625f;
    blurredBloom += gTexture.Sample(gSampler, input.texcoord + float2(0.0f, -pixelSize.y) * blurAmount).rgb * 0.125f;
    blurredBloom += gTexture.Sample(gSampler, input.texcoord + float2(pixelSize.x, -pixelSize.y) * blurAmount).rgb * 0.0625f;
    
    blurredBloom += gTexture.Sample(gSampler, input.texcoord + float2(-pixelSize.x, 0.0f) * blurAmount).rgb * 0.125f;
    blurredBloom += gTexture.Sample(gSampler, input.texcoord).rgb * 0.25f;
    blurredBloom += gTexture.Sample(gSampler, input.texcoord + float2(pixelSize.x, 0.0f) * blurAmount).rgb * 0.125f;
    
    blurredBloom += gTexture.Sample(gSampler, input.texcoord + float2(-pixelSize.x, pixelSize.y) * blurAmount).rgb * 0.0625f;
    blurredBloom += gTexture.Sample(gSampler, input.texcoord + float2(0.0f, pixelSize.y) * blurAmount).rgb * 0.125f;
    blurredBloom += gTexture.Sample(gSampler, input.texcoord + float2(pixelSize.x, pixelSize.y) * blurAmount).rgb * 0.0625f;
    
    // 輝度抽出した部分にブラーをかける
    float3 finalBloom = blurredBloom;
    if (Luminance(blurredBloom) > gParams.threshold)
    {
        finalBloom = blurredBloom * (Luminance(blurredBloom) - gParams.threshold);
    }
    else
    {
        finalBloom = float3(0.0f, 0.0f, 0.0f);
    }
    
    // 元の色とブルームを加算合成
    float3 finalColor = originalColor.rgb + finalBloom * gParams.intensity;
    
    output.color = float4(finalColor, originalColor.a);
    
    return output;
}
