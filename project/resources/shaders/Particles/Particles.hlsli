struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

struct Particle
{
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

// Emitter shape types - must match EmitterShapeType enum in C++
#define EMITTER_SHAPE_POINT 0
#define EMITTER_SHAPE_LINE 1
#define EMITTER_SHAPE_SPHERE 2
#define EMITTER_SHAPE_BOX 3
#define EMITTER_SHAPE_PLANE 4
#define EMITTER_SHAPE_RING 5

// EmitterSphere structure - must match EmitterStateGPU in C++
struct EmitterSphere
{
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

    float3 normalVelocity;
    float pad19;
    
    float lifeTime;
    uint lifeTimeRandom;
    float minLifeTime;
    float maxLifeTime;
    
    uint shapeType;
    float3 size;
    
    float3 lineStart;
    float lineLength;
    
    float ringInnerRadius;
    float ringOuterRadius;
    float2 pad21;
    
    float3 planeNormal;
    float pad22;
    
    uint spawnOnEdge; // エッジ上に生成するか (0: 内部/表面全体, 1: エッジのみ)
    uint enableVisualization; // 可視化を有効にするか (0: 無効, 1: 有効)
    uint useGravity;
    uint blendModeValue; // BlendModeをuint32_tとして格納
    
    float gravityY;
    float accelerationY;
    float2 pad23;
};

struct PerView
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};

struct PerFrame
{
    float time;
    float deltaTime;
    uint index;
    float pad1; // 16バイト境界に合わせるためのパディング
};

class RandomGenerator
{
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

// Line emitter - generates position along a line or at endpoints
float3 GenerateLinePosition(uint baseSeed, EmitterSphere emitter)
{
    float3 start = emitter.translate + emitter.lineStart;
    float3 direction = normalize(emitter.size);
    
    if (emitter.spawnOnEdge != 0)
    {
        // Spawn at endpoints only
        float choice = RandomFloat(baseSeed + 10);
        if (choice < 0.5f)
        {
            return start; // Start point
        }
        else
        {
            return start + direction * emitter.lineLength; // End point
        }
    }
    else
    {
        // Spawn along the line
        float t = RandomFloat(baseSeed);
        return start + direction * emitter.lineLength * t;
    }
}

// Sphere emitter - generates position in volume or on surface
float3 GenerateSpherePosition(uint baseSeed, EmitterSphere emitter)
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

    float r;
    if (emitter.spawnOnEdge != 0)
    {
        // Spawn on surface only
        r = emitter.radius;
    }
    else
    {
        // Spawn in volume
        r = emitter.radius * pow(w, 1.0f / 3.0f);
    }

    return emitter.translate + r * dir;
}

// Box emitter - generates position in volume, on surface, or on edges
float3 GenerateBoxPosition(uint baseSeed, EmitterSphere emitter)
{
    if (emitter.spawnOnEdge != 0)
    {
        // Spawn on edges only (12 edges of a box)
        uint edgeIndex = uint(RandomFloat(baseSeed + 10) * 12.0f);
        float t = RandomFloat(baseSeed + 0) - 0.5f; // [-0.5, 0.5]
        
        float halfX = emitter.size.x * 0.5f;
        float halfY = emitter.size.y * 0.5f;
        float halfZ = emitter.size.z * 0.5f;
        
        float3 pos;
        
        // Bottom 4 edges
        if (edgeIndex == 0)
            pos = float3(t * emitter.size.x, -halfY, -halfZ);
        else if (edgeIndex == 1)
            pos = float3(t * emitter.size.x, -halfY, halfZ);
        else if (edgeIndex == 2)
            pos = float3(-halfX, -halfY, t * emitter.size.z);
        else if (edgeIndex == 3)
            pos = float3(halfX, -halfY, t * emitter.size.z);
        // Top 4 edges
        else if (edgeIndex == 4)
            pos = float3(t * emitter.size.x, halfY, -halfZ);
        else if (edgeIndex == 5)
            pos = float3(t * emitter.size.x, halfY, halfZ);
        else if (edgeIndex == 6)
            pos = float3(-halfX, halfY, t * emitter.size.z);
        else if (edgeIndex == 7)
            pos = float3(halfX, halfY, t * emitter.size.z);
        // Vertical 4 edges
        else if (edgeIndex == 8)
            pos = float3(-halfX, t * emitter.size.y, -halfZ);
        else if (edgeIndex == 9)
            pos = float3(halfX, t * emitter.size.y, -halfZ);
        else if (edgeIndex == 10)
            pos = float3(-halfX, t * emitter.size.y, halfZ);
        else
            pos = float3(halfX, t * emitter.size.y, halfZ);
            
        return emitter.translate + pos;
    }
    else
    {
        // Spawn in volume
        float x = (RandomFloat(baseSeed + 0) - 0.5f) * emitter.size.x;
        float y = (RandomFloat(baseSeed + 1) - 0.5f) * emitter.size.y;
        float z = (RandomFloat(baseSeed + 2) - 0.5f) * emitter.size.z;
        
        return emitter.translate + float3(x, y, z);
    }
}

