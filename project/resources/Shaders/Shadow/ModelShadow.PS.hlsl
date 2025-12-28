#include "ModelShadow.hlsli"

/// <summary>
/// PCF（3x3）を用いたシャドウ計算
/// </summary>
/// <param name="lightSpacePos">ライト空間での座標</param>
/// <param name="normal">ワールド空間での法線</param>
/// <returns>シャドウ値（0.0 = 完全に影、1.0 = 完全に光）</returns>
float CalculateShadowPCF(float4 lightSpacePos, float3 normal) {
    // 透視除算（Perspective Divide）
    // w で割って NDC（Normalized Device Coordinates）[-1, 1] に変換
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    
    // NDC [-1, 1] を UV座標 [0, 1] に変換
    // DirectX: UV座標系は左上が(0,0)、右下が(1,1)
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;  // Y軸反転
    // Z座標も [0, 1] に変換（DirectXではすでに [0, 1] の場合が多いが念のため）
    // projCoords.z = projCoords.z; // すでに [0, 1]
    
    // シャドウマップ範囲外チェック
    if (projCoords.x < 0.0f || projCoords.x > 1.0f ||
        projCoords.y < 0.0f || projCoords.y > 1.0f ||
        projCoords.z > 1.0f) {
        // 範囲外は影にしない
        return 1.0f;
    }
    
    // 現在のフラグメントの深度値
    float currentDepth = projCoords.z;
    
    // Shadow Acne 対策: Slope-Scaled Depth Bias
    // 法線とライト方向の角度に応じてバイアスを調整
    float3 lightDir = normalize(-lightDirection);
    float cosTheta = saturate(dot(normal, lightDir));
    // cosTheta が小さい（角度が大きい）ほどバイアスを大きくする
    float bias = shadowBias + shadowBiasSlope * (1.0f - cosTheta);
    
    // PCF（Percentage Closer Filtering）: 3x3サンプリング
    float shadow = 0.0f;
    float2 texelSize;
    shadowMap.GetDimensions(texelSize.x, texelSize.y);
    texelSize = 1.0f / texelSize;
    
    [unroll]
    for (int x = -1; x <= 1; ++x) {
        [unroll]
        for (int y = -1; y <= 1; ++y) {
            float2 offset = float2(x, y) * texelSize;
            float2 sampleUV = projCoords.xy + offset;
            
            // シャドウマップから深度値を取得
            float closestDepth = shadowMap.Sample(diffuseSampler, sampleUV).r;
            
            // 深度比較
            // currentDepth > closestDepth + bias なら影の中
            shadow += (currentDepth - bias > closestDepth) ? 0.0f : 1.0f;
        }
    }
    shadow /= 9.0f;  // 9サンプルの平均
    
    return shadow;
}

/// <summary>
/// 比較サンプラーを使用したPCF（より効率的）
/// ハードウェアPCFを活用
/// </summary>
float CalculateShadowHardwarePCF(float4 lightSpacePos, float3 normal) {
    // 透視除算
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    
    // NDC → UV変換
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;
    
    // 範囲外チェック
    if (projCoords.x < 0.0f || projCoords.x > 1.0f ||
        projCoords.y < 0.0f || projCoords.y > 1.0f ||
        projCoords.z > 1.0f) {
        return 1.0f;
    }
    
    // バイアス計算
    float3 lightDir = normalize(-lightDirection);
    float cosTheta = saturate(dot(normal, lightDir));
    float bias = shadowBias + shadowBiasSlope * (1.0f - cosTheta);
    
    float currentDepth = projCoords.z - bias;
    
    // SampleCmpLevelZero: ハードウェアPCF
    // 比較サンプラーを使用して自動的に周辺をサンプリング
    float shadow = shadowMap.SampleCmpLevelZero(
        shadowSampler,
        projCoords.xy,
        currentDepth
    );
    
    return shadow;
}

/// <summary>
/// Blinn-Phong ライティング + シャドウマッピング
/// </summary>
float4 main(VSOutput input) : SV_TARGET {
    // テクスチャカラー
    float4 texColor = diffuseTexture.Sample(diffuseSampler, input.texcoord);
    
    // 法線正規化
    float3 normal = normalize(input.normal);
    
    // ライト方向
    float3 lightDir = normalize(-lightDirection);
    
    // --- Ambient ---
    float3 ambient = 0.15f * lightColor;
    
    // --- Diffuse (Lambert) ---
    float diff = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = diff * lightColor;
    
    // --- Specular (Blinn-Phong) ---
    float3 viewDir = normalize(cameraPos - input.worldPos);
    float3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    float3 specular = spec * lightColor;
    
    // --- Shadow ---
    // ソフトウェアPCFまたはハードウェアPCFを選択
    float shadow = CalculateShadowPCF(input.lightSpacePos, normal);
    // float shadow = CalculateShadowHardwarePCF(input.lightSpacePos, normal);
    
    // 最終カラー計算
    // ambient は影響を受けない
    // diffuse と specular は影の影響を受ける
    float3 lighting = ambient + shadow * (diffuse + specular);
    float3 finalColor = lighting * texColor.rgb;
    
    return float4(finalColor, texColor.a);
}
