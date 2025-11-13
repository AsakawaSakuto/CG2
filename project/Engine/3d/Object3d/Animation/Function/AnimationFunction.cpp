#include "AnimationFunction.h"
#include "../Quaternion/QuaternionFunction.h"
#include "../Easing/Easing.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Animation LoadAnimationFile(const std::string& filename)
{
    Animation animation; // 今回作るアニメーション

    Assimp::Importer importer;
    std::string filePath = filename;

    // モデルと同じでOK。資料では 0 になってるが、既に使ってるフラグがあればそれでいい
    const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
    assert(scene && "Failed to load animation file");
    assert(scene->mNumAnimations != 0 && "No animations in file");

    // 最初のアニメーションだけ採用
    aiAnimation* animationAssimp = scene->mAnimations[0];

    // 時間の単位を秒に変換
    const double ticksPerSecond =
        (animationAssimp->mTicksPerSecond != 0.0)
        ? animationAssimp->mTicksPerSecond
        : 1.0; // 0防止

    animation.duration = static_cast<float>(
        animationAssimp->mDuration / ticksPerSecond);

    // 各ノードの Animation(channel) を解析
    for (uint32_t channelIndex = 0;
        channelIndex < animationAssimp->mNumChannels;
        ++channelIndex)
    {
        aiNodeAnim* nodeAnimationAssimp =
            animationAssimp->mChannels[channelIndex];

        // ノード名で NodeAnimation を取得
        NodeAnimation& nodeAnimation =
            animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

        // --------- Translate ----------
        for (uint32_t keyIndex = 0;
            keyIndex < nodeAnimationAssimp->mNumPositionKeys;
            ++keyIndex)
        {
            aiVectorKey& keyAssimp =
                nodeAnimationAssimp->mPositionKeys[keyIndex];

            KeyframeVector3 keyframe;
            keyframe.time = static_cast<float>(
                keyAssimp.mTime / ticksPerSecond);  // 秒に変換

            // 右手→左手変換（資料どおり：x反転だけ or x,z反転など、
            // 自分のモデル読み込みと揃えること）
            keyframe.value = {
                -keyAssimp.mValue.x,
                 keyAssimp.mValue.y,
                 keyAssimp.mValue.z
            };

            nodeAnimation.translate.keyframes.push_back(keyframe);
        }

        // --------- Rotate ----------
        for (uint32_t keyIndex = 0;
            keyIndex < nodeAnimationAssimp->mNumRotationKeys;
            ++keyIndex)
        {
            aiQuatKey& keyAssimp =
                nodeAnimationAssimp->mRotationKeys[keyIndex];

            KeyframeQuaternion keyframe;
            keyframe.time = static_cast<float>(
                keyAssimp.mTime / ticksPerSecond);

            // Assimp は右手系。自分のエンジンに合わせて変換
            const aiQuaternion& q = keyAssimp.mValue;

            // 例：Y,Z 反転で右手→左手 にするパターン
            Quaternion rotate{};
            rotate.x = q.x;
            rotate.y = -q.y;
            rotate.z = -q.z;
            rotate.w = q.w;

            keyframe.value = rotate;
            nodeAnimation.rotate.keyframes.push_back(keyframe);
        }

        // --------- Scale ----------
        for (uint32_t keyIndex = 0;
            keyIndex < nodeAnimationAssimp->mNumScalingKeys;
            ++keyIndex)
        {
            aiVectorKey& keyAssimp =
                nodeAnimationAssimp->mScalingKeys[keyIndex];

            KeyframeVector3 keyframe;
            keyframe.time = static_cast<float>(
                keyAssimp.mTime / ticksPerSecond);

            // Scale はそのままでOK
            keyframe.value = {
                keyAssimp.mValue.x,
                keyAssimp.mValue.y,
                keyAssimp.mValue.z
            };

            nodeAnimation.scale.keyframes.push_back(keyframe);
        }
    }

    // 解析完了
    return animation;
}

Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time) {

    assert(!keyframes.empty());   // キーがないと値がわからないのでダメ

    // キーが1つだけ、または time が最初のキー以前なら最初の値
    if (keyframes.size() == 1 || time <= keyframes[0].time) {
        return keyframes[0].value;
    }

    // 範囲内を探す
    for (size_t index = 0; index < keyframes.size() - 1; ++index) {
        size_t nextIndex = index + 1;

        if (keyframes[index].time <= time &&
            time <= keyframes[nextIndex].time)
        {
            float t =
                (time - keyframes[index].time) /
                (keyframes[nextIndex].time - keyframes[index].time);

            return Easing::LerpVector3(keyframes[index].value,
                keyframes[nextIndex].value,
                t);
        }
    }

    // ここまで来たら最後のキーより後なので一番最後の値
    return (*keyframes.rbegin()).value;
}


Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {

    assert(!keyframes.empty());

    if (keyframes.size() == 1 || time <= keyframes[0].time) {
        return keyframes[0].value;
    }

    for (size_t index = 0; index < keyframes.size() - 1; ++index) {
        size_t nextIndex = index + 1;

        if (keyframes[index].time <= time &&
            time <= keyframes[nextIndex].time)
        {
            float t =
                (time - keyframes[index].time) /
                (keyframes[nextIndex].time - keyframes[index].time);

            return Slerp(keyframes[index].value,
                keyframes[nextIndex].value,
                t);
        }
    }

    return (*keyframes.rbegin()).value;
}

Matrix4x4 MakeAffineAnimationMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate)
{
    Matrix4x4 m{};

    // 回転行列を取得
    Matrix4x4 rot = MakeRotateMatrix(rotate);

    // 拡大を反映（行列の各要素にスケールを掛ける）
    rot.m[0][0] *= scale.x;
    rot.m[0][1] *= scale.x;
    rot.m[0][2] *= scale.x;

    rot.m[1][0] *= scale.y;
    rot.m[1][1] *= scale.y;
    rot.m[1][2] *= scale.y;

    rot.m[2][0] *= scale.z;
    rot.m[2][1] *= scale.z;
    rot.m[2][2] *= scale.z;

    // 行列にコピー
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            m.m[r][c] = rot.m[r][c];
        }
    }

    // 平行移動
    m.m[3][0] = translate.x;
    m.m[3][1] = translate.y;
    m.m[3][2] = translate.z;

    // 最後の要素
    m.m[3][3] = 1.0f;

    return m;
}