struct VertexShaderOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

static const uint kMaxParticles = 1;

struct Particle {
    float3 scale;
    float pad1;
    float3 rotate;
    float pad2;
    float3 translate;
    float pad3;
    float3 velocity;
    float pad4;
    float lifeTime;
    float currentTime;
    float2 pad5;
    float4 color;
};

struct EmitterSphere {
    float3 translate;
    float radius;
    uint count;
    float frequency;
    float frequencyTime;
    uint emit;
    float2 pad;
};

struct PreView {
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};

struct PreFrame {
    float time;
    float deltaTime;
};

class RandomGenerator {
    float3 seed;

    float3 rand3dTo3d(float3 s)
    {
        return frac(sin(s) * 43758.5453);
    }

    float rand3dTo1d(float3 s)
    {
        return frac(sin(dot(s, float3(12.9898, 78.233, 45.164))) * 43758.5453);
    }

    float3 Generate3d()
    {
        seed = rand3dTo3d(seed);
        return seed;
    }

    float Generate1d()
    {
        float result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
};
