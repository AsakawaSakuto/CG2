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
#define EMITTER_SHAPE_BOX_VOLUME 4
#define EMITTER_SHAPE_BOX_SURFACE 5
#define EMITTER_SHAPE_RING_XZ 6
#define EMITTER_SHAPE_RING_XY 7
#define EMITTER_SHAPE_RING_YZ 8
#define EMITTER_SHAPE_CONE_VOLUME 9
#define EMITTER_SHAPE_CONE_SURFACE 10
#define EMITTER_SHAPE_HEMISPHERE_VOLUME 11
#define EMITTER_SHAPE_HEMISPHERE_SURFACE 12
#define EMITTER_SHAPE_PLANE_ANGLE 13
#define EMITTER_SHAPE_PLANE_ANGLE_EDGE 14
#define EMITTER_SHAPE_RING_ANGLE 15
#define EMITTER_SHAPE_RING_ANGLE_EDGE 16

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

    float3 normalVelocity;
    float pad19;
    
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
    
    // New fields for cone and hemisphere emitters
    float coneAngle;        // Cone angle in degrees (0-180)
    float coneHeight;       // Cone height
    float3 coneDirection;   // Cone direction vector
    float hemisphereAngle;  // Hemisphere angle in degrees (0-180)
    
    // New fields for angle-based plane and ring emitters
    float3 planeNormal;     // Plane normal vector (for PLANE_ANGLE types)
    float planeWidth;       // Plane width
    float planeHeight;      // Plane height
    float ringAngle;        // Ring rotation angle around normal axis
    float3 ringNormal;      // Ring normal vector (for RING_ANGLE types)
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

