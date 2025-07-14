#include "Particles.hlsli"

// StructuredBufferをt1でバインド（t0はPSのテクスチャ用なので競合回避！）
StructuredBuffer<Particle> gParticles : register(t1);

// カメラやビルボード行列用CB（必要に応じて）
ConstantBuffer<PreView> gPreView : register(b1);

// 頂点シェーダー出力
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

// main
VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    Particle particle = gParticles[instanceId];
    
    // ビルボード行列を使い、パーティクル個別のスケール・平行移動を反映
    float4x4 worldMatrix = gPreView.billboardMatrix;
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;

    output.position = mul(mul(input.position, worldMatrix), gPreView.viewProjection);
    output.texcoord = input.texcoord;
    output.color = particle.color;
    return output;
}
