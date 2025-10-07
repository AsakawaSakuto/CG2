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
            if (gEmitter.isMove != 0)
            {
                gParticles[particleIndex].translate += gParticles[particleIndex].velocity * gPerFrame.deltaTime;
                // Z軸の回転のみを更新する場合
                gParticles[particleIndex].rotate.z += gParticles[particleIndex].rotateVelocity * gPerFrame.deltaTime;
            }
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            
            // 透明度フェードのフラグをチェックして適用
            if (gEmitter.enableAlphaFade != 0)
            {
                float alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
                gParticles[particleIndex].color.a = saturate(alpha);
            }
            
            // スケールフェードのフラグをチェックして適用
            if (gEmitter.enableScaleFade != 0)
            {
                // 生存時間の進行度 (0.0 = 開始, 1.0 = 終了)
                float lifeProgress = gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime;
                lifeProgress = saturate(lifeProgress);
                
                // 開始スケールから終了スケールに線形補間
                float currentScaleMultiplier = lerp(gEmitter.startScale, gEmitter.endScale, lifeProgress);
                
                // 元のスケール値に倍率を適用（初期スケールを保持する必要がある場合は別途管理が必要）
                // ここでは現在のスケールに倍率を適用
                float3 baseScale = gParticles[particleIndex].scale;
                // 基準スケールを1として、倍率を適用
                gParticles[particleIndex].scale = baseScale * currentScaleMultiplier;
            }
            // フラグがfalseの場合、スケールは初期値のまま維持
        }
        
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