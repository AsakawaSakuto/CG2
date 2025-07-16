#include "Particles.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
ConstantBuffer<EmitterSphere> gEmitter : register(b5);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (gEmitter.emit != 0)
    { // 射出許可が出たので射出
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            // カウント分Particleを射出する
            gParticles[countIndex].scale = float3(0.3f, 0.3f, 0.3f);
            gParticles[countIndex].translate = float3(0.0f, 0.0f, 0.0f);
            gParticles[countIndex].color = float4(0.0f, 1.0f, 0.0f, 1.0f);
        }
    }
}