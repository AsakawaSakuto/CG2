#include "Particles.hlsli"

// インスタンシング用GPU構造体
struct ParticleForGPU {
    float4x4 WVP;
    float4x4 World;
    float4 color;
};

StructuredBuffer<ParticleForGPU> gParticles : register(t1);

// 頂点シェーダー出力構造体
struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcord : TEXCOORD0;
    float3 normal : NORMAL0;
};

// main
VertexShaderOutput main(VertexShaderInput input,uint instanceId : SV_InstanceId) {
    VertexShaderOutput output;
    output.position = mul(input.position, gParticles[instanceId].WVP);
    output.texcoord = input.texcord;
    output.normal = normalize(mul(input.normal, (float3x3) gParticles[instanceId].World));
    output.color = gParticles[instanceId].color;
    return output;
}