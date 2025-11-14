#include "AnimationFunction.h"
#include "../MatrixFunction/MatrixFunction.h"
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

Skeleton CreateSkeleton(const ModelNode& rootNode) {
    Skeleton skeleton;
    skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

    // 名前とindexのマッピングを行いアクセスしやすくする
    for (const Joint& joint : skeleton.joints) {
        skeleton.jointMap.emplace(joint.name, joint.index);
    }

    return skeleton;
}

int32_t CreateJoint(const ModelNode& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {

    Joint joint;
    joint.name = node.name;
    joint.localMatrix = node.localMatrix;
    joint.skeletonSpaceMatrix = MakeIdentityMatrix();
    joint.transform = node.transform;
    joint.index = int32_t(joints.size()); // 現在登録されている数をIndexに
    joint.parent = parent;
    joints.push_back(joint); // SkeletonのJoint列に追加

    // 子Jontを作成し、そのIndexを登録
    for (const ModelNode& child : node.children) {
        int32_t childIndex = CreateJoint(child, joint.index, joints);
        joints[joint.index].children.push_back(childIndex);
    }

    // 自身のIndexを返す
    return joint.index;
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

void UpdateAnimation(Skeleton& skeleton) {
    // すべてのJointを更新。親が若いので通常ループで処理可能になっている
    for (Joint& joint : skeleton.joints) {
        joint.localMatrix = MakeAffineAnimationMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
        if (joint.parent) { // 親がいれば親の行列を掛ける
            joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
        } else { // 親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
            joint.skeletonSpaceMatrix = joint.localMatrix;
        }
    }
}

void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime) {
    for (Joint& joint : skeleton.joints) {
        // 対象のJointのAnimationがあれば、値の適用を行う。下記のif文はC++17から可能になった初期化付きif文。
        if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
            const NodeAnimation& rootNodeAnimation = (*it).second;
            joint.transform.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
            joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
            joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
        }
    }
}