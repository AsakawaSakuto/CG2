#include"MatrixFunction.h"

// 単位行列
Matrix4x4 MakeIdentityMatrix() {
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
Matrix4x4 MultiplyMatrix(const Matrix4x4& v1, const Matrix4x4& v2) {
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

// 回転行列XYZ
Matrix4x4 MakeRotateXYZMatrix(Vector3 rotate) {
	// 各軸の回転行列を生成
	Matrix4x4 rotX = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotY = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotZ = MakeRotateZMatrix(rotate.z);

	// ローカル空間の回転順 Z → Y → X（右から適用される）
	return MultiplyMatrix(MultiplyMatrix(rotZ, rotY), rotX);
}

// アフィン変換
Matrix4x4 MakeAffineMatrix(const  Vector3& scale, const  Vector3& rotate, const  Vector3& translate) {
	Matrix4x4 result{};

	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = MultiplyMatrix(rotateXMatrix, MultiplyMatrix(rotateYMatrix, rotateZMatrix));

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
Matrix4x4 InverseMatrix(Matrix4x4 cameraMatrix)
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

	result.m[0][0] = (1.0f / aspectRatio) * 1.0f / std::tan(fovY / 2.0f);
	result.m[1][1] = 1.0f / std::tan(fovY / 2.0f);
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return result;
}

// 平行投影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result{};
	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 TransposeMatrix(const Matrix4x4& m) {
	Matrix4x4 result;

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			result.m[row][col] = m.m[col][row];
		}
	}

	return result;
}

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {

	Matrix4x4 result;

	result.m[0][0] = width / 2;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = -height / 2;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[2][3] = 0.0f;

	result.m[3][0] = left + (width / 2);
	result.m[3][1] = top + (height / 2);
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;

	return result;
}

Vector3 TransformVtoM(const Vector3& v, const Matrix4x4& m) {
	Vector3 result;
	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];
	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];
	result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	// w除算で正規化（透視変換対応）
	if (w != 0.0f) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}

	return result;
}

float Dot_(const Vector3& a, const Vector3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

bool IsCollideSphere(const Vector3& centerA, float radiusA,
	const Vector3& centerB, float radiusB)
{
	// 中心間ベクトル
	Vector3 d = { centerB.x - centerA.x, centerB.y - centerA.y, centerB.z - centerA.z };

	// 中心間距離の二乗
	float dist2 = Dot_(d, d);

	// 半径の和
	float r = radiusA + radiusB;

	// 当たり判定：中心間距離 <= 半径の和
	return dist2 <= (r * r);
}

float Lerp(float start, float end, float t) {
	return start + (end - start) * t;
};