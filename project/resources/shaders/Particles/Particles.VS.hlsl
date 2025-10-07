#include "Particles.hlsli"

// 頂点シェーダー出力
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

// StructuredBufferをt1でバインド（t0はPSのテクスチャ用なので競合回避！）
StructuredBuffer<Particle> gParticles : register(t1);

// カメラやビルボード行列用CB（必要に応じて）
ConstantBuffer<PerView> gPerView : register(b1);

// 回転行列を作成する関数（Z軸回転）
float3x3 CreateRotationZ(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return float3x3(
        c, -s, 0,
        s,  c, 0,
        0,  0, 1
    );
}

// 回転行列を作成する関数（3軸回転）
float3x3 CreateRotationMatrix(float3 rotation)
{
    float cx = cos(rotation.x);
    float sx = sin(rotation.x);
    float cy = cos(rotation.y);
    float sy = sin(rotation.y);
    float cz = cos(rotation.z);
    float sz = sin(rotation.z);
    
    // X軸回転行列
    float3x3 rotX = float3x3(
        1, 0, 0,
        0, cx, -sx,
        0, sx, cx
    );
    
    // Y軸回転行列
    float3x3 rotY = float3x3(
        cy, 0, sy,
        0, 1, 0,
        -sy, 0, cy
    );
    
    // Z軸回転行列
    float3x3 rotZ = float3x3(
        cz, -sz, 0,
        sz, cz, 0,
        0, 0, 1
    );
    
    // Z * Y * X の順で回転を適用
    return mul(mul(rotZ, rotY), rotX);
}

// main
VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    Particle particle = gParticles[instanceId];
    
    // パーティクルの回転行列を作成
    float3x3 particleRotation = CreateRotationMatrix(particle.rotate);
    
    // 頂点位置に回転を適用
    float3 rotatedPosition = mul(input.position.xyz, particleRotation);
    
    // ビルボード行列を使い、パーティクル個別のスケール・平行移動を反映
    float4x4 worldMatrix = gPerView.billboardMatrix;
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;

    // 回転済みの頂点位置を使用
    float4 rotatedVertex = float4(rotatedPosition, input.position.w);
    output.position = mul(rotatedVertex, mul(worldMatrix, gPerView.viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;
 
    return output;
}
