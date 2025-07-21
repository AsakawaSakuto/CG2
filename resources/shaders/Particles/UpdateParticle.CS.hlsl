#include "Particles.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);
ConstantBuffer<EmitterSphere> gEmitter : register(b5);
ConstantBuffer<PerFrame> gPerFrame : register(b6);

[RootSignature("UAV(u0), UAV(u1), UAV(u2), CBV(b5), CBV(b6)")]

// 768以下
[numthreads(512, 1, 1)]

void main( uint3 DTid : SV_DispatchThreadID ) {
    uint particleIndex = DTid.x;
    if (particleIndex < gEmitter.kMaxParticle)
    {
        if (gParticles[particleIndex].color.a != 0.0f)
        {
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].color.a = saturate(alpha);
        }
        
        if (gParticles[particleIndex].color.a == 0.0f)
        {
            gParticles[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);
            int freeListIndex;
            InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
            if ((freeListIndex + 1) < gEmitter.kMaxParticle)
            {
                gFreeList[freeListIndex + 1] = particleIndex;
            }
            else
            {
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            }
        }
    }
}