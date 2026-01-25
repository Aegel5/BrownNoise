


namespace Rand {
    std::mt19937 m_rng{ std::random_device{}() };        // Быстрый генератор PRNG
    auto rand_real(auto a, auto b) {
        std::uniform_real_distribution<decltype(a)> d( a, b );
        return d(m_rng);
    }
    auto rand_i(auto a, auto b) {
        std::uniform_int_distribution<decltype(a)> d(a, b);
        return d(m_rng);
    }
    bool rand_bool() {
        return m_rng() & 1;
    }
}


class AudioStreamAutoGen : public sf::SoundStream {

    static const int v_min = -32700;
    static const int v_max = 32700;
    static const int smpl_per_sec = 44100;

public:
    enum class Mode { Red1, Red2, Red3, Red4, Red5, Test_La, White };
private:

    Mode mode = Mode::Red1;
    std::vector<int16_t> samples;
    int prev = 0;

    inline static const double m_frequency = 440.0;      // Частота ноты Ля
    inline static const double phaseStep = (2.0 * std::numbers::pi * m_frequency) / smpl_per_sec;
    double m_phase = 0;              // Текущая фаза (сохраняется между вызовами)

    // Состояния для интерполяции
    float m_target = 0.0f;      // Куда движемся
    float m_previous = 0.0f;    // Откуда вышли
    int m_stepCount = 0;        // Сколько семплов прошло
    const int m_stepsPerChange = 180; // И

    double m_lastOutput = 0;

    float smoothStep(float t) {
        return t * t * (3.0f - 2.0f * t);
    }

public:

    int Gen() {
        if (mode == Mode::Red1) {
            bool isUp = Rand::rand_bool();

            auto curW = std::max(v_max - prev, prev - v_min);
            auto rem = isUp ? v_max - prev : prev - v_min;

            auto cur = std::round(rem / 50.0);

            if (isUp) prev += cur;
            else prev -= cur;

            return prev = std::clamp(prev, v_min, v_max);
        }

        if (mode == Mode::Red2) {

            auto cur = v_max / 8.0;

            if (Rand::rand_bool()) cur = -cur;
            if (abs(prev + cur) > v_max) cur = -cur;

            if ((prev > 0 && cur < 0) || (prev < 0 && cur > 0)) { auto k = 1 + (0.5 * (double)abs(prev) / v_max); cur *= k; }

            prev += std::round(cur);
            return prev = std::clamp(prev, v_min, v_max);
        }

        if (mode == Mode::Test_La) {
            // Формула шага фазы: 2 * PI * частота / частота_дискретизации

           // Генерируем значение от -32767 до 32767
            auto res = std::round(30000 * std::sin(m_phase));

            m_phase += phaseStep;

            // Чтобы фаза не росла бесконечно (хотя для double это не критично)
            if (m_phase > 2.0 * std::numbers::pi)
                m_phase -= 2.0 * std::numbers::pi;

            return res;
        }

        if (mode == Mode::White) {
            return Rand::rand_i(v_min, v_max);
        }

        if (mode == Mode::Red3) {
            // Если дошли до цели, выбираем новую
            if (m_stepCount >= m_stepsPerChange) {
                m_previous = m_target;
                m_target = Rand::rand_real((float)v_min, (float)v_max);
                m_stepCount = 0;
            }

            // Вычисляем прогресс (от 0.0 до 1.0)
            float t = static_cast<float>(m_stepCount) / m_stepsPerChange;

            // Плавная интерполяция между точками
            float val = m_previous + (m_target - m_previous) * smoothStep(t);

            m_stepCount++;
            return std::round(val);
        }

        if (mode == Mode::Red4) {

            // мягкий шум - автовозврат через тангенс к 0.

            m_lastOutput = (0.997 * m_lastOutput) + (0.02 * Rand::rand_real(-1.0, 1.0));
            m_lastOutput = std::tanh(m_lastOutput);

            auto res = std::round(m_lastOutput * v_max*2);
            return res;
        }

        if (mode == Mode::Red5) {
        }

        return 0;
    }
private:
    virtual bool onGetData(Chunk& data) override {
        for (auto& smpl : samples) {
            smpl = Gen();
        }
        data.sampleCount = samples.size();
        data.samples = &samples[0];
        return true;
    }

    virtual void onSeek(sf::Time timeOffset) override { return; }
public:
    const auto* GetSamples() {
        return &samples[0];
    }
    int GetSamplesCount() {
        return samples.size();
    }
    void SetMode(Mode mode) { this->mode = mode; }
    Mode GetMode() { return mode; }
    AudioStreamAutoGen() { 
        samples.resize(5000);
        initialize(1, smpl_per_sec, { sf::SoundChannel::Mono });
    }

    virtual ~AudioStreamAutoGen() override = default;
};