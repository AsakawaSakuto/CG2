#include "ShadowMap.hlsli"

/// <summary>
/// シャドウマップ生成用 Vertex Shader
/// ライト空間への変換のみを行う
/// </summary>
ShadowMapVSOutput main(ShadowMapVSInput input) {
    ShadowMapVSOutput output;
    
    // モデル空間 → ワールド空間 → ライト空間
    // row-majorの場合: output = mul(input, model * lightSpaceMatrix)
    // column-majorの場合: output = lightSpaceMatrix * model * input
    
    // DirectX標準（row-major）
    float4 worldPos = mul(input.position, model);
    output.position = mul(worldPos, lightSpaceMatrix);
    
    return output;
}
