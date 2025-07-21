#include "Particles.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

[RootSignature("UAV(u0), UAV(u1), UAV(u2)")]

// 768以下
[numthreads(kMaxParticles, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) {
    uint particlesIndex = DTid.x;
    if (particlesIndex == 0) {
        gFreeListIndex[0] = kMaxParticles - 1;
    }
    
    if (particlesIndex < kMaxParticles) {
        gParticles[particlesIndex] = (Particle) 0;
        gFreeList[particlesIndex] = particlesIndex;
    }
}