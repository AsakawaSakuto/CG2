struct VertexShaderOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

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
    
    float rotateVelocity;
    float3 pad6;
};

struct EmitterSphere {
    float3 translate;
    float radius;
    
    uint count;
    float frequency;
    float frequencyTime;
    uint emit;
    
    uint kMaxParticle;
    uint isMove;
    uint enableAlphaFade; // 透明度フェードフラグ
    uint enableScaleFade; // スケールフェードフラグ
    
    float2 startScale;     // 開始時のスケール倍率
    float2 endScale;       // 終了時のスケール倍率
    
    uint enableColorFade; // スケールフェードフラグ
    uint enableRotateMove;
    float2 pad1;
    
    float3 startColor;
    float pad2;
    float3 endColor;
    float pad3;
};

struct EmitterRange {
    float3 minScale;
    float pad1;
    float3 maxScale;
    float pad2;
    
    float3 minTranslate;
    float pad3;
    float3 maxTranslate;
    float pad4;
    
    float3 minColor;
    float pad5;
    float3 maxColor;
    float pad6;
    
    float3 minVelocity;
    float pad7;
    float3 maxVelocity;
    float pad8;
    
    float minLifeTime;
    float maxLifeTime;
    float2 pad9;
    
    float minRotateVelocity;
    float maxRotateVelocity;
    float2 pad10;
};

struct PerView {
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};

struct PerFrame {
    float time;
    float deltaTime;
    uint index;
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

uint Hash(uint x)
{
    x ^= x >> 17;
    x *= 0xed5ad4bbU;
    x ^= x >> 11;
    x *= 0xac4c1b51U;
    x ^= x >> 15;
    x *= 0x31848babU;
    x ^= x >> 14;
    return x;
}

float RandomFloat(uint seed)
{
    return frac(Hash(seed) / 65536.0f);
}

float3 GenerateSpherePosition(uint baseSeed)
{
    float u = RandomFloat(baseSeed + 0);
    float v = RandomFloat(baseSeed + 1);
    float w = RandomFloat(baseSeed + 2);

    float theta = 2.0f * 3.14159265f * u;
    float phi = acos(2.0f * v - 1.0f);
    float r = pow(w, 1.0f / 3.0f);

    float sinPhi = sin(phi);

    return float3(
        r * sinPhi * cos(theta),
        r * sinPhi * sin(theta),
        r * cos(phi)
    );
}

float3 GenerateSpherePositionCustom(uint baseSeed, float3 center, float radius)
{
    float u = RandomFloat(baseSeed + 0);
    float v = RandomFloat(baseSeed + 1);
    float w = RandomFloat(baseSeed + 2);

    float theta = 2.0f * 3.14159265f * u;
    float cosPhi = 1.0f - 2.0f * v;
    float sinPhi = sqrt(max(0.0f, 1.0f - cosPhi * cosPhi));

    float3 dir = float3(sinPhi * cos(theta),
                        cosPhi,
                        sinPhi * sin(theta));

    float r = radius * pow(w, 1.0f / 3.0f);

    return center + r * dir;
}

float RandomRange(uint seed, float minV, float maxV)
{
    return lerp(minV, maxV, RandomFloat(seed));
}

float GenerateColorR(uint baseSeed)
{
    return float(
        RandomFloat(baseSeed + 10)
    );
}

float GenerateColorG(uint baseSeed)
{
    return float(
        RandomFloat(baseSeed + 11)
    );
}

float GenerateColorB(uint baseSeed)
{
    return float(
        RandomFloat(baseSeed + 12)
    );
}