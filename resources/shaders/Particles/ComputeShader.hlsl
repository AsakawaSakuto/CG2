#include "Particles.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}