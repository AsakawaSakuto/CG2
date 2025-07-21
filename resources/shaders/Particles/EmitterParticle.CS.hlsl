#include "Particles.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);
ConstantBuffer<EmitterSphere> gEmitter : register(b5);
ConstantBuffer<PerFrame> gPerFrame : register(b6);

[RootSignature("UAV(u0), UAV(u1), UAV(u2), CBV(b5), CBV(b6)")]

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (gEmitter.emit != 0)
    {
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            int freeLsitIndex;
            InterlockedAdd(gFreeListIndex[0], -1, freeLsitIndex);
            if (0 <= freeLsitIndex && freeLsitIndex < gEmitter.kMaxParticle)
            {
                uint particleIndex = gFreeList[freeLsitIndex];
                uint baseSeed = particleIndex + countIndex * 12345 + gPerFrame.index * 6789;
                
                gParticles[particleIndex].scale = float3(1.0f, 1.0f, 1.0f);
                gParticles[particleIndex].translate = GenerateSpherePosition(baseSeed);
                gParticles[particleIndex].color.rgb = GenerateColor(baseSeed + 3000);
                gParticles[particleIndex].color.a = 1.0f;
                gParticles[particleIndex].rotate = float3(0.0f, 0.0f, 0.0f);
                gParticles[particleIndex].velocity = GenerateSpherePosition(baseSeed + 1000) * RandomRange(baseSeed + 2000, 0.2f, 1.0f);
                gParticles[particleIndex].velocity.z = 0.0f;
                gParticles[particleIndex].lifeTime = 1.0f;
                gParticles[particleIndex].currentTime = 0.0f;
            } 
            else 
            {
                InterlockedAdd(gFreeListIndex[0], 1);
                break;
            }
        }
    }
}
