
using int16 = std::int16_t;
static const auto v_min = std::numeric_limits<int16>::min();
static const auto v_max = std::numeric_limits<int16>::max();




namespace Rand {

    std::mt19937 m_rng{ std::random_device{}() };        // Быстрый генератор PRNG

    float rand_float(float a, float b) {
        std::uniform_real_distribution<float> d{ a, b };
        return d(m_rng);
    }

    int16 rand16_s() { std::uniform_int_distribution<int> d(v_min, v_max); return d(m_rng); }
    int16 rand_int(int a, int b) { std::uniform_int_distribution<int> d(a, b); return d(m_rng); }
}


class AudioStreamAutoGen : public sf::SoundStream {
public:
    enum class Mode { Red1, Red2, Red3, Red4, Red5, Test_La, White };
private:
    static const int smpl_per_sec = 44100;

    Mode mode = Mode::Red1;
    std::vector<int16> samples;
    int prev = 0;

    inline static const double m_frequency = 440.0;      // Частота ноты Ля
    inline static const double phaseStep = (2.0 * std::numbers::pi * m_frequency) / smpl_per_sec;
    double m_phase = 0;              // Текущая фаза (сохраняется между вызовами)

    // Состояния для интерполяции
    float m_target = 0.0f;      // Куда движемся
    float m_previous = 0.0f;    // Откуда вышли
    int m_stepCount = 0;        // Сколько семплов прошло
    const int m_stepsPerChange = 180; // И

    float m_lastOutput = 0;
    float m_vibratingValue = 0.0f; // Второе состояние для инерции

    float smoothStep(float t) {
        return t * t * (3.0f - 2.0f * t);
    }

public:

    void Gen(int16& smpl) {
        if (mode == Mode::Red1) {
            bool isUp = Rand::rand_int(0, 1) == 0;

            auto curW = std::max(v_max - prev, prev - v_min);
            auto rem = isUp ? v_max - prev : prev - v_min;

            auto cur = std::round(2.0 * rem / 100);

            if (isUp) prev += cur;
            else prev -= cur;
            smpl = prev = std::clamp(prev, (int)v_min, (int)v_max);
            return;
        }

        if (mode == Mode::Red2) {

            //auto cur = (double)rand16_s() / 10;
            auto cur = (double)v_max / 8;

            if (Rand::rand_int(0, 1) == 0) cur = -cur;
            if (abs(prev + cur) > v_max) cur = -cur;
            //if ((prev == v_min && cur < 0) || (prev == v_max && cur > 0)) { cur = -cur; }

            if ((prev > 0 && cur < 0) || (prev < 0 && cur > 0)) { auto k = 1 + (0.5 * (double)abs(prev) / v_max); cur *= k; }

            prev += std::round(cur);
            smpl = prev = std::clamp(prev, (int)v_min, (int)v_max);
            return;
        }

        if (mode == Mode::Test_La) {
            // Формула шага фазы: 2 * PI * частота / частота_дискретизации

           // Генерируем значение от -32767 до 32767
            smpl = static_cast<int16>(30000 * std::sin(m_phase));

            m_phase += phaseStep;

            // Чтобы фаза не росла бесконечно (хотя для double это не критично)
            if (m_phase > 2.0 * std::numbers::pi)
                m_phase -= 2.0 * std::numbers::pi;

            return;
        }

        if (mode == Mode::White) {
            smpl = Rand::rand16_s();
            return;
        }

        if (mode == Mode::Red3) {
            // Если дошли до цели, выбираем новую
            if (m_stepCount >= m_stepsPerChange) {
                m_previous = m_target;
                m_target = Rand::rand_float(-30000, 30000);
                m_stepCount = 0;
            }

            // Вычисляем прогресс (от 0.0 до 1.0)
            float t = static_cast<float>(m_stepCount) / m_stepsPerChange;

            // Плавная интерполяция между точками
            float val = m_previous + (m_target - m_previous) * smoothStep(t);

            smpl = static_cast<int16>(std::clamp(val, -32000.0f, 32000.0f));
            m_stepCount++;
            return;
        }

        if (mode == Mode::Red4) {
            // 1. Берем чистый белый шум [-1, 1]
            float white = Rand::rand_float(-1, 1);

            // 2. Алгоритм интегратора для Brown Noise:
            // m_lastOutput = (предыдущее * инерция) + (белый_шум * шаг)
            // Коэффициент 0.995f убирает ВЧ-"песок", делая звук глухим.
            // Коэффициент 0.01f делает блуждание очень плавным.
            m_lastOutput = (m_lastOutput * 0.995f) + (white * 0.01f);

            // 3. Усиливаем до уровня Int16 (32767)
            // Коричневый шум имеет низкую амплитуду из-за фильтрации, 
            // поэтому множитель здесь должен быть высоким (около 150-200к).
            float out = m_lastOutput * 180000.0f;

            // 4. Ограничиваем и сохраняем
            smpl = static_cast<int16>(std::clamp(out, -32000.0f, 32000.0f));

            return;
        }

        if (mode == Mode::Red5) {


            return;
        }
    }
private:
    virtual bool onGetData(Chunk& data) override {
        for (auto& smpl : samples) {
            Gen(smpl);
        }
        data.sampleCount = samples.size();
        data.samples = &samples[0];
        return true;
    }

    virtual void onSeek(sf::Time timeOffset) override { return; }
public:
    const int16* GetSamples() {
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