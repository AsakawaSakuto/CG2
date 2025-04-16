#include"Matrix.h"

// 単位行列
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result{};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j) {
				result.m[j][i] = 1.0f;
			} else {
				result.m[j][i] = 0.0f;
			}
		}
	}
	return result;
}

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& v1, const Matrix4x4& v2) {
	Matrix4x4 result{};

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			for (int z = 0; z < 4; z++) {
				result.m[y][x] += v1.m[y][z] * v2.m[z][x];
			}
		}
	}

	return result;
};

// 移動行列
Matrix4x4 MakeTranslateMatrix(const  Vector3& translate) {
	Matrix4x4 result{};

	result.m[0][0] = 1.f;
	result.m[0][1] = 0.f;
	result.m[0][2] = 0.f;
	result.m[0][3] = 0.f;

	result.m[1][0] = 0.f;
	result.m[1][1] = 1.f;
	result.m[1][2] = 0.f;
	result.m[1][3] = 0.f;

	result.m[2][0] = 0.f;
	result.m[2][1] = 0.f;
	result.m[2][2] = 1.f;
	result.m[2][3] = 0.f;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.f;

	return result;
}

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const  Vector3& scale) {
	Matrix4x4 result{};

	result.m[0][0] = scale.x;
	result.m[0][1] = 0.f;
	result.m[0][2] = 0.f;
	result.m[0][3] = 0.f;

	result.m[1][0] = 0.f;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0.f;
	result.m[1][3] = 0.f;

	result.m[2][0] = 0.f;
	result.m[2][1] = 0.f;
	result.m[2][2] = scale.z;
	result.m[2][3] = 0.f;

	result.m[3][0] = 0.f;
	result.m[3][1] = 0.f;
	result.m[3][2] = 0.f;
	result.m[3][3] = 1.f;

	return result;
}

// 回転行列X
Matrix4x4 MakeRotateXMatrix(float rotate) {
	Matrix4x4 result{};

	result.m[0][0] = 1.f;
	result.m[0][1] = 0.f;
	result.m[0][2] = 0.f;
	result.m[0][3] = 0.f;
	result.m[1][0] = 0.f;
	result.m[1][1] = std::cos(rotate);
	result.m[1][2] = std::sin(rotate);
	result.m[1][3] = 0.f;
	result.m[2][0] = 0.f;
	result.m[2][1] = -std::sin(rotate);
	result.m[2][2] = std::cos(rotate);
	result.m[2][3] = 0.f;
	result.m[3][0] = 0.f;
	result.m[3][1] = 0.f;
	result.m[3][2] = 0.f;
	result.m[3][3] = 1.f;

	return result;
}
// 回転行列Y
Matrix4x4 MakeRotateYMatrix(float rotate) {
	Matrix4x4 result{};

	result.m[0][0] = std::cos(rotate);
	result.m[0][1] = 0.f;
	result.m[0][2] = -std::sin(rotate);;
	result.m[0][3] = 0.f;
	result.m[1][0] = 0.f;
	result.m[1][1] = 1.f;
	result.m[1][2] = 0.f;
	result.m[1][3] = 0.f;
	result.m[2][0] = std::sin(rotate);
	result.m[2][1] = 0.f;
	result.m[2][2] = std::cos(rotate);
	result.m[2][3] = 0.f;
	result.m[3][0] = 0.f;
	result.m[3][1] = 0.f;
	result.m[3][2] = 0.f;
	result.m[3][3] = 1.f;

	return result;
}
// 回転行列Z
Matrix4x4 MakeRotateZMatrix(float rotate) {
	Matrix4x4 result{};

	result.m[0][0] = std::cos(rotate);
	result.m[0][1] = std::sin(rotate);
	result.m[0][2] = 0.f;
	result.m[0][3] = 0.f;
	result.m[1][0] = -std::sin(rotate);
	result.m[1][1] = std::cos(rotate);
	result.m[1][2] = 0.f;
	result.m[1][3] = 0.f;
	result.m[2][0] = 0.f;
	result.m[2][1] = 0.f;
	result.m[2][2] = 1.f;
	result.m[2][3] = 0.f;
	result.m[3][0] = 0.f;
	result.m[3][1] = 0.f;
	result.m[3][2] = 0.f;
	result.m[3][3] = 1.f;

	return result;
}

