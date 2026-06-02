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

// ディゾルブ効果用パラメータ（32バイトアライメント）
struct DissolveParams
{
    float3 edgeColor; // エッジの色 (RGB)
    float threshold; // ディゾルブの閾値 (0.0 ～ 1.0)
    float edgeRange; // エッジの幅 (0.0 ～ 0.1)
    float3 padding; // C++側の padding[3] に対応するアライメント調整用
};

// ディゾルブ用定数バッファのバインド
ConstantBuffer<DissolveParams> gDissolveParams : register(b2);