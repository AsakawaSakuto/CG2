#pragma once
#include "../Math/Vector3.h"
#include "../Math/Matrix4x4.h"
#include "../Quaternion/Quaternion.h"
#include "../Model/Animation/AnimationStruct.h"
#include "../Model/ModelDataStruct.h"
#include <vector>
#include <cassert> 

Animation LoadAnimationFile(const std::string& filename);

Skeleton CreateSkeleton(const ModelNode& rootNode);

int32_t CreateJoint(const ModelNode& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

void UpdateAnimation(Skeleton& skeleton);

void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);