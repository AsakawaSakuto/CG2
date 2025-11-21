#pragma once

// ビネット効果のパラメータ
struct VignetteParams {
    float strength = 0.8f;      // 効果の強さ (0.0～1.0)
    float radius = 0.7f;        // ビネット開始半径 (0.0～1.0)
    float smoothness = 2.0f;    // エッジの滑らかさ (1.0～5.0)
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

// 全ポストエフェクトパラメータを統合
struct PostEffectParams {
    VignetteParams vignette;
    BlurParams blur;
    SepiaParams sepia;
    GrayscaleParams grayscale;
};
