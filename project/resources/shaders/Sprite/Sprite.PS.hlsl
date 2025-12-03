#include "Sprite.hlsli"

ConstantBuffer<SpriteMaterial> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(SpriteVertexOutput input)
{
    PixelShaderOutput output;
    
    // Sample the texture
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // Multiply texture color by material color (includes alpha)
    output.color = textureColor * input.color;
    
    return output;
}
