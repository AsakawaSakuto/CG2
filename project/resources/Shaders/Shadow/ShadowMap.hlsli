// シャドウマップ共通定義

struct ShadowMapVSInput {
    float4 position : POSITION0;
};

struct ShadowMapVSOutput {
    float4 position : SV_POSITION;
};

// 定数バッファ
cbuffer ShadowMapConstants : register(b0) {
    matrix lightSpaceMatrix;  // ライト空間変換行列
    matrix model;             // モデル行列
};
