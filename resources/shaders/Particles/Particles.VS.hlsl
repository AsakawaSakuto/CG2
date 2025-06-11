#include "Particles.hlsli"

// VertexShader
// 与えられた座標を同次クリップ空間に変換する

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 World;
};

StructuredBuffer<TransformationMatrix> gTransformationMatrices : register(t1);

struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input,uint instanceId : SV_InstanceId) {
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrices[instanceId].WVP);
    output.texcoord = input.texcord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrices[instanceId].World));
    return output;
}