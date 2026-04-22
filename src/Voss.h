#include <vector>
#include <random>
#include <cmath>
#include <algorithm>

class Voss {
private:
    struct Generator {
        float currentVal;
        float targetVal;
        int period;
        int timer;
        float step;
        float weight;
    };

    std::vector<Generator> gens;
    std::mt19937 engine;
    std::uniform_real_distribution<float> dist;

    float normFactor;
    const float maxAmp = 0.5f; // Ваше "окно" громкости

    float nextRandom() { return dist(engine); }

    // Тот самый soft limiter: до 90% от maxAmp звук линеен, выше — плавно поджимается
    float softLimit(float x) {
        const float threshold = 0.95f;
        const float margin = 1.0f - threshold; // 0.05

        float absX = std::abs(x);

        if (absX <= threshold) return x;

        // Считаем лимит: к порогу добавляем сжатый остаток
        float soft = threshold + margin * std::tanh((absX - threshold) / margin);

        return (x > 0) ? soft : -soft;
    }

public:
    int skip = 3;
    // beta: 0 - белый, 1 - розовый, 2 - коричневый (бас)
    Voss(int numGens = 15, float beta = 2.0f)
        : engine(std::random_device{}()), dist(-1.0f, 1.0f) {
        float sumSqWeights = 0.0f;
        int lastperiod = 0;
        for (int i = 3; i < numGens; ++i) {
            Generator g;
            g.period = (int)std::round(std::pow(1.6f, (float)i));
            if (g.period <= lastperiod) g.period = lastperiod + 1;
            lastperiod = g.period;
            g.timer = 0;
            g.currentVal = nextRandom();
            g.targetVal = nextRandom();
            g.step = (g.targetVal - g.currentVal) / (float)g.period;
            g.weight = 1;

            // Расчет веса для спектра
            //float freq = 1.0f / (float)g.period;
            //g.weight = std::pow(g.period, (beta - 1.0f) * 0.5f);
            //sumSqWeights += g.weight * g.weight;

            gens.push_back(g);
        }

        // Статистическая нормализация (3 сигмы = 1.73 * sqrt(n). Возьмем с запасом + softLimit для страховки)
        auto cnt = (float)numGens - skip;

        normFactor = 1.0f / std::min(std::sqrt(cnt) * 2.4f, cnt);
    }

    float getNext() {
        float rawSum = 0.0f;

        for (auto& g: gens) {
            g.currentVal += g.step;
            rawSum +=   g.currentVal * g.weight;
            if (++g.timer >= g.period) {
                g.timer = 0;
                g.currentVal = g.targetVal;
                g.targetVal = nextRandom();
                g.step = (g.targetVal - g.currentVal) / (float)g.period;
            }
        }

        // 1. Приводим к базовой амплитуде
        float output = rawSum * normFactor;

        // 3. Страхуем лимитером от редких случайных всплесков
        output = softLimit(output);

        // 2. Масштабируем под ваше "окно" 0.5
        return output * maxAmp;
    }
};