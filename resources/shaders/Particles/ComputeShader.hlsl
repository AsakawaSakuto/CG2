#include "Particles.hlsli"

static const uint kMaxParticles = 512;
RWStructuredBuffer<Particle> gParticles : register(u0);

[RootSignature("UAV(u0)")]

// 768以下
[numthreads(512, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) {
    uint particlesIndex = DTid.x;
    if (particlesIndex < kMaxParticles) {
        gParticles[particlesIndex] = (Particle) 0;
        gParticles[particlesIndex].scale = float3(0.5f, 0.5f, 0.5f);
        gParticles[particlesIndex].color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}