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
    
    // スレッド0でフリーリストインデックスを初期化
    if (particlesIndex == 0)
    {
        gFreeListIndex[0] = gEmitter.kMaxParticle;
    }

    if (particlesIndex < gEmitter.kMaxParticle)
    {
        // パーティクルを完全にゼロクリア
        gParticles[particlesIndex].scale = float3(0.0f, 0.0f, 0.0f);
        gParticles[particlesIndex].rotate = float3(0.0f, 0.0f, 0.0f);
        gParticles[particlesIndex].translate = float3(0.0f, 0.0f, 0.0f);
        gParticles[particlesIndex].velocity = float3(0.0f, 0.0f, 0.0f);
        gParticles[particlesIndex].lifeTime = 0.0f;
        gParticles[particlesIndex].currentTime = 0.0f;
        gParticles[particlesIndex].color = float4(0.0f, 0.0f, 0.0f, 0.0f);
        gParticles[particlesIndex].rotateVelocity = 0.0f;
        gParticles[particlesIndex].saveScale = float3(0.0f, 0.0f, 0.0f);
        
        // フリーリストの初期化（逆順にして、0から順番にポップされるようにする）
        gFreeList[particlesIndex] = gEmitter.kMaxParticle - 1 - particlesIndex;
    }
}