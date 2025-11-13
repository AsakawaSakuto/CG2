#pragma once
#include "Quaternion.h"
#include <cmath>

// Quaternionの積（ハミルトン積）
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

// 単位Quaternionを返す
Quaternion IdentityQuaternion();

// 共役Quaternionを返す
Quaternion Conjugate(const Quaternion& quaternion);

// Quaternionのnorm（長さ）を返す
float Norm(const Quaternion& quaternion);

// 正規化したQuaternionを返す
Quaternion Normalize(const Quaternion& quaternion);

// 逆Quaternionを返す
Quaternion Inverse(const Quaternion& quaternion);