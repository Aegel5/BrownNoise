#pragma once

class Red1 {
    float prev = (v_max + v_min)/2;
    static constexpr float v_min = -0.5f;   // Укажите ваши границы
    static constexpr float v_max = 0.5f;
public:
    float Next() {

        bool isUp = Rand::rand_bool();
        float rem = isUp ? (v_max - prev) : (prev - v_min);
        float step = rem / 50.0f;

        if (isUp) prev += step;
        else      prev -= step;

        // Clamp обязателен, чтобы избежать микро-ошибок float на границах
        return prev = std::clamp(prev, v_min, v_max);
    }
};