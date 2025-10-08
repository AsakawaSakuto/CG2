#include "Particles.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);
ConstantBuffer<EmitterSphere> gEmitter : register(b5);
ConstantBuffer<PerFrame> gPerFrame : register(b6);
ConstantBuffer<EmitterRange> gRange : register(b7);

[RootSignature("UAV(u0), UAV(u1), UAV(u2), CBV(b5), CBV(b6), CBV(b7)")]

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (gEmitter.emit != 0)
    {
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            int freeListIndex;
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);

            if (freeListIndex > 0)
            {
                uint particleIndex = gFreeList[freeListIndex - 1];
                uint baseSeed = particleIndex + countIndex * 12345 + gPerFrame.index * 6789;

                if (gEmitter.enableScaleFade == 0)
                {
                    gParticles[particleIndex].scale = lerp(gRange.minScale, gRange.maxScale, RandomFloat(baseSeed + 500));
                }
                else
                {
                    gParticles[particleIndex].scale.x = gEmitter.startScale.x;
                    gParticles[particleIndex].scale.y = gEmitter.startScale.y;
                    gParticles[particleIndex].scale.z = 0.0f;
                }
                
                if (gEmitter.enableColorFade == 0)
                {
                    gParticles[particleIndex].color.r = lerp(gRange.minColor.r, gRange.maxColor.r, GenerateColorR(baseSeed + 3001));
                    gParticles[particleIndex].color.g = lerp(gRange.minColor.g, gRange.maxColor.g, GenerateColorG(baseSeed + 3002));
                    gParticles[particleIndex].color.b = lerp(gRange.minColor.b, gRange.maxColor.b, GenerateColorB(baseSeed + 3003));
                }
                else
                {
                    gParticles[particleIndex].color.r = gEmitter.startColor.r;
                    gParticles[particleIndex].color.g = gEmitter.startColor.g;
                    gParticles[particleIndex].color.b = gEmitter.startColor.b;
                }
                
                gParticles[particleIndex].translate = GenerateSpherePositionCustom(baseSeed + 5000, gEmitter.translate, gEmitter.radius);
                gParticles[particleIndex].color.a = 1.0f;
                gParticles[particleIndex].rotate = float3(0.0f, 0.0f, 0.0f);
                gParticles[particleIndex].velocity = float3(
                RandomRange(baseSeed + 2001, gRange.minVelocity.x, gRange.maxVelocity.x),
                RandomRange(baseSeed + 2002, gRange.minVelocity.y, gRange.maxVelocity.y),
                RandomRange(baseSeed + 2003, gRange.minVelocity.z, gRange.maxVelocity.z));
                gParticles[particleIndex].rotateVelocity = lerp(gRange.minRotateVelocity, gRange.maxRotateVelocity, RandomFloat(baseSeed + 500));
                gParticles[particleIndex].lifeTime = lerp(gRange.minLifeTime, gRange.maxLifeTime, RandomFloat(baseSeed + 4000));
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
