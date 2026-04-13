#include <cmath>
#include <numbers> // C++20 для std::numbers::pi

class SineGenerator {
    float phase = 0.0f;
    float phaseStep = 0.0f;
    float sampleRate = 48000.0f;

public:
    SineGenerator() {
        SetFrequency(440.0f);
    }
    // Установка частоты (например, 440.0f для ноты Ля)
    void SetFrequency(float frequency) {
        // Формула: (2 * PI * Freq) / SampleRate
        phaseStep = (2.0f * std::numbers::pi_v<float> *frequency) / sampleRate;
    }

    void SetSampleRate(float sr) {
        sampleRate = sr;
    }

    float Next() {
        float sample = std::sin(phase);

        phase += phaseStep;

        // Держим фазу в пределах 0...2PI, чтобы sin() не терял точность
        if (phase >= 2.0f * std::numbers::pi_v<float>) {
            phase -= 2.0f * std::numbers::pi_v<float>;
        }

        return sample;
    }
};
