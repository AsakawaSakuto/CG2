#include "Particles.hlsli"

static const uint kMaxParticles = 1;
RWStructuredBuffer<Particle> gParticles : register(u0);

[RootSignature("UAV(u0), CBV(b1)")]

// 768以下
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) {
    uint particlesIndex = DTid.x;
    if (particlesIndex < kMaxParticles) {
        gParticles[particlesIndex] = (Particle) 0;
        gParticles[particlesIndex].translate = float3(0.0f, 0.0f, 0.0f);
        gParticles[particlesIndex].scale = float3(1.0f, 1.0f, 1.0f);
        gParticles[particlesIndex].color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}