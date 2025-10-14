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
    
    float3 saveScale;
    float pad7;
};

// Emitter shape types
#define EMITTER_SHAPE_POINT 0
#define EMITTER_SHAPE_LINE 1
#define EMITTER_SHAPE_SPHERE_VOLUME 2
#define EMITTER_SHAPE_SPHERE_SURFACE 3
#define EMITTER_SHAPE_BOX 4
#define EMITTER_SHAPE_RING 5

struct EmitterSphere {
    float3 translate;
    float radius;
	
    uint useEmitter;
    uint emit;
    uint count;
    uint kMaxParticle;

    float frequency;
    float frequencyTime;
    float2 pad2;

    float2 startScale;
    float2 endScale;

    uint scaleFade;
    uint scaleRandom;
    float2 pad3;

    float3 minScale;
    float pad4;
    float3 maxScale;
    float pad5;

    uint rotateMove;
    float startRotateVelocity;
    float endRotateVelocity;
    uint rotateVelocityRandom;
    
    float minRotateVelocity;
    float maxRotateVelocity;
    float2 pad6;

    uint alphaFade;
    uint colorFade;
    float2 pad7;

    float3 startColor;
    float pad8;
    float3 endColor;
    float pad9;

    uint colorRandom;
    float3 pad10;

    float3 minColor;
    float pad11;
    float3 maxColor;
    float pad12;

    uint isMove;
    float3 pad13;
    
    float3 startVelocity;
    float pad14;

    float3 endVelocity;
    float pad15;

    uint velocityRandom;
    float3 pad16;

    float3 minVelocity;
    float pad17;
    float3 maxVelocity;
    float pad18;

    float lifeTime;
    uint lifeTimeRandom;
    float minLifeTime;
    float maxLifeTime;
    
    // New fields for multi-shape support
    uint shapeType;  // Emitter shape type
    float3 size;     // For box: width, height, depth; For line: direction vector; For ring: inner radius, outer radius, 0
    
    float3 lineStart;  // For line emitter: start point
    float lineLength;  // For line emitter: length
    
    float ringInnerRadius;  // For ring emitter
    float ringOuterRadius;  // For ring emitter
    float2 padNew;
};

struct PerView {
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};

struct PerFrame {
    float time;
    float deltaTime;
    uint index;
    float pad1;  // 16バイト境界に合わせるためのパディング
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

// Point emitter - returns the exact center position
float3 GeneratePointPosition(uint baseSeed, float3 center)
{
    return center;
}

// Line emitter - generates position along a line
float3 GenerateLinePosition(uint baseSeed, float3 start, float3 direction, float length)
{
    float t = RandomFloat(baseSeed);
    return start + normalize(direction) * length * t;
}

// Sphere volume emitter (original sphere function)
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

// Sphere volume emitter with custom center and radius
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

// Sphere surface emitter - generates position on sphere surface only
float3 GenerateSphereSurfacePosition(uint baseSeed, float3 center, float radius)
{
    float u = RandomFloat(baseSeed + 0);
    float v = RandomFloat(baseSeed + 1);

    float theta = 2.0f * 3.14159265f * u;
    float cosPhi = 1.0f - 2.0f * v;
    float sinPhi = sqrt(max(0.0f, 1.0f - cosPhi * cosPhi));

    float3 dir = float3(sinPhi * cos(theta),
                        cosPhi,
                        sinPhi * sin(theta));

    return center + radius * dir;
}

// Box emitter - generates position within a box
float3 GenerateBoxPosition(uint baseSeed, float3 center, float3 size)
{
    float x = RandomFloat(baseSeed + 0) - 0.5f;
    float y = RandomFloat(baseSeed + 1) - 0.5f;
    float z = RandomFloat(baseSeed + 2) - 0.5f;

    return center + float3(x * size.x, y * size.y, z * size.z);
}

// Ring emitter - generates position on a ring (circle)
float3 GenerateRingPosition(uint baseSeed, float3 center, float innerRadius, float outerRadius, float3 normal)
{
    float angle = RandomFloat(baseSeed + 0) * 2.0f * 3.14159265f;
    float radiusLerp = RandomFloat(baseSeed + 1);
    float radius = lerp(innerRadius, outerRadius, radiusLerp);

    // Generate position in XZ plane first
    float3 localPos = float3(cos(angle) * radius, 0.0f, sin(angle) * radius);
    
    // If normal is not (0,1,0), we need to rotate the ring
    float3 up = float3(0.0f, 1.0f, 0.0f);
    if (abs(dot(normal, up)) < 0.999f)
    {
        float3 right = normalize(cross(up, normal));
        float3 forward = cross(normal, right);
        localPos = localPos.x * right + localPos.y * normal + localPos.z * forward;
    }

    return center + localPos;
}

// Universal position generator based on emitter type
float3 GenerateEmitterPosition(uint baseSeed, EmitterSphere emitter)
{
    switch (emitter.shapeType)
    {
        case EMITTER_SHAPE_POINT:
            return GeneratePointPosition(baseSeed, emitter.translate);
            
        case EMITTER_SHAPE_LINE:
            return GenerateLinePosition(baseSeed, emitter.lineStart, emitter.size, emitter.lineLength);
            
        case EMITTER_SHAPE_SPHERE_VOLUME:
            return GenerateSpherePositionCustom(baseSeed, emitter.translate, emitter.radius);
            
        case EMITTER_SHAPE_SPHERE_SURFACE:
            return GenerateSphereSurfacePosition(baseSeed, emitter.translate, emitter.radius);
            
        case EMITTER_SHAPE_BOX:
            return GenerateBoxPosition(baseSeed, emitter.translate, emitter.size);
            
        case EMITTER_SHAPE_RING:
            return GenerateRingPosition(baseSeed, emitter.translate, emitter.ringInnerRadius, emitter.ringOuterRadius, float3(0.0f, 1.0f, 0.0f));
            
        default:
            return GenerateSpherePositionCustom(baseSeed, emitter.translate, emitter.radius);
    }
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