#include "Object3d.hlsli"

// 定数バッファ（b1）: ワールド行列とWVP行列
struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);

// 入力頂点構造体
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    // 同次座標変換
    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;

    // 法線ベクトルをワールド空間に変換
    output.normal = normalize(mul(input.normal, (float3x3)gTransformationMatrix.WorldInverseTranspose));

    // ワールド座標変換
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;

    return output;
}
