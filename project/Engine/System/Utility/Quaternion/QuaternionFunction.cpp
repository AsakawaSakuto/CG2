#include "QuaternionFunction.h"

// Quaternionの積（ハミルトン積）
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
    Quaternion result{};

    result.w = lhs.w * rhs.w
        - lhs.x * rhs.x
        - lhs.y * rhs.y
        - lhs.z * rhs.z;

    result.x = lhs.w * rhs.x
        + lhs.x * rhs.w
        + lhs.y * rhs.z
        - lhs.z * rhs.y;

    result.y = lhs.w * rhs.y
        - lhs.x * rhs.z
        + lhs.y * rhs.w
        + lhs.z * rhs.x;

    result.z = lhs.w * rhs.z
        + lhs.x * rhs.y
        - lhs.y * rhs.x
        + lhs.z * rhs.w;

    return result;
}

// 単位Quaternionを返す
Quaternion IdentityQuaternion() {
    return { 0.0f, 0.0f, 0.0f, 1.0f };
}

// 共役Quaternionを返す
Quaternion Conjugate(const Quaternion& quaternion) {
    return { -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
}

// Quaternionのnorm（長さ）を返す
float Norm(const Quaternion& quaternion) {
    return std::sqrt(
        quaternion.x * quaternion.x +
        quaternion.y * quaternion.y +
        quaternion.z * quaternion.z +
        quaternion.w * quaternion.w);
}

// 正規化したQuaternionを返す
Quaternion Normalize(const Quaternion& quaternion) {
    float n = Norm(quaternion);
    if (n == 0.0f) {
        // 0除算を避ける。ここは課題の方針に合わせて変更してOK
        return { 0.0f, 0.0f, 0.0f, 0.0f };
    }
    float invN = 1.0f / n;
    return {
        quaternion.x * invN,
        quaternion.y * invN,
        quaternion.z * invN,
        quaternion.w * invN
    };
}

// 逆Quaternionを返す
Quaternion Inverse(const Quaternion& quaternion) {
    // |q|^2
    float n2 =
        quaternion.x * quaternion.x +
        quaternion.y * quaternion.y +
        quaternion.z * quaternion.z +
        quaternion.w * quaternion.w;

    if (n2 == 0.0f) {
        // 逆が定義できないのでゼロを返す（ここも方針に合わせてOK）
        return { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    Quaternion conj = Conjugate(quaternion);
    float invN2 = 1.0f / n2;

    return {
        conj.x * invN2,
        conj.y * invN2,
        conj.z * invN2,
        conj.w * invN2
    };
}