#include "Particles.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
ConstantBuffer<EmitterSphere> gEmitter : register(b5);
ConstantBuffer<PerFrame> gPerFrame : register(b6);

[RootSignature("UAV(u0), CBV(b5) ,CBV(b6)")]

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    RandomGenerator generator;
    generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
    if (gEmitter.emit != 0)
    { // 射出許可が出たので射出
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            // カウント分Particleを射出する
            gParticles[countIndex].scale = float3(1.0f, 1.0f, 1.0f);
            gParticles[countIndex].translate = generator.Generate3d();
            gParticles[countIndex].color.rgb = generator.Generate3d();
            gParticles[countIndex].color.a = 1.0f;
            gParticles[countIndex].rotate = float3(0.0f, 0.0f, 0.0f);
            gParticles[countIndex].velocity = float3(0.0f, 0.0f, 0.0f);
            gParticles[countIndex].lifeTime = 0.0f;
            gParticles[countIndex].currentTime = 0.0f;
        }
    }
}