// アフィン変換
Matrix4x4 MakeAffineMatrix(const  Vector3& scale, const  Vector3& rotate, const  Vector3& translate) {
	Matrix4x4 result{};

	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	result.m[0][0] = scale.x * rotateXYZMatrix.m[0][0];
	result.m[0][1] = scale.x * rotateXYZMatrix.m[0][1];
	result.m[0][2] = scale.x * rotateXYZMatrix.m[0][2];
	result.m[0][3] = 0.f;
	result.m[1][0] = scale.y * rotateXYZMatrix.m[1][0];
	result.m[1][1] = scale.y * rotateXYZMatrix.m[1][1];
	result.m[1][2] = scale.y * rotateXYZMatrix.m[1][2];
	result.m[1][3] = 0.f;
	result.m[2][0] = scale.z * rotateXYZMatrix.m[2][0];
	result.m[2][1] = scale.z * rotateXYZMatrix.m[2][1];
	result.m[2][2] = scale.z * rotateXYZMatrix.m[2][2];
	result.m[2][3] = 0.f;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.f;

	return result;
};

// 逆行列
Matrix4x4 Inverse(Matrix4x4 cameraMatrix)
{
	Matrix4x4 result{};

	float abs;//絶対値はint型にする

	// |A|
	abs = (cameraMatrix.m[0][0] * cameraMatrix.m[1][1] * cameraMatrix.m[2][2] * cameraMatrix.m[3][3]) + (cameraMatrix.m[0][0] * cameraMatrix.m[1][2] * cameraMatrix.m[2][3] * cameraMatrix.m[3][1]) + (cameraMatrix.m[0][0] * cameraMatrix.m[1][3] * cameraMatrix.m[2][1] * cameraMatrix.m[3][2])
		- (cameraMatrix.m[0][0] * cameraMatrix.m[1][3] * cameraMatrix.m[2][2] * cameraMatrix.m[3][1]) - (cameraMatrix.m[0][0] * cameraMatrix.m[1][2] * cameraMatrix.m[2][1] * cameraMatrix.m[3][3]) - (cameraMatrix.m[0][0] * cameraMatrix.m[1][1] * cameraMatrix.m[2][3] * cameraMatrix.m[3][2])
		- (cameraMatrix.m[0][1] * cameraMatrix.m[1][0] * cameraMatrix.m[2][2] * cameraMatrix.m[3][3]) - (cameraMatrix.m[0][2] * cameraMatrix.m[1][0] * cameraMatrix.m[2][3] * cameraMatrix.m[3][1]) - (cameraMatrix.m[0][3] * cameraMatrix.m[1][0] * cameraMatrix.m[2][1] * cameraMatrix.m[3][2])
		+ (cameraMatrix.m[0][3] * cameraMatrix.m[1][0] * cameraMatrix.m[2][2] * cameraMatrix.m[3][1]) + (cameraMatrix.m[0][2] * cameraMatrix.m[1][0] * cameraMatrix.m[2][1] * cameraMatrix.m[3][3]) + (cameraMatrix.m[0][1] * cameraMatrix.m[1][0] * cameraMatrix.m[2][3] * cameraMatrix.m[3][2])
		+ (cameraMatrix.m[0][1] * cameraMatrix.m[1][2] * cameraMatrix.m[2][0] * cameraMatrix.m[3][3]) + (cameraMatrix.m[0][2] * cameraMatrix.m[1][3] * cameraMatrix.m[2][0] * cameraMatrix.m[3][1]) + (cameraMatrix.m[0][3] * cameraMatrix.m[1][1] * cameraMatrix.m[2][0] * cameraMatrix.m[3][2])
		- (cameraMatrix.m[0][3] * cameraMatrix.m[1][2] * cameraMatrix.m[2][0] * cameraMatrix.m[3][1]) - (cameraMatrix.m[0][2] * cameraMatrix.m[1][1] * cameraMatrix.m[2][0] * cameraMatrix.m[3][3]) - (cameraMatrix.m[0][1] * cameraMatrix.m[1][3] * cameraMatrix.m[2][0] * cameraMatrix.m[3][2])
		- (cameraMatrix.m[0][1] * cameraMatrix.m[1][2] * cameraMatrix.m[2][3] * cameraMatrix.m[3][0]) - (cameraMatrix.m[0][2] * cameraMatrix.m[1][3] * cameraMatrix.m[2][1] * cameraMatrix.m[3][0]) - (cameraMatrix.m[0][3] * cameraMatrix.m[1][1] * cameraMatrix.m[2][2] * cameraMatrix.m[3][0])
		+ (cameraMatrix.m[0][3] * cameraMatrix.m[1][2] * cameraMatrix.m[2][1] * cameraMatrix.m[3][0]) + (cameraMatrix.m[0][2] * cameraMatrix.m[1][1] * cameraMatrix.m[2][3] * cameraMatrix.m[3][0]) + (cameraMatrix.m[0][1] * cameraMatrix.m[1][3] * cameraMatrix.m[2][2] * cameraMatrix.m[3][0]
			);

	// 1/A
	result.m[0][0] = 1.0f / abs * (
		(cameraMatrix.m[1][1] * cameraMatrix.m[2][2] * cameraMatrix.m[3][3]) + (cameraMatrix.m[1][2] * cameraMatrix.m[2][3] * cameraMatrix.m[3][1]) + (cameraMatrix.m[1][3] * cameraMatrix.m[2][1] * cameraMatrix.m[3][2])
		- (cameraMatrix.m[1][3] * cameraMatrix.m[2][2] * cameraMatrix.m[3][1]) - (cameraMatrix.m[1][2] * cameraMatrix.m[2][1] * cameraMatrix.m[3][3]) - (cameraMatrix.m[1][1] * cameraMatrix.m[2][3] * cameraMatrix.m[3][2])
		);
	result.m[0][1] = 1.0f / abs * (
		-(cameraMatrix.m[0][1] * cameraMatrix.m[2][2] * cameraMatrix.m[3][3]) - (cameraMatrix.m[0][2] * cameraMatrix.m[2][3] * cameraMatrix.m[3][1]) - (cameraMatrix.m[0][3] * cameraMatrix.m[2][1] * cameraMatrix.m[3][2])
		+ cameraMatrix.m[0][3] * cameraMatrix.m[2][2] * cameraMatrix.m[3][1] + cameraMatrix.m[0][2] * cameraMatrix.m[2][1] * cameraMatrix.m[3][3] + cameraMatrix.m[0][1] * cameraMatrix.m[2][3] * cameraMatrix.m[3][2]
		);
	result.m[0][2] = 1.0f / abs * (
		(cameraMatrix.m[0][1] * cameraMatrix.m[1][2] * cameraMatrix.m[3][3]) + (cameraMatrix.m[0][2] * cameraMatrix.m[1][3] * cameraMatrix.m[3][1]) + (cameraMatrix.m[0][3] * cameraMatrix.m[1][1] * cameraMatrix.m[3][2])
		- (cameraMatrix.m[0][3] * cameraMatrix.m[1][2] * cameraMatrix.m[3][1]) - (cameraMatrix.m[0][2] * cameraMatrix.m[1][1] * cameraMatrix.m[3][3]) - (cameraMatrix.m[0][1] * cameraMatrix.m[1][3] * cameraMatrix.m[3][2])
		);
	result.m[0][3] = 1.0f / abs * (
		-(cameraMatrix.m[0][1] * cameraMatrix.m[1][2] * cameraMatrix.m[2][3]) - (cameraMatrix.m[0][2] * cameraMatrix.m[1][3] * cameraMatrix.m[2][1]) - (cameraMatrix.m[0][3] * cameraMatrix.m[1][1] * cameraMatrix.m[2][2])
		+ (cameraMatrix.m[0][3] * cameraMatrix.m[1][2] * cameraMatrix.m[2][1]) + (cameraMatrix.m[0][2] * cameraMatrix.m[1][1] * cameraMatrix.m[2][3]) + (cameraMatrix.m[0][1] * cameraMatrix.m[1][3] * cameraMatrix.m[2][2])
		);

	result.m[1][0] = 1.0f / abs * (
		-(cameraMatrix.m[1][0] * cameraMatrix.m[2][2] * cameraMatrix.m[3][3]) - (cameraMatrix.m[1][2] * cameraMatrix.m[2][3] * cameraMatrix.m[3][0]) - (cameraMatrix.m[1][3] * cameraMatrix.m[2][0] * cameraMatrix.m[3][2])
		+ (cameraMatrix.m[1][3] * cameraMatrix.m[2][2] * cameraMatrix.m[3][0]) + (cameraMatrix.m[1][2] * cameraMatrix.m[2][0] * cameraMatrix.m[3][3]) + (cameraMatrix.m[1][0] * cameraMatrix.m[2][3] * cameraMatrix.m[3][2])
		);
	result.m[1][1] = 1.0f / abs * (
		(cameraMatrix.m[0][0] * cameraMatrix.m[2][2] * cameraMatrix.m[3][3]) + (cameraMatrix.m[0][2] * cameraMatrix.m[2][3] * cameraMatrix.m[3][0]) + (cameraMatrix.m[0][3] * cameraMatrix.m[2][0] * cameraMatrix.m[3][2])
		- (cameraMatrix.m[0][3] * cameraMatrix.m[2][2] * cameraMatrix.m[3][0]) - (cameraMatrix.m[0][2] * cameraMatrix.m[2][0] * cameraMatrix.m[3][3]) - (cameraMatrix.m[0][0] * cameraMatrix.m[2][3] * cameraMatrix.m[3][2])
		);
	result.m[1][2] = 1.0f / abs * (
		-(cameraMatrix.m[0][0] * cameraMatrix.m[1][2] * cameraMatrix.m[3][3]) - (cameraMatrix.m[0][2] * cameraMatrix.m[1][3] * cameraMatrix.m[3][0]) - (cameraMatrix.m[0][3] * cameraMatrix.m[1][0] * cameraMatrix.m[3][2])
		+ (cameraMatrix.m[0][3] * cameraMatrix.m[1][2] * cameraMatrix.m[3][0]) + (cameraMatrix.m[0][2] * cameraMatrix.m[1][0] * cameraMatrix.m[3][3]) + (cameraMatrix.m[0][0] * cameraMatrix.m[1][3] * cameraMatrix.m[3][2])
		);
	result.m[1][3] = 1.0f / abs * (
		(cameraMatrix.m[0][0] * cameraMatrix.m[1][2] * cameraMatrix.m[2][3]) + (cameraMatrix.m[0][2] * cameraMatrix.m[1][3] * cameraMatrix.m[2][0]) + (cameraMatrix.m[0][3] * cameraMatrix.m[1][0] * cameraMatrix.m[2][2])
		- (cameraMatrix.m[0][3] * cameraMatrix.m[1][2] * cameraMatrix.m[2][0]) - (cameraMatrix.m[0][2] * cameraMatrix.m[1][0] * cameraMatrix.m[2][3]) - (cameraMatrix.m[0][0] * cameraMatrix.m[1][3] * cameraMatrix.m[2][2])
		);

	result.m[2][0] = 1.0f / abs * (
		(cameraMatrix.m[1][0] * cameraMatrix.m[2][1] * cameraMatrix.m[3][3]) + (cameraMatrix.m[1][1] * cameraMatrix.m[2][3] * cameraMatrix.m[3][0]) + (cameraMatrix.m[1][3] * cameraMatrix.m[2][0] * cameraMatrix.m[3][1])
		- (cameraMatrix.m[1][3] * cameraMatrix.m[2][1] * cameraMatrix.m[3][0]) - (cameraMatrix.m[1][1] * cameraMatrix.m[2][0] * cameraMatrix.m[3][3]) - (cameraMatrix.m[1][0] * cameraMatrix.m[2][3] * cameraMatrix.m[3][1])
		);
	result.m[2][1] = 1.0f / abs * (
		-(cameraMatrix.m[0][0] * cameraMatrix.m[2][1] * cameraMatrix.m[3][3]) - (cameraMatrix.m[0][1] * cameraMatrix.m[2][3] * cameraMatrix.m[3][0]) - (cameraMatrix.m[0][3] * cameraMatrix.m[2][0] * cameraMatrix.m[3][1])
		+ (cameraMatrix.m[0][3] * cameraMatrix.m[2][1] * cameraMatrix.m[3][0]) + (cameraMatrix.m[0][1] * cameraMatrix.m[2][0] * cameraMatrix.m[3][3]) + (cameraMatrix.m[0][0] * cameraMatrix.m[2][3] * cameraMatrix.m[3][1])
		);
	result.m[2][2] = 1.0f / abs * (
		(cameraMatrix.m[0][0] * cameraMatrix.m[1][1] * cameraMatrix.m[3][3]) + (cameraMatrix.m[0][1] * cameraMatrix.m[1][3] * cameraMatrix.m[3][0]) + (cameraMatrix.m[0][3] * cameraMatrix.m[1][0] * cameraMatrix.m[3][1])
		- (cameraMatrix.m[0][3] * cameraMatrix.m[1][1] * cameraMatrix.m[3][0]) - (cameraMatrix.m[0][1] * cameraMatrix.m[1][0] * cameraMatrix.m[3][3]) - (cameraMatrix.m[0][0] * cameraMatrix.m[1][3] * cameraMatrix.m[3][1])
		);
	result.m[2][3] = 1.0f / abs * (
		-(cameraMatrix.m[0][0] * cameraMatrix.m[1][1] * cameraMatrix.m[2][3]) - (cameraMatrix.m[0][1] * cameraMatrix.m[1][3] * cameraMatrix.m[2][0]) - (cameraMatrix.m[0][3] * cameraMatrix.m[1][0] * cameraMatrix.m[2][1])
		+ (cameraMatrix.m[0][3] * cameraMatrix.m[1][1] * cameraMatrix.m[2][0]) + (cameraMatrix.m[0][1] * cameraMatrix.m[1][0] * cameraMatrix.m[2][3]) + (cameraMatrix.m[0][0] * cameraMatrix.m[1][3] * cameraMatrix.m[2][1])
		);

	result.m[3][0] = 1.0f / abs * (
		-(cameraMatrix.m[1][0] * cameraMatrix.m[2][1] * cameraMatrix.m[3][2]) - (cameraMatrix.m[1][1] * cameraMatrix.m[2][2] * cameraMatrix.m[3][0]) - (cameraMatrix.m[1][2] * cameraMatrix.m[2][0] * cameraMatrix.m[3][1])
		+ (cameraMatrix.m[1][2] * cameraMatrix.m[2][1] * cameraMatrix.m[3][0]) + (cameraMatrix.m[1][1] * cameraMatrix.m[2][0] * cameraMatrix.m[3][2]) + (cameraMatrix.m[1][0] * cameraMatrix.m[2][2] * cameraMatrix.m[3][1])
		);
	result.m[3][1] = 1.0f / abs * (
		(cameraMatrix.m[0][0] * cameraMatrix.m[2][1] * cameraMatrix.m[3][2]) + (cameraMatrix.m[0][1] * cameraMatrix.m[2][2] * cameraMatrix.m[3][0]) + (cameraMatrix.m[0][2] * cameraMatrix.m[2][0] * cameraMatrix.m[3][1])
		- (cameraMatrix.m[0][2] * cameraMatrix.m[2][1] * cameraMatrix.m[3][0]) - (cameraMatrix.m[0][1] * cameraMatrix.m[2][0] * cameraMatrix.m[3][2]) - (cameraMatrix.m[0][0] * cameraMatrix.m[2][2] * cameraMatrix.m[3][1])
		);
	result.m[3][2] = 1.0f / abs * (
		-(cameraMatrix.m[0][0] * cameraMatrix.m[1][1] * cameraMatrix.m[3][2]) - (cameraMatrix.m[0][1] * cameraMatrix.m[1][2] * cameraMatrix.m[3][0]) - (cameraMatrix.m[0][2] * cameraMatrix.m[1][0] * cameraMatrix.m[3][1])
		+ (cameraMatrix.m[0][2] * cameraMatrix.m[1][1] * cameraMatrix.m[3][0]) + (cameraMatrix.m[0][1] * cameraMatrix.m[1][0] * cameraMatrix.m[3][2]) + (cameraMatrix.m[0][0] * cameraMatrix.m[1][2] * cameraMatrix.m[3][1])
		);
	result.m[3][3] = 1.0f / abs * (
		(cameraMatrix.m[0][0] * cameraMatrix.m[1][1] * cameraMatrix.m[2][2]) + (cameraMatrix.m[0][1] * cameraMatrix.m[1][2] * cameraMatrix.m[2][0]) + (cameraMatrix.m[0][2] * cameraMatrix.m[1][0] * cameraMatrix.m[2][1])
		- (cameraMatrix.m[0][2] * cameraMatrix.m[1][1] * cameraMatrix.m[2][0]) - (cameraMatrix.m[0][1] * cameraMatrix.m[1][0] * cameraMatrix.m[2][2]) - (cameraMatrix.m[0][0] * cameraMatrix.m[1][2] * cameraMatrix.m[2][1])
		);


	return result;
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
	Matrix4x4 result{};

	result.m[0][0] = (1 / aspectRatio) * 1 / std::tan(fovY / 2);
	result.m[1][1] = 1 / std::tan(fovY / 2);
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return result;
}