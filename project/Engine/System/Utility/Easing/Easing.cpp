#include "Easing.h"
#include <numbers>

namespace Easing {

    float Apply(float t, Type type) {
        // tを0.0～1.0の範囲にClamp
        t = std::clamp(t, 0.0f, 1.0f);

        switch (type) {
        case Type::Linear:
            return t;

        case Type::EaseInQuad:
            return t * t;

        case Type::EaseOutQuad:
            return 1.0f - (1.0f - t) * (1.0f - t);

        case Type::EaseInOutQuad:
            if (t < 0.5f) {
                return 2.0f * t * t;
            } else {
                return 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
            }

        case Type::EaseOutInQuad:
            if (t < 0.5f) {
                return 0.5f * (1.0f - (1.0f - 2.0f * t) * (1.0f - 2.0f * t));
            } else {
                return 0.5f + 0.5f * (2.0f * t - 1.0f) * (2.0f * t - 1.0f);
            }

        case Type::EaseInCubic:
            return t * t * t;

        case Type::EaseOutCubic:
            return 1.0f - std::pow(1.0f - t, 3.0f);

        case Type::EaseInOutCubic:
            if (t < 0.5f) {
                return 4.0f * t * t * t;
            } else {
                return 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
            }

        case Type::EaseOutInCubic:
            if (t < 0.5f) {
                return 0.5f * (1.0f - std::pow(1.0f - 2.0f * t, 3.0f));
            } else {
                return 0.5f + 0.5f * std::pow(2.0f * t - 1.0f, 3.0f);
            }

        case Type::EaseInQuart:
            return t * t * t * t;

        case Type::EaseOutQuart:
            return 1.0f - std::pow(1.0f - t, 4.0f);

        case Type::EaseInOutQuart:
            if (t < 0.5f) {
                return 8.0f * t * t * t * t;
            } else {
                return 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
            }

        case Type::EaseOutInQuart:
            if (t < 0.5f) {
                return 0.5f * (1.0f - std::pow(1.0f - 2.0f * t, 4.0f));
            } else {
                return 0.5f + 0.5f * std::pow(2.0f * t - 1.0f, 4.0f);
            }

        case Type::EaseInQuint:
            return t * t * t * t * t;

        case Type::EaseOutQuint:
            return 1.0f - std::pow(1.0f - t, 5.0f);

        case Type::EaseInOutQuint:
            if (t < 0.5f) {
                return 16.0f * t * t * t * t * t;
            } else {
                return 1.0f - std::pow(-2.0f * t + 2.0f, 5.0f) / 2.0f;
            }

        case Type::EaseOutInQuint:
            if (t < 0.5f) {
                return 0.5f * (1.0f - std::pow(1.0f - 2.0f * t, 5.0f));
            } else {
                return 0.5f + 0.5f * std::pow(2.0f * t - 1.0f, 5.0f);
            }

        case Type::EaseInSine:
            return 1.0f - std::cos((t * std::numbers::pi_v<float>) / 2.0f);

        case Type::EaseOutSine:
            return std::sin((t * std::numbers::pi_v<float>) / 2.0f);

        case Type::EaseInOutSine:
            return -(std::cos(std::numbers::pi_v<float> *t) - 1.0f) / 2.0f;

        case Type::EaseOutInSine:
            if (t < 0.5f) {
                return 0.5f * std::sin((2.0f * t * std::numbers::pi_v<float>) / 2.0f);
            } else {
                return 1.0f - 0.5f * std::cos(((2.0f * t - 1.0f) * std::numbers::pi_v<float>) / 2.0f);
            }

        case Type::EaseInExpo:
            return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * (t - 1.0f));

        case Type::EaseOutExpo:
            return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);

