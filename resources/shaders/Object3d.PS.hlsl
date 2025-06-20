#include "Object3d.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);
ConstantBuffer<Camera> gCamera : register(b3);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    float3 N = normalize(input.normal);
    float3 L = normalize(-gDirectionalLight.direction);
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    float3 halfVector = normalize(L + toEye);

    float NdotL = saturate(dot(N, L));
    float NdotH = saturate(dot(N, halfVector));

    float halfLambert = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float specularPow = pow(NdotH, gMaterial.shininess);

    if (gMaterial.enableLighting != 0)
    {
        float3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * halfLambert * gDirectionalLight.intensity;
        float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow;
        output.color.rgb = diffuse + specular;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }

    if (output.color.a <= 0.5f)
    {
        discard;
    }

    return output;
}
