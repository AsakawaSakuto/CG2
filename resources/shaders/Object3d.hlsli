struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
};

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

struct Material
{
    float4 color;
    int enableLighting;
    float3 padding1;
    float4x4 uvTransform;
    float shininess;
    float3 padding2;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
    float3 padding;
};

struct Camera
{
    float3 worldPosition;
    float padding;
};

struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
};

struct SpotLight
{
    float4 color; 
    float3 position;
    float intensity;
    float3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFalloffStart;
};