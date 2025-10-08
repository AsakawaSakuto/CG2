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
        if (gParticles[particleIndex].color.a > 0.0f)
        {
            if (gEmitter.isMove != 0)
            {
                gParticles[particleIndex].translate += gParticles[particleIndex].velocity * gPerFrame.deltaTime;
            }
            
            if (gEmitter.enableRotateMove != 0)
            {
                // rotateVelocityはfloat型なので直接使用
                float rotVel = gParticles[particleIndex].rotateVelocity;
                gParticles[particleIndex].rotate.z += rotVel * gPerFrame.deltaTime;
            }
            
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            
            // 生存時間の進行度 (0.0 = 開始, 1.0 = 終了)
            float lifeProgress = gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime;
            lifeProgress = saturate(lifeProgress);
            
            // 透明度フェードのフラグをチェックして適用
            if (gEmitter.enableAlphaFade != 0)
            {
                float alpha = 1.0f - lifeProgress;
                gParticles[particleIndex].color.a = saturate(alpha);
            }
            
            // スケールフェードのフラグをチェックして適用
            if (gEmitter.enableScaleFade != 0)
            {
                // 開始スケールから終了スケールに線形補間
                float currentScaleMultiplierX = lerp(gEmitter.startScale.x, gEmitter.endScale.x, lifeProgress);
                float currentScaleMultiplierY = lerp(gEmitter.startScale.y, gEmitter.endScale.y, lifeProgress);
                
                // 基準スケールを1として、倍率を適用
                gParticles[particleIndex].scale.x = currentScaleMultiplierX;
                gParticles[particleIndex].scale.y = currentScaleMultiplierY;
                gParticles[particleIndex].scale.z = 0.0f;
            }
            
            // カラーフェードのフラグをチェックして適用
            if (gEmitter.enableColorFade != 0)
            {
                // 開始カラーから終了カラーに線形補間
                float3 currentColor = lerp(gEmitter.startColor, gEmitter.endColor, lifeProgress);
                
                // カラーを適用
                gParticles[particleIndex].color.r = currentColor.r;
                gParticles[particleIndex].color.g = currentColor.g;
                gParticles[particleIndex].color.b = currentColor.b;
            }
            
            // ライフタイムが終了したかチェック
            if (gParticles[particleIndex].currentTime >= gParticles[particleIndex].lifeTime)
            {
                gParticles[particleIndex].color.a = 0.0f;
            }
        }
        
        // アルファ値が0以下になったパーティクルをフリーリストに戻す
        if (gParticles[particleIndex].color.a <= 0.0f)
        {
            gParticles[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);

            int freeListIndex;
            InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);

            if (freeListIndex < gEmitter.kMaxParticle)
            {
                gFreeList[freeListIndex] = particleIndex;
            }
            else
            {
                InterlockedAdd(gFreeListIndex[0], -1);
            }
        }
    }
}