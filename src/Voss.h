class Voss {

    typedef struct {
        float current; // То, что слышим сейчас
        float target;  // То, к чему стремимся
    } SmoothLevel;

    float get_random_float() {
        return Rand::rand_real(-1.0f, 1.0f);
    }
public:

    float generate_smooth_voss() {
        static SmoothLevel stack[16] = { 0 };
        static uint32_t timer = 0;
        float final_sample = 0;

        timer++;

        for (int i = 0; i < 16; i++) {
            uint32_t period = (1 << i); // 1, 2, 4, 8, 16...

            // 1. Когда период закончился — выбираем новую цель
            if (timer % period == 0) {
                stack[i].current = stack[i].target;
                stack[i].target = get_random_float();
            }

            // 2. Считаем, где мы находимся внутри текущего периода (от 0.0 до 1.0)
            float fraction = (float)(timer % period) / (float)period;

            // 3. Линейно интерполируем (lerp)
            float interpolated = stack[i].current + (stack[i].target - stack[i].current) * fraction;

            // 4. Добавляем в общую сумму с весом для коричневого шума
            // Для коричневого спектра вес должен быть очень мощным на низах
            float weight = (float)(1 << i);
            final_sample += interpolated * weight;
        }

        // Нормализуем сумму всех весов (для 16 уровней сумма степеней двойки ~65535)
        return final_sample / 65535.0f * 100;
    }
};