#include <vector>
#include <random>

class PowerLawNoise {
public:
    /**
     * @param alpha Показатель степени (1.0 - розовый, 2.0 - коричневый)
     * @param order Глубина фильтра (чем выше, тем точнее НЧ, обычно 64-128)
     */
    PowerLawNoise(float alpha, int order = 64)
        : m_order(order), m_historyIndex(0), m_dist(-1.0f, 1.0f) {
        m_coeffs.resize(order);
        m_history.assign(order, 0.0f);

        // Предварительный расчет коэффициентов (разложение в ряд)
        float a = alpha / 2.0f;
        m_coeffs[0] = -a;
        for (int i = 1; i < order; i++) {
            m_coeffs[i] = m_coeffs[i - 1] * (i - a) / (i + 1);
        }
    }

    float nextSample() {
        // 1. Генерация белого шума
        float white = m_dist(m_rng);

        // 2. AR-фильтрация (кольцевой буфер)
        float out = white;
        for (int i = 0; i < m_order; i++) {
            int idx = (m_historyIndex - i + m_order) % m_order;
            out -= m_coeffs[i] * m_history[idx];
        }

        // 3. Обновление состояния
        m_historyIndex = (m_historyIndex + 1) % m_order;
        m_history[m_historyIndex] = out;

        // Небольшой гейн, так как фильтр сильно задирает амплитуду
        return out * 0.1f;
    }

private:
    int m_order;
    int m_historyIndex;
    std::vector<float> m_coeffs;
    std::vector<float> m_history;

    // Генератор случайных чисел
    std::mt19937 m_rng{ std::random_device{}() };
    std::uniform_real_distribution<float> m_dist;
};