// Box surface emitter - generates position only on box surface
float3 GenerateBoxSurfacePosition(uint baseSeed, float3 center, float3 size)
{
    // Choose which face to spawn on (6 faces)
    uint faceIndex = uint(RandomFloat(baseSeed + 3) * 6.0f);
    
    float x, y, z;
    
    switch (faceIndex)
    {
        case 0: // Front face (+Z)
            x = (RandomFloat(baseSeed + 0) - 0.5f) * size.x;
            y = (RandomFloat(baseSeed + 1) - 0.5f) * size.y;
            z = size.z * 0.5f;
            break;
        case 1: // Back face (-Z)
            x = (RandomFloat(baseSeed + 0) - 0.5f) * size.x;
            y = (RandomFloat(baseSeed + 1) - 0.5f) * size.y;
            z = -size.z * 0.5f;
            break;
        case 2: // Right face (+X)
            x = size.x * 0.5f;
            y = (RandomFloat(baseSeed + 1) - 0.5f) * size.y;
            z = (RandomFloat(baseSeed + 2) - 0.5f) * size.z;
            break;
        case 3: // Left face (-X)
            x = -size.x * 0.5f;
            y = (RandomFloat(baseSeed + 1) - 0.5f) * size.y;
            z = (RandomFloat(baseSeed + 2) - 0.5f) * size.z;
            break;
        case 4: // Top face (+Y)
            x = (RandomFloat(baseSeed + 0) - 0.5f) * size.x;
            y = size.y * 0.5f;
            z = (RandomFloat(baseSeed + 2) - 0.5f) * size.z;
            break;
        default: // Bottom face (-Y)
            x = (RandomFloat(baseSeed + 0) - 0.5f) * size.x;
            y = -size.y * 0.5f;
            z = (RandomFloat(baseSeed + 2) - 0.5f) * size.z;
            break;
    }
    
    return center + float3(x, y, z);
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

// RING_XY emitter - generates position on a ring in the XY plane
float3 GenerateRingXYPosition(uint baseSeed, float3 center, float innerRadius, float outerRadius)
{
    float angle = RandomFloat(baseSeed + 0) * 2.0f * 3.14159265f;
    float radiusLerp = RandomFloat(baseSeed + 1);
    float radius = lerp(innerRadius, outerRadius, radiusLerp);

    // Generate position in XY plane (Z = 0)
    float3 localPos = float3(cos(angle) * radius, sin(angle) * radius, 0.0f);

    return center + localPos;
}

// RING_YZ emitter - generates position on a ring in the YZ plane
float3 GenerateRingYZPosition(uint baseSeed, float3 center, float innerRadius, float outerRadius)
{
    float angle = RandomFloat(baseSeed + 0) * 2.0f * 3.14159265f;
    float radiusLerp = RandomFloat(baseSeed + 1);
    float radius = lerp(innerRadius, outerRadius, radiusLerp);

    // Generate position in YZ plane (X = 0)
    float3 localPos = float3(0.0f, cos(angle) * radius, sin(angle) * radius);

    return center + localPos;
}

// Cone emitter - generates position within a cone volume
float3 GenerateConePosition(uint baseSeed, float3 center, float3 direction, float angle, float height)
{
    // Convert angle from degrees to radians
    float angleRad = angle * 3.14159265f / 180.0f;
    float maxRadius = tan(angleRad) * height;
    
    // Random height along the cone
    float t = RandomFloat(baseSeed + 0);
    float currentHeight = height * t;
    
    // Random radius at this height (cone gets wider as height increases)
    float currentMaxRadius = maxRadius * t;
    float radiusRandom = RandomFloat(baseSeed + 1);
    float currentRadius = currentMaxRadius * sqrt(radiusRandom); // sqrt for uniform distribution
    
    // Random angle around the cone axis
    float theta = RandomFloat(baseSeed + 2) * 2.0f * 3.14159265f;
    
    // Generate local position in cone space (assuming direction is +Y)
    float3 localPos = float3(
        cos(theta) * currentRadius,
        currentHeight,
        sin(theta) * currentRadius
    );
    
    // Create rotation matrix to align with cone direction
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 normalizedDir = normalize(direction);
    
    // If direction is not up, rotate the local position
    if (abs(dot(normalizedDir, up)) < 0.999f)
    {
        float3 right = normalize(cross(up, normalizedDir));
        float3 forward = cross(normalizedDir, right);
        localPos = localPos.x * right + localPos.y * normalizedDir + localPos.z * forward;
    }
    else if (dot(normalizedDir, up) < 0.0f)
    {
        // Direction is down, flip Y
        localPos.y = -localPos.y;
    }
    
    return center + localPos;
}

// Cone surface emitter - generates position only on cone surface
float3 GenerateConeSurfacePosition(uint baseSeed, float3 center, float3 direction, float angle, float height)
{
    // Convert angle from degrees to radians
    float angleRad = angle * 3.14159265f / 180.0f;
    float maxRadius = tan(angleRad) * height;
    
    // Choose between cone surface or base
    float surfaceChoice = RandomFloat(baseSeed + 3);
    float3 localPos;
    
    if (surfaceChoice < 0.8f) // 80% chance for cone surface
    {
        // Random height along the cone
        float t = RandomFloat(baseSeed + 0);
        float currentHeight = height * t;
        
        // Radius at this height
        float currentRadius = maxRadius * t;
        
        // Random angle around the cone axis
        float theta = RandomFloat(baseSeed + 2) * 2.0f * 3.14159265f;
        
        localPos = float3(
            cos(theta) * currentRadius,
            currentHeight,
            sin(theta) * currentRadius
        );
    }
    else // 20% chance for base
    {
        // Random position on the base circle
        float theta = RandomFloat(baseSeed + 2) * 2.0f * 3.14159265f;
        float radiusRandom = RandomFloat(baseSeed + 1);
        float baseRadius = maxRadius * sqrt(radiusRandom);
        
        localPos = float3(
            cos(theta) * baseRadius,
            height,
            sin(theta) * baseRadius
        );
    }
    
    // Create rotation matrix to align with cone direction
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 normalizedDir = normalize(direction);
    
    // If direction is not up, rotate the local position
    if (abs(dot(normalizedDir, up)) < 0.999f)
    {
        float3 right = normalize(cross(up, normalizedDir));
        float3 forward = cross(normalizedDir, right);
        localPos = localPos.x * right + localPos.y * normalizedDir + localPos.z * forward;
    }
    else if (dot(normalizedDir, up) < 0.0f)
    {
        // Direction is down, flip Y
        localPos.y = -localPos.y;
    }
    
    return center + localPos;
}

// Hemisphere emitter - generates position within a hemisphere volume
float3 GenerateHemispherePosition(uint baseSeed, float3 center, float3 direction, float radius, float angle)
{
    // Convert angle from degrees to radians
    float angleRad = angle * 3.14159265f / 180.0f;
    float maxCosTheta = cos(angleRad);
    
    // Generate random spherical coordinates within the hemisphere
    float u = RandomFloat(baseSeed + 0);
    float v = RandomFloat(baseSeed + 1);
    float w = RandomFloat(baseSeed + 2);
    
    // Constrain theta to hemisphere angle
    float cosTheta = lerp(maxCosTheta, 1.0f, u);
    float sinTheta = sqrt(max(0.0f, 1.0f - cosTheta * cosTheta));
    float phi = 2.0f * 3.14159265f * v;
    
    // Random radius within hemisphere
    float r = radius * pow(w, 1.0f / 3.0f);
    
    // Generate local position in hemisphere space (assuming direction is +Y)
    float3 localPos = float3(
        r * sinTheta * cos(phi),
        r * cosTheta,
        r * sinTheta * sin(phi)
    );
    
    // Rotate to align with hemisphere direction
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 normalizedDir = normalize(direction);
    
    if (abs(dot(normalizedDir, up)) < 0.999f)
    {
        float3 right = normalize(cross(up, normalizedDir));
        float3 forward = cross(normalizedDir, right);
        localPos = localPos.x * right + localPos.y * normalizedDir + localPos.z * forward;
    }
    else if (dot(normalizedDir, up) < 0.0f)
    {
        // Direction is down, flip Y
        localPos.y = -localPos.y;
    }
    
    return center + localPos;
}

// Hemisphere surface emitter - generates position only on hemisphere surface
float3 GenerateHemisphereSurfacePosition(uint baseSeed, float3 center, float3 direction, float radius, float angle)
{
    // Convert angle from degrees to radians
    float angleRad = angle * 3.14159265f / 180.0f;
    float maxCosTheta = cos(angleRad);
    
    // Generate random spherical coordinates on the hemisphere surface
    float u = RandomFloat(baseSeed + 0);
    float v = RandomFloat(baseSeed + 1);
    
    // Constrain theta to hemisphere angle
    float cosTheta = lerp(maxCosTheta, 1.0f, u);
    float sinTheta = sqrt(max(0.0f, 1.0f - cosTheta * cosTheta));
    float phi = 2.0f * 3.14159265f * v;
    
    // Generate local position on hemisphere surface (assuming direction is +Y)
    float3 localPos = float3(
        radius * sinTheta * cos(phi),
        radius * cosTheta,
        radius * sinTheta * sin(phi)
    );
    
    // Rotate to align with hemisphere direction
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 normalizedDir = normalize(direction);
    
    if (abs(dot(normalizedDir, up)) < 0.999f)
    {
        float3 right = normalize(cross(up, normalizedDir));
        float3 forward = cross(normalizedDir, right);
        localPos = localPos.x * right + localPos.y * normalizedDir + localPos.z * forward;
    }
    else if (dot(normalizedDir, up) < 0.0f)
    {
        // Direction is down, flip Y
        localPos.y = -localPos.y;
    }
    
    return center + localPos;
}

// Plane angle emitter - generates position on an oriented plane surface
float3 GeneratePlaneAnglePosition(uint baseSeed, float3 center, float3 normal, float width, float height)
{
    float u = RandomFloat(baseSeed + 0) - 0.5f; // [-0.5, 0.5]
    float v = RandomFloat(baseSeed + 1) - 0.5f; // [-0.5, 0.5]
    
    // Create orthogonal basis vectors from the normal
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 normalizedNormal = normalize(normal);
    
    // If normal is parallel to up, use right vector instead
    if (abs(dot(normalizedNormal, up)) > 0.999f)
    {
        up = float3(1.0f, 0.0f, 0.0f);
    }
    
    float3 right = normalize(cross(up, normalizedNormal));
    float3 forward = cross(normalizedNormal, right);
    
    // Generate position on plane
    float3 localPos = right * (u * width) + forward * (v * height);
    
    return center + localPos;
}

// Plane angle edge emitter - generates position only on plane edges
float3 GeneratePlaneAngleEdgePosition(uint baseSeed, float3 center, float3 normal, float width, float height)
{
    // Choose which edge to spawn on (4 edges)
    uint edgeIndex = uint(RandomFloat(baseSeed + 2) * 4.0f);
    
    // Create orthogonal basis vectors from the normal
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 normalizedNormal = normalize(normal);
    
    if (abs(dot(normalizedNormal, up)) > 0.999f)
    {
        up = float3(1.0f, 0.0f, 0.0f);
    }
    
    float3 right = normalize(cross(up, normalizedNormal));
    float3 forward = cross(normalizedNormal, right);
    
    float u = RandomFloat(baseSeed + 0) - 0.5f; // [-0.5, 0.5]
    float3 localPos;
    
    switch (edgeIndex)
    {
        case 0: // Top edge
            localPos = right * (u * width) + forward * (height * 0.5f);
            break;
        case 1: // Bottom edge
            localPos = right * (u * width) + forward * (-height * 0.5f);
            break;
        case 2: // Right edge
            localPos = right * (width * 0.5f) + forward * (u * height);
            break;
        default: // Left edge
            localPos = right * (-width * 0.5f) + forward * (u * height);
            break;
    }
    
    return center + localPos;
}

// Ring angle emitter - generates position in a rotatable ring area
float3 GenerateRingAnglePosition(uint baseSeed, float3 center, float3 normal, float innerRadius, float outerRadius)
{
    float angle = RandomFloat(baseSeed + 0) * 2.0f * 3.14159265f;
    float radiusLerp = RandomFloat(baseSeed + 1);
    float radius = lerp(innerRadius, outerRadius, radiusLerp);
    
    // Create orthogonal basis vectors from the normal
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 normalizedNormal = normalize(normal);
    
    if (abs(dot(normalizedNormal, up)) > 0.999f)
    {
        up = float3(1.0f, 0.0f, 0.0f);
    }
    
    float3 right = normalize(cross(up, normalizedNormal));
    float3 forward = cross(normalizedNormal, right);
    
    // Generate position in ring plane
    float3 localPos = right * (cos(angle) * radius) + forward * (sin(angle) * radius);
    
    return center + localPos;
}

// Ring angle edge emitter - generates position only on ring circumference
float3 GenerateRingAngleEdgePosition(uint baseSeed, float3 center, float3 normal, float innerRadius, float outerRadius)
{
    float angle = RandomFloat(baseSeed + 0) * 2.0f * 3.14159265f;
    
    // Choose between inner or outer circumference
    float edgeChoice = RandomFloat(baseSeed + 2);
    float radius = (edgeChoice < 0.5f) ? innerRadius : outerRadius;
    
    // Create orthogonal basis vectors from the normal
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 normalizedNormal = normalize(normal);
    
    if (abs(dot(normalizedNormal, up)) > 0.999f)
    {
        up = float3(1.0f, 0.0f, 0.0f);
    }
    
    float3 right = normalize(cross(up, normalizedNormal));
    float3 forward = cross(normalizedNormal, right);
    
    // Generate position on ring circumference
    float3 localPos = right * (cos(angle) * radius) + forward * (sin(angle) * radius);
    
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
            
        case EMITTER_SHAPE_BOX_VOLUME:
            return GenerateBoxPosition(baseSeed, emitter.translate, emitter.size);
            
        case EMITTER_SHAPE_RING_XZ:
            return GenerateRingPosition(baseSeed, emitter.translate, emitter.ringInnerRadius, emitter.ringOuterRadius, float3(0.0f, 1.0f, 0.0f));
            
        case EMITTER_SHAPE_BOX_SURFACE:
            return GenerateBoxSurfacePosition(baseSeed, emitter.translate, emitter.size);
            
        case EMITTER_SHAPE_RING_XY:
            return GenerateRingXYPosition(baseSeed, emitter.translate, emitter.ringInnerRadius, emitter.ringOuterRadius);
            
        case EMITTER_SHAPE_RING_YZ:
            return GenerateRingYZPosition(baseSeed, emitter.translate, emitter.ringInnerRadius, emitter.ringOuterRadius);
            
        case EMITTER_SHAPE_CONE_VOLUME:
            return GenerateConePosition(baseSeed, emitter.translate, emitter.coneDirection, emitter.coneAngle, emitter.coneHeight);
            
        case EMITTER_SHAPE_CONE_SURFACE:
            return GenerateConeSurfacePosition(baseSeed, emitter.translate, emitter.coneDirection, emitter.coneAngle, emitter.coneHeight);
            
        case EMITTER_SHAPE_HEMISPHERE_VOLUME:
            return GenerateHemispherePosition(baseSeed, emitter.translate, emitter.coneDirection, emitter.radius, emitter.hemisphereAngle);
            
        case EMITTER_SHAPE_HEMISPHERE_SURFACE:
            return GenerateHemisphereSurfacePosition(baseSeed, emitter.translate, emitter.coneDirection, emitter.radius, emitter.hemisphereAngle);
            
        case EMITTER_SHAPE_PLANE_ANGLE:
            return GeneratePlaneAnglePosition(baseSeed, emitter.translate, emitter.planeNormal, emitter.planeWidth, emitter.planeHeight);
            
        case EMITTER_SHAPE_PLANE_ANGLE_EDGE:
            return GeneratePlaneAngleEdgePosition(baseSeed, emitter.translate, emitter.planeNormal, emitter.planeWidth, emitter.planeHeight);
            
        case EMITTER_SHAPE_RING_ANGLE:
            return GenerateRingAnglePosition(baseSeed, emitter.translate, emitter.ringNormal, emitter.ringInnerRadius, emitter.ringOuterRadius);
            
        case EMITTER_SHAPE_RING_ANGLE_EDGE:
            return GenerateRingAngleEdgePosition(baseSeed, emitter.translate, emitter.ringNormal, emitter.ringInnerRadius, emitter.ringOuterRadius);
            
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