#pragma once

class Red1 {
    float prev = 0.5f;
    static constexpr float v_min = 0.0f;   // Укажите ваши границы
    static constexpr float v_max = 1.0f;
    float velocity = 0.0f; // Текущая скорость изменения
public:
    float Next() {
        //// Добавляем случайный "толчок" к скорости (акселерация)
        //float accel = Rand::rand_real(-0.5f, 0.5f) * 0.01f;
        //velocity += accel;

        //// Трение: чтобы скорость не разгонялась до бесконечности
        //velocity *= 0.98f;

        //// Притяжение к центру (мягкое)
        //velocity += (0.5f - prev) * 0.005f;

        //prev += velocity;

        //return prev = std::clamp(prev, v_min, v_max);

        bool isUp = Rand::rand_bool();
        float rem = isUp ? (v_max - prev) : (prev - v_min);
        float step = rem / 50.0f;

        if (isUp) prev += step;
        else      prev -= step;

        // Clamp обязателен, чтобы избежать микро-ошибок float на границах
        return prev = std::clamp(prev, v_min, v_max);
    }
};