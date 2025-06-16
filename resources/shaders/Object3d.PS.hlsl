#include "Object3d.hlsli"

// 定数バッファ（b0）: マテリアル情報
struct Material
{
    float4 color;
    int enableLighting;
    float3 padding1; // ← アライメント調整のため
    float4x4 uvTransform;
    float shininess; // ← ← これが正しい！
    float3 padding2; // ← アライメント調整のため（合計16バイト単位にする）
};

// 定数バッファ（b2）: 平行光源
struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
    float3 padding;
};

// 定数バッファ（b3）: カメラ
struct Camera
{
    float3 worldPosition;
    float padding;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);
ConstantBuffer<Camera> gCamera : register(b3);

// テクスチャとサンプラー
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 出力ピクセル構造体
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // UV変換 + テクスチャ取得
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    float3 N = normalize(input.normal);
    float3 L = normalize(-gDirectionalLight.direction);
    float NdotL = saturate(dot(N, L));

    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    float3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
    float RdotE = dot(reflectLight, toEye);
    //float specularPow = pow(saturate(RdotE), gMaterial.shininess); // 鏡面反射の強度

    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f); // Half Lambert

    float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
    float NDotH = dot(normalize(input.normal), halfVector);
    float specularPow = pow(saturate(NDotH), gMaterial.shininess);
    
    if (gMaterial.enableLighting != 0)
    {
        float3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        output.color.rgb = diffuse + specular;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }

    if (textureColor.a <= 0.5 || output.color.a == 0.0f)
    {
        discard;
    }

    return output;
}