// Plane emitter - generates position on plane or on edges
float3 GeneratePlanePosition(uint baseSeed, EmitterSphere emitter)
{
    // Create orthogonal basis vectors from the normal
    float3 normalizedNormal = normalize(emitter.planeNormal);
    float3 up = float3(0.0f, 1.0f, 0.0f);
    
    // If normal is parallel to up, use right vector instead
    if (abs(dot(normalizedNormal, up)) > 0.999f)
    {
        up = float3(1.0f, 0.0f, 0.0f);
    }
    
    float3 right = normalize(cross(up, normalizedNormal));
    float3 forward = cross(normalizedNormal, right);
    
    float halfWidth = emitter.size.x * 0.5f;
    float halfHeight = emitter.size.y * 0.5f;
    
    if (emitter.spawnOnEdge != 0)
    {
        // Spawn on edges only (4 edges of the plane rectangle)
        uint edgeIndex = uint(RandomFloat(baseSeed + 10) * 4.0f);
        float t = RandomFloat(baseSeed + 0) - 0.5f; // [-0.5, 0.5]
        
        float3 localPos;
        
        if (edgeIndex == 0) // Top edge
            localPos = right * (t * emitter.size.x) + forward * halfHeight;
        else if (edgeIndex == 1) // Bottom edge
            localPos = right * (t * emitter.size.x) + forward * (-halfHeight);
        else if (edgeIndex == 2) // Right edge
            localPos = right * halfWidth + forward * (t * emitter.size.y);
        else // Left edge
            localPos = right * (-halfWidth) + forward * (t * emitter.size.y);
            
        return emitter.translate + localPos;
    }
    else
    {
        // Spawn on plane surface
        float u = RandomFloat(baseSeed + 0) - 0.5f; // [-0.5, 0.5]
        float v = RandomFloat(baseSeed + 1) - 0.5f; // [-0.5, 0.5]
        
        float3 localPos = right * (u * emitter.size.x) + forward * (v * emitter.size.y);
        
        return emitter.translate + localPos;
    }
}

// Ring emitter - generates position in ring area or on circumferences
float3 GenerateRingPosition(uint baseSeed, EmitterSphere emitter)
{
    float angle = RandomFloat(baseSeed + 0) * 2.0f * 3.14159265f;
    
    // Create orthogonal basis vectors from the normal
    float3 normalizedNormal = normalize(emitter.planeNormal);
    float3 up = float3(0.0f, 1.0f, 0.0f);
    
    if (abs(dot(normalizedNormal, up)) > 0.999f)
    {
        up = float3(1.0f, 0.0f, 0.0f);
    }
    
    float3 right = normalize(cross(up, normalizedNormal));
    float3 forward = cross(normalizedNormal, right);
    
    float radius;
    
    if (emitter.spawnOnEdge != 0)
    {
        // Spawn on circumferences only (inner or outer)
        float choice = RandomFloat(baseSeed + 10);
        if (choice < 0.5f)
        {
            radius = emitter.ringInnerRadius;
        }
        else
        {
            radius = emitter.ringOuterRadius;
        }
    }
    else
    {
        // Spawn in ring area
        float radiusLerp = RandomFloat(baseSeed + 1);
        radius = lerp(emitter.ringInnerRadius, emitter.ringOuterRadius, radiusLerp);
    }
    
    // Generate position in ring plane
    float3 localPos = right * (cos(angle) * radius) + forward * (sin(angle) * radius);
    
    return emitter.translate + localPos;
}

// Universal position generator based on emitter type
float3 GenerateEmitterPosition(uint baseSeed, EmitterSphere emitter)
{
    switch (emitter.shapeType)
    {
        case EMITTER_SHAPE_POINT:
            return GeneratePointPosition(baseSeed, emitter.translate);
            
        case EMITTER_SHAPE_LINE:
            return GenerateLinePosition(baseSeed, emitter);
            
        case EMITTER_SHAPE_SPHERE:
            return GenerateSpherePosition(baseSeed, emitter);
            
        case EMITTER_SHAPE_BOX:
            return GenerateBoxPosition(baseSeed, emitter);
            
        case EMITTER_SHAPE_PLANE:
            return GeneratePlanePosition(baseSeed, emitter);
            
        case EMITTER_SHAPE_RING:
            return GenerateRingPosition(baseSeed, emitter);
            
        default:
            return emitter.translate; // Fallback to point
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