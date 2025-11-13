#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include <string>
#include <vector>
#include <unordered_map>

struct Bone {
    std::string name;
    int parentIndex;         // -1 なら root
    Matrix4x4 localBindPose; // バインドポーズ時のローカル行列
    Matrix4x4 inverseBind;   // 逆バインド行列
};


struct Skeleton {
    std::vector<Bone> bones;
    // 名前 → インデックスのマップもあると便利
    std::unordered_map<std::string, int> boneNameToIndex;
};

struct Quaternion {
    float x; 
    float y; 
    float z; 
    float w;
};

struct Keyframe {
    float time;
    Vector3 translate;
    Quaternion rotate;
    Vector3 scale;
};

struct BoneAnimation {
    std::vector<Keyframe> keyframes;
};

struct AnimationClip {
    std::string name;
    float duration;
    std::vector<BoneAnimation> boneAnimations; // [boneCount]
};
