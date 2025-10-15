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
            int originalFreeListIndex;
            InterlockedAdd(gFreeListIndex[0], -1, originalFreeListIndex);

            // フリーリストに利用可能なパーティクルがあるかチェック
            if (originalFreeListIndex > 0 && originalFreeListIndex <= gEmitter.kMaxParticle)
            {
                uint particleIndex = gFreeList[originalFreeListIndex - 1];
                
                // パーティクルインデックスが有効範囲内かチェック
                if (particleIndex < gEmitter.kMaxParticle)
                {
                    uint baseSeed = particleIndex + countIndex * 12345 + gPerFrame.index * 6789;

                    // パーティクルを完全に初期化
                    gParticles[particleIndex].currentTime = 0.0f;
                    gParticles[particleIndex].rotate = float3(0.0f, 0.0f, 0.0f);
                    gParticles[particleIndex].color.a = 1.0f;

                    if (gEmitter.scaleRandom != 0)
                    {
                        gParticles[particleIndex].scale = lerp(gEmitter.minScale, gEmitter.maxScale, RandomFloat(baseSeed + 500));
                        gParticles[particleIndex].saveScale = gParticles[particleIndex].scale;
                    }
                    else
                    {
                        gParticles[particleIndex].scale.x = gEmitter.startScale.x;
                        gParticles[particleIndex].scale.y = gEmitter.startScale.y;
                        gParticles[particleIndex].scale.z = 0.0f;
                        gParticles[particleIndex].saveScale = gParticles[particleIndex].scale;
                    }
                    
                    if (gEmitter.colorRandom != 0)
                    {
                        gParticles[particleIndex].color.r = lerp(gEmitter.minColor.r, gEmitter.maxColor.r, GenerateColorR(baseSeed + 3001));
                        gParticles[particleIndex].color.g = lerp(gEmitter.minColor.g, gEmitter.maxColor.g, GenerateColorG(baseSeed + 3002));
                        gParticles[particleIndex].color.b = lerp(gEmitter.minColor.b, gEmitter.maxColor.b, GenerateColorB(baseSeed + 3003));
                    }
                    else
                    {
                        gParticles[particleIndex].color.r = gEmitter.startColor.r;
                        gParticles[particleIndex].color.g = gEmitter.startColor.g;
                        gParticles[particleIndex].color.b = gEmitter.startColor.b;
                    }
                    
                    // Use the new universal position generator
                    gParticles[particleIndex].translate = GenerateEmitterPosition(baseSeed + 5000, gEmitter);
                    
                    if (gEmitter.velocityRandom != 0)
                    {
                        gParticles[particleIndex].velocity = float3(
                        RandomRange(baseSeed + 2001, gEmitter.minVelocity.x, gEmitter.maxVelocity.x),
                        RandomRange(baseSeed + 2002, gEmitter.minVelocity.y, gEmitter.maxVelocity.y),
                        RandomRange(baseSeed + 2003, gEmitter.minVelocity.z, gEmitter.maxVelocity.z));
                    }
                    else
                    {
                        gParticles[particleIndex].velocity = gParticles[particleIndex].translate - gEmitter.translate;
                        gParticles[particleIndex].velocity = normalize(gParticles[particleIndex].velocity);
                    }
                    
                    if (gEmitter.rotateVelocityRandom != 0)
                    {
                        gParticles[particleIndex].rotateVelocity = lerp(gEmitter.minRotateVelocity, gEmitter.maxRotateVelocity, RandomFloat(baseSeed + 500));
                    }
                    else
                    {
                        float random = lerp(-1.0f, 1.0f, RandomFloat(baseSeed + 600));
                        if (random >= 0.0f)
                        {
                            gParticles[particleIndex].rotateVelocity = gEmitter.startRotateVelocity;
                        }
                        else
                        {
                            gParticles[particleIndex].rotateVelocity = gEmitter.endRotateVelocity;
                        }
                    }
                    
                    if (gEmitter.lifeTimeRandom != 0)
                    {
                        gParticles[particleIndex].lifeTime = lerp(gEmitter.minLifeTime, gEmitter.maxLifeTime, RandomFloat(baseSeed + 4000));
                    }
                    else
                    {
                        gParticles[particleIndex].lifeTime = gEmitter.lifeTime;
                    }
                }
                else
                {
                    // 無効なパーティクルインデックスの場合、フリーリストインデックスを戻す
                    InterlockedAdd(gFreeListIndex[0], 1);
                    break;
                }
            }
            else
            {
                // フリーリストが空の場合、カウンターを戻して終了
                InterlockedAdd(gFreeListIndex[0], 1);
                break;
            }
        }
    }

}
