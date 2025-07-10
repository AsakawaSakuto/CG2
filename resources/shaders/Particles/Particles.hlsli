struct VertexShaderOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

struct Particle {
    float3 scale;
    float3 rotate;
    float3 translate;
    float3 velocity;
    float lifeTime;
    float currentTime;
    float4 color;
};