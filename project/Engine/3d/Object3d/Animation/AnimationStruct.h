#pragma once
#include "../Quaternion/Quaternion.h"
#include "../Math/Vector3.h"
#include <vector>
#include <map>
#include <cassert> 

template <typename tValue>
struct Keyframe {
    float time;
    tValue value;
};

using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

template<typename tValue>
struct AnimationCurve {
    std::vector<Keyframe<tValue>> keyframes;
};

struct NodeAnimation {
    AnimationCurve<Vector3> translate;
    AnimationCurve<Quaternion> rotate;
    AnimationCurve<Vector3> scale;
};

struct Animation {
    float duration;  // アニメーション全体の尺（単位は秒）
    // NodeAnimationの集合。Node名でひけるようにしておく
    std::map<std::string, NodeAnimation> nodeAnimations;
};

// 関数宣言
Vector3 CalculateValueVector3(const std::vector<KeyframeVector3>& keyframes, float time);

Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
