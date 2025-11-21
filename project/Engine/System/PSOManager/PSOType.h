#pragma once

// 事前定義されたPSOタイプ
enum class PSOType {
    // 3Dモデル用
    Model_Solid_Normal,
    Model_Solid_Add,
    Model_Wireframe_Normal,
    Model_Alpha_Normal,

    // スキニングモデル用
    SkinningModel_Solid_Normal,
    SkinningModel_Solid_Add,
    SkinningModel_Wireframe_Normal,
    SkinningModel_Alpha_Normal,

    // スプライト用
    Sprite_Normal,

    // パーティクル用
    Particle_None,
    Particle_Normal,
    Particle_Add,
    Particle_Subtract,
    Particle_Multiply,
    Particle_Screen,

    // オフスクリーン用（ポストエフェクト）
    Offscreen_None,
    Offscreen_Grayscale,
    Offscreen_Sepia,
    Offscreen_Vignette,
    Offscreen_Invert,
    Offscreen_Blur,
};