#include <random>

class Random {
public:
    Random() : engine(std::random_device{}()) {}

    // min ～ max の整数乱数を返す
    int Int(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine);
    }

    // min ～ max の浮動小数乱数を返す
    float Float(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(engine);
    }

private:
    std::mt19937 engine;
};