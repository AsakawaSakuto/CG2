struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

// ビネット効果用パラメータ
struct VignetteParams
{
    float strength;
    float radius;
    float smoothness;
    float padding;
};

// ブラー効果用パラメータ
struct BlurParams
{
    float amount;
    int sampleCount;
    float2 padding;
};

// セピア/グレースケール用パラメータ
struct ColorEffectParams
{
    float intensity;
    float3 padding;
};

// アウトライン効果用パラメータ
struct OutlineParams
{
    float threshold;
    float thickness;
    float smoothness;
    float backgroundBlend;
    float3 edgeColor;
    float padding;
};