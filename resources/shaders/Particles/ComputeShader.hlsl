#include "Particles.hlsli"

static const uint kMaxParticles = 512;
RWStructuredBuffer<Particle> gParticles : register(u0);

[numthreads(512, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) {
    uint particlesIndex = DTid.x;
    if (particlesIndex < kMaxParticles) {
        gParticles[particlesIndex] = (Particle) 0;
    }
}