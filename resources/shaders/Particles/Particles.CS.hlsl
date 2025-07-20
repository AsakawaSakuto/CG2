#include "Particles.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);

[RootSignature("UAV(u0)")]

// 768以下
[numthreads(kMaxParticles, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) {
   
}