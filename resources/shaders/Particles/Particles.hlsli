struct VertexShaderOutput {
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
};

struct PreView
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};