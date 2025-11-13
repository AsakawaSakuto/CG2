#include "AnimationStruct.h"
#include "../Quaternion/QuaternionFunction.h"
#include "../Easing/Easing.h"

Vector3 CalculateValueVector3(const std::vector<KeyframeVector3>& keyframes, float time) {

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
