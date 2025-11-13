#pragma once
#include "../Math/Vector3.h"
#include "../Math/Matrix4x4.h"
#include "../Quaternion/Quaternion.h"
#include "../Model/Animation/AnimationStruct.h"
#include <vector>
#include <cassert> 

Animation LoadAnimationFile(const std::string& filename);

Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

Matrix4x4 MakeAffineAnimationMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);