        case Type::EaseInOutExpo:
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;
            if (t < 0.5f) {
                return std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f;
            }
            else {
                return (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
            }

        case Type::EaseOutInExpo:
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;
            if (t < 0.5f) {
                return 0.5f * (1.0f - std::pow(2.0f, -20.0f * t));
            } else {
                return 0.5f + 0.5f * std::pow(2.0f, 20.0f * (t - 1.0f));
            }

        case Type::EaseInCirc:
            return 1.0f - std::sqrt(1.0f - t * t);

        case Type::EaseOutCirc:
            return std::sqrt(1.0f - std::pow(t - 1.0f, 2.0f));

        case Type::EaseInOutCirc:
            if (t < 0.5f) {
                return (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f;
            }
            else {
                return (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
            }

        case Type::EaseOutInCirc:
            if (t < 0.5f) {
                return 0.5f * std::sqrt(1.0f - std::pow(2.0f * t - 1.0f, 2.0f));
            } else {
                return 1.0f - 0.5f * std::sqrt(1.0f - std::pow(2.0f * t - 1.0f, 2.0f));
            }

        case Type::EaseInBack: {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            return c3 * t * t * t - c1 * t * t;
        }

        case Type::EaseOutBack: {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
        }

        case Type::EaseInOutBack: {
            const float c1 = 1.70158f;
            const float c2 = c1 * 1.525f;
            if (t < 0.5f) {
                return (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f;
            }
            else {
                return (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
            }
        }

        case Type::EaseOutInBack: {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            if (t < 0.5f) {
                return 0.5f * (1.0f + c3 * std::pow(2.0f * t - 1.0f, 3.0f) + c1 * std::pow(2.0f * t - 1.0f, 2.0f));
            } else {
                return 0.5f + 0.5f * (c3 * std::pow(2.0f * t - 1.0f, 3.0f) - c1 * std::pow(2.0f * t - 1.0f, 2.0f));
            }
        }

        case Type::EaseInElastic: {
            const float c4 = (2.0f * std::numbers::pi_v<float>) / 3.0f;
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;
            return -std::pow(2.0f, 10.0f * t - 10.0f) * std::sin((t * 10.0f - 10.75f) * c4);
        }

        case Type::EaseOutElastic: {
            const float c4 = (2.0f * std::numbers::pi_v<float>) / 3.0f;
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;
            return std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
        }

        case Type::EaseInOutElastic: {
            const float c5 = (2.0f * std::numbers::pi_v<float>) / 4.5f;
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;
            if (t < 0.5f) {
                return -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f;
            } else {
                return (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
            }
        }

        case Type::EaseOutInElastic: {
            const float c4 = (2.0f * std::numbers::pi_v<float>) / 3.0f;
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;
            if (t < 0.5f) {
                return 0.5f * (std::pow(2.0f, -20.0f * t) * std::sin((20.0f * t - 0.75f) * c4) + 1.0f);
            } else {
                return 1.0f - 0.5f * std::pow(2.0f, 20.0f * (t - 1.0f)) * std::sin((20.0f * (t - 1.0f) - 10.75f) * c4);
            }
        }

        case Type::EaseInBounce:
            return 1.0f - Apply(1.0f - t, Type::EaseOutBounce);

        case Type::EaseOutBounce: {
            const float n1 = 7.5625f;
            const float d1 = 2.75f;

            if (t < 1.0f / d1) {
                return n1 * t * t;
            }
            else if (t < 2.0f / d1) {
                return n1 * (t -= 1.5f / d1) * t + 0.75f;
            }
            else if (t < 2.5f / d1) {
                return n1 * (t -= 2.25f / d1) * t + 0.9375f;
            }
            else {
                return n1 * (t -= 2.625f / d1) * t + 0.984375f;
            }
        }

        case Type::EaseInOutBounce:
            if (t < 0.5f) {
                return (1.0f - Apply(1.0f - 2.0f * t, Type::EaseOutBounce)) / 2.0f;
            }
            else
            {
                return (1.0f + Apply(2.0f * t - 1.0f, Type::EaseOutBounce)) / 2.0f;
            }

        case Type::EaseOutInBounce:
            if (t < 0.5f) {
                return 0.5f * Apply(2.0f * t, Type::EaseOutBounce);
            } else {
                return 1.0f - 0.5f * Apply(2.0f - 2.0f * t, Type::EaseOutBounce);
            }

        default:
            return t;
        }
    }

    float Lerp(float start, float end, float t, Type type) {
        float easedT = Apply(t, type);
        return start + (end - start) * easedT;
    }

    Vector2 LerpVector2(const Vector2& start, const Vector2& end, float t, Type type) {
        float easedT = Apply(t, type);

        Vector2 diff = end - start;
        Vector2 scaledDiff = diff * easedT;
        return start + scaledDiff;
    }

    Vector3 LerpVector3(const Vector3& start, const Vector3& end, float t, Type type) {
        float easedT = Apply(t, type);

        Vector3 diff = end - start;
        Vector3 scaledDiff = diff * easedT;
        return start + scaledDiff;
    }
}