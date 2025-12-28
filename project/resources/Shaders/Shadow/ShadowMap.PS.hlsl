#include "ShadowMap.hlsli"

/// <summary>
/// シャドウマップ生成用 Pixel Shader
/// 深度値の書き込みはハードウェアが自動で行うため、何もしない
/// </summary>
void main(ShadowMapVSOutput input) {
    // 深度バッファへの書き込みは自動的に行われる
    // gl_FragDepth = gl_FragCoord.z; と同等の処理がハードウェアで実行される
    
    // 明示的に深度を書き込む場合は以下のように SV_Depth を使用
    // return input.position.z / input.position.w;
}
