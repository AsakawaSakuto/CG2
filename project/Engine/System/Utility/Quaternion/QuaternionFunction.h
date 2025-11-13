#pragma once
#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "../MathFunction/MathFunction.h"
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

// 任意軸回転を表すQuaternionの生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& v, const Quaternion& q);

// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion& q);

// 球面線形補間 Slerp
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);