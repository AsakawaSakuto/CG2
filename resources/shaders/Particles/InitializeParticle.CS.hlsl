#include "Particles.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);
ConstantBuffer<EmitterSphere> gEmitter : register(b5);

[RootSignature("UAV(u0), UAV(u1), UAV(u2), CBV(b5)")]

// 768以下
[numthreads(512, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) {
    uint particlesIndex = DTid.x;
    if (particlesIndex == 0)
    {
        gFreeListIndex[0] = gEmitter.kMaxParticle - 1;
    }
    
    if (particlesIndex < gEmitter.kMaxParticle)
    {
        gParticles[particlesIndex] = (Particle) 0;
        gFreeList[particlesIndex] = particlesIndex;
    }
}