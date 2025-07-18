#include "Particles.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);

[RootSignature("UAV(u0)")]

// 768以下
[numthreads(kMaxParticles, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) {
    uint particlesIndex = DTid.x;
    if (particlesIndex < kMaxParticles) {
        gParticles[particlesIndex] = (Particle) 0;
        gParticles[particlesIndex].translate = float3(0.0f, 0.0f, 0.0f);
        gParticles[particlesIndex].scale = float3(1.0f, 1.0f, 1.0f);
        gParticles[particlesIndex].color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}