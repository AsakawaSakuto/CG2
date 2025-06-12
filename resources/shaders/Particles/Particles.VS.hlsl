#include "Particles.hlsli"

// VertexShader
// 与えられた座標を同次クリップ空間に変換する

struct ParticleForGPU {
    float4x4 WVP;
    float4x4 World;
    float4 color;
};

StructuredBuffer<ParticleForGPU> gParticles : register(t1);

struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input,uint instanceId : SV_InstanceId) {
    VertexShaderOutput output;
    output.position = mul(input.position, gParticles[instanceId].WVP);
    output.texcoord = input.texcord;
    output.normal = normalize(mul(input.normal, (float3x3) gParticles[instanceId].World));
    output.color = gParticles[instanceId].color;
    return output;
}