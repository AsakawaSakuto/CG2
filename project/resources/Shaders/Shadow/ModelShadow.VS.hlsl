#include "ModelShadow.hlsli"

/// <summary>
/// シャドウマッピング付き Vertex Shader
/// </summary>
VSOutput main(VSInput input) {
    VSOutput output;
    
    // ワールド空間座標
    output.worldPos = mul(input.position, world).xyz;
    
    // クリップ空間座標（カメラ視点）
    output.position = mul(float4(output.worldPos, 1.0f), viewProjection);
    
    // 法線をワールド空間に変換（非等倍スケーリング対応）
    output.normal = normalize(mul(input.normal, (float3x3)worldInverseTranspose));
    
    // UV座標
    output.texcoord = input.texcoord;
    
    // ライト空間座標（シャドウマップ参照用）
    output.lightSpacePos = mul(float4(output.worldPos, 1.0f), lightSpaceMatrix);
    
    return output;
}
