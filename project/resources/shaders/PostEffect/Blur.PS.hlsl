#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<BlurParams> gParams : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャのサイズを取得
    float2 texSize;
    gTexture.GetDimensions(texSize.x, texSize.y);
    
    // ピクセルサイズを計算
    float2 pixelSize = 1.0f / texSize;
    
    // ブラー効果を適用
    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    // パラメータから値を取得
    float blurAmount = gParams.amount;
    
    // 9タップブラー、3x3
    color += gTexture.Sample(gSampler, input.texcoord + float2(-pixelSize.x, -pixelSize.y) * blurAmount).rgb * 0.0625f;
    color += gTexture.Sample(gSampler, input.texcoord + float2(0.0f, -pixelSize.y) * blurAmount).rgb * 0.125f;
    color += gTexture.Sample(gSampler, input.texcoord + float2(pixelSize.x, -pixelSize.y) * blurAmount).rgb * 0.0625f;
    
    color += gTexture.Sample(gSampler, input.texcoord + float2(-pixelSize.x, 0.0f) * blurAmount).rgb * 0.125f;
    color += gTexture.Sample(gSampler, input.texcoord).rgb * 0.25f;
    color += gTexture.Sample(gSampler, input.texcoord + float2(pixelSize.x, 0.0f) * blurAmount).rgb * 0.125f;
    
    color += gTexture.Sample(gSampler, input.texcoord + float2(-pixelSize.x, pixelSize.y) * blurAmount).rgb * 0.0625f;
    color += gTexture.Sample(gSampler, input.texcoord + float2(0.0f, pixelSize.y) * blurAmount).rgb * 0.125f;
    color += gTexture.Sample(gSampler, input.texcoord + float2(pixelSize.x, pixelSize.y) * blurAmount).rgb * 0.0625f;
    
    // アルファは中央のピクセルから取得
    float alpha = gTexture.Sample(gSampler, input.texcoord).a;
    
    output.color = float4(color, alpha);
    
    return output;
}
