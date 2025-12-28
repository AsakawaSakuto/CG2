// シャドウマッピング付き3Dモデル描画用共通定義

struct VSInput {
    float4 position : POSITION0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float3 worldPos : POSITION0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float4 lightSpacePos : POSITION1;  // ライト空間での座標
};

// 定数バッファ
cbuffer SceneConstants : register(b0) {
    matrix view;
    matrix projection;
    matrix viewProjection;
    float3 cameraPos;
    float padding0;
};

cbuffer ModelConstants : register(b1) {
    matrix world;
    matrix worldInverseTranspose;  // 法線変換用
};

cbuffer LightConstants : register(b2) {
    float3 lightDirection;    // Directional Light方向
    float padding1;
    float3 lightColor;
    float padding2;
    matrix lightSpaceMatrix;  // ライト空間変換行列
    float shadowBias;         // シャドウアクネ対策用バイアス
    float shadowBiasSlope;    // スロープスケールバイアス
    float2 padding3;
};

// テクスチャとサンプラー
Texture2D<float4> diffuseTexture : register(t0);
Texture2D<float> shadowMap : register(t1);
SamplerState diffuseSampler : register(s0);
SamplerComparisonState shadowSampler : register(s1);  // 比較サンプラー
