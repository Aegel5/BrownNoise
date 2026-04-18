class RealTimeStats {
private:
    int sampleRate;
    int numBins;
    float range;

    std::vector<int> currentBins;
    std::vector<int> stableBins;

    // Новые метрики
    double currentSumSq = 0;   // Для RMS
    float currentMax = 0;      // Для пикового значения
    float stableRMS = 0;
    float stableMax = 0;

    int samplesCollected = 0;

public:
    RealTimeStats(int rate = 44100, int bins = 31, float maxRange = 0.5f)
        : sampleRate(rate), numBins(bins), range(maxRange) {
        currentBins.assign(numBins, 0);
        stableBins.assign(numBins, 0);
    }

    void addSample(float s) {
        // 1. Гистограмма
        int binIdx = static_cast<int>((s + range) / (2.0f * range) * numBins);
        if (binIdx >= 0 && binIdx < numBins) currentBins[binIdx]++;

        // 2. Метрики (энергия и пик)
        currentSumSq += (double)s * s;
        if (std::abs(s) > currentMax) currentMax = std::abs(s);

        samplesCollected++;

        if (samplesCollected >= sampleRate) {

            stableBins = std::move(currentBins);
            stableRMS = std::sqrt(currentSumSq / sampleRate);
            stableMax = currentMax;

            // Сброс
            currentBins.clear();
            currentSumSq = 0;
            currentMax = 0;
            samplesCollected = 0;
        }
    }

    void draw() const {
        int maxCount = 0;
        for (int count : stableBins) if (count > maxCount) maxCount = count;
        if (maxCount == 0) return;

        float db = 20.0f * std::log10(stableRMS + 1e-9f);

        //std::cout << "\x1B[2J\x1B[H"; // Очистка экрана
        std::cout << "--- Статистика за секунду ---\n";
        printf("RMS (Громкость): %.4f | Peak (Пик): %.4f | Loudness: %.2f\n\n", stableRMS, stableMax, db);

        for (int i = 0; i < numBins; ++i) {
            float val = -range + (i + 0.5f) * (2.0f * range) / numBins;
            printf("%5.2f | ", val);
            int barWidth = (stableBins[i] * 50) / maxCount;
            std::cout << std::string(barWidth, '#') << "\n";
        }
    }
};