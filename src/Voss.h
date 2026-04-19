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
        float threshold = maxAmp * 0.95f;
        float absX = std::abs(x);

        if (absX <= threshold) return x;

        // Плавно скругляем пик, чтобы не выйти за maxAmp
        float limit = threshold + (maxAmp - threshold) * std::tanh((absX - threshold) / (maxAmp - threshold));
        return (x > 0) ? limit : -limit;
    }

public:
    // beta: 0 - белый, 1 - розовый, 2 - коричневый (бас)
    Voss(int numGens = 10, float beta = 2.0f)
        : engine(std::random_device{}()), dist(-1.0f, 1.0f) {
        float sumSqWeights = 0.0f;
        int lastperiod = 0;
        for (int i = 0; i < numGens; ++i) {
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

        // Статистическая нормализация (корень из суммы квадратов весов)
        // Коэффициент 2.0 дает плотный звук, который идеально ложится в лимитер
        //normFactor = std::sqrt(sumSqWeights) * 2.0f;
        normFactor = std::sqrt((float)numGens-3) * 2.5f; // 3 удаляем
    }

    float getNext() {
        float rawSum = 0.0f;

        for (int i = 3; i < gens.size(); i++) {
            auto& g = gens[i];
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
        float output = rawSum / normFactor;

        // 2. Масштабируем под ваше "окно" 0.5
        output *= maxAmp;

        // 3. Страхуем лимитером от редких случайных всплесков
        return softLimit(output);
    }
};