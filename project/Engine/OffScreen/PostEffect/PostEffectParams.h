#pragma once
#include "Math/Type/Matrix4x4.h"

// ビネット効果のパラメータ
struct VignetteParams {
    float strength = 1.0f;      // 効果の強さ      (0.0～1.0)
    float radius = 0.2f;        // ビネット開始半径 (0.0～1.0)
    float smoothness = 5.0f;    // エッジの滑らかさ (1.0～5.0)
};

// ブラー効果のパラメータ
struct BlurParams {
    float amount = 1.0f;        // ぼかしの量 (0.0～5.0)
    int sampleCount = 9;        // サンプル数 (9 or 25)
};

// セピア効果のパラメータ
struct SepiaParams {
    float intensity = 1.0f;     // セピアの強度 (0.0～1.0)
};

// グレースケール効果のパラメータ
struct GrayscaleParams {
    float intensity = 1.0f;     // グレースケールの強度 (0.0～1.0)
};

// ラジアルブラー効果のパラメータ
struct RadialBlurParams {
    float centerX = 0.5f;       // ブラーの中心X座標 (0.0～1.0)
    float centerY = 0.5f;       // ブラーの中心Y座標 (0.0～1.0)
    float strength = 0.1f;      // ブラーの強さ (0.0～1.0)
    int sampleCount = 10;       // サンプル数 (5～20)
};

// アウトライン効果のパラメータ（Depthベース）
struct OutlineParams {
    Matrix4x4 projectionInverse;  // P^-1（NDC→View変換用）
    float uvStepSize[2];          // (1/width, 1/height)
    float thickness;              // アウトラインの太さ (1.0～5.0)
    float depthSensitivity;       // 深度の感度 (0.1～10.0)
    float outlineColor[3];        // アウトラインの色 (RGB: 0.0～1.0)
    float padding;                // アライメント用パディング

    OutlineParams() : thickness(1.0f), depthSensitivity(1.0f), padding(0.0f) {
        uvStepSize[0] = 0.0f;
        uvStepSize[1] = 0.0f;
        outlineColor[0] = 0.0f;   // デフォルトは黒
        outlineColor[1] = 0.0f;
        outlineColor[2] = 0.0f;
    }
};

// フォグ効果のパラメータ（Depthベース）
struct FogParams {
    Matrix4x4 projectionInverse;  // P^-1（NDC→View変換用）
    float fogColor[3];            // フォグの色 (RGB: 0.0～1.0)
    float fogStart;               // フォグ開始距離
    float fogEnd;                 // フォグ終了距離（完全に霧になる距離）
    float fogDensity;             // フォグの濃度 (0.0～1.0)
    float padding[2];             // アライメント用パディング

    FogParams() : fogStart(5.0f), fogEnd(50.0f), fogDensity(1.0f) {
        fogColor[0] = 0.7f;       // デフォルトは薄い灰色
        fogColor[1] = 0.7f;
        fogColor[2] = 0.8f;
        padding[0] = 0.0f;
        padding[1] = 0.0f;
    }
};

// 全ポストエフェクトパラメータを統合
struct PostEffectParams {
    VignetteParams vignette;
    BlurParams blur;
    SepiaParams sepia;
    GrayscaleParams grayscale;
    RadialBlurParams radialBlur;
    OutlineParams outline;
    FogParams fog;
};
