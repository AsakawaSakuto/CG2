#pragma once
#include"Matrix4x4.h"
#include "Vector3.h"
#include <cmath>
#include <iostream>

// 単位行列
Matrix4x4 MakeIdentityMatrix();

// 行列の積
Matrix4x4 MultiplyMatrix(const Matrix4x4& m1, const Matrix4x4& m2);

// 移動行列
Matrix4x4 MakeTranslateMatrix(const  Vector3& translate);

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const  Vector3& scale);

// 回転行列X
Matrix4x4 MakeRotateXMatrix(float rotate);

// 回転行列Y
Matrix4x4 MakeRotateYMatrix(float rotate);

// 回転行列Z
Matrix4x4 MakeRotateZMatrix(float rotate);

// 回転行列XYZ
Matrix4x4 MakeRotateXYZMatrix(Vector3 rotate);

// アフィン変換
Matrix4x4 MakeAffineMatrix(const  Vector3& scale, const  Vector3& rotate, const  Vector3& translate);

// 逆行列
Matrix4x4 InverseMatrix(Matrix4x4 cameraMatrix);

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// 平行投影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

Matrix4x4 TransposeMatrix(const Matrix4x4& m);