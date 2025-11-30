#include "Sprite.hlsli"

ConstantBuffer<SpriteMaterial> gMaterial : register(b0);
ConstantBuffer<SpriteTransformationMatrix> gTransform : register(b1);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

SpriteVertexOutput main(SpriteVertexInput input)
{
    SpriteVertexOutput output;
    
    // Transform position to clip space
    output.position = mul(input.position, gTransform.WVP);
    
    // Transform UV coordinates
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransformMatrix);
    output.texcoord = transformedUV.xy;
    
    // Pass through normal (not used for sprites but kept for consistency)
    output.normal = input.normal;
    
    // Pass through material color
    output.color = gMaterial.color;
    
    return output;
}
