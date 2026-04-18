#include "miniaudio.h"
#include <vector>
#include <atomic>
#include "Red1.h"
#include "Voss.h"
#include "SinGenerate.h"

class Player {
    static constexpr uint32_t CHUNK_SIZE = 1024; 
    static constexpr size_t TOTAL_CHUNKS = 256;

    ma_device m_device;
    std::vector<std::vector<float>> m_chunks;
    std::atomic<int64_t> m_r{ 0 };
    int64_t m_w = 0;
    std::atomic<float> m_volume = 0;
    float m_last_volume = 0;
    Red1 red1[2];
    SineGenerator sin;
    int m_channels;
    int channels() const { return m_channels; }
    ma_hpf hpf; 
    ma_noise noise;
    Voss voss[2];

    ma_gainer m_gainer; // проходим через барьер памяти, синхронизации не требуется.
public:
    void start() {
        step(); // забиваем буфер
        ma_device_start(&m_device);
    }
    Player() {

        // device
        {
            auto config = ma_device_config_init(ma_device_type_playback);

            config.playback.format = ma_format_f32;
            //config.playback.channels = 1;

            config.periodSizeInFrames = CHUNK_SIZE;
            config.periods = 8;

            config.dataCallback = [](ma_device* pD, void* pOut, const void*, ma_uint32 fCount) {

                auto* self = (Player*)pD->pUserData;
                float* out = (float*)pOut;

                size_t r = self->m_r.load(std::memory_order_relaxed);
                const float* chunkData = self->m_chunks[r % TOTAL_CHUNKS].data();

                // громоксть - самая частая операция, делаем прямо в audio-потоке, чтобы не перегенеривать весь буфер.
                {
                    auto vol = self->m_volume.load(std::memory_order_relaxed);
                    if (vol != self->m_last_volume) {
                        self->m_last_volume = vol;
                        ma_gainer_set_gain(&self->m_gainer, vol);
                    }
                    ma_gainer_process_pcm_frames(&self->m_gainer, pOut, chunkData, CHUNK_SIZE);
                }

                //memcpy(pOut, chunkData, fCount * sizeof(float) * self->channels());

                self->m_r.store(r + 1, std::memory_order_relaxed);

                };
            config.pUserData = this;

            ma_device_init(NULL, &config, &m_device);
            m_channels = m_device.playback.channels;
        }

        m_chunks.resize(TOTAL_CHUNKS, std::vector<float>(CHUNK_SIZE*channels()));

        // Фильтр низких частот.
        {
            auto cfg = ma_hpf_config_init(ma_format_f32, channels(), m_device.sampleRate, 50, 2); // order 1 может не хватить для борьбы с залипанием мембраны.
            if (ma_hpf_init(&cfg, 0, &hpf)) {
                std::terminate();
            }
        }

        // Стандартный генератор
        {
            auto cfg = ma_noise_config_init(ma_format_f32, channels(), ma_noise_type_brownian, 0, 0.5);
            ma_noise_init(&cfg, 0, &noise);
        }

        // Плавная громкость.
        {
            auto cfg = ma_gainer_config_init(channels(), m_device.sampleRate/4);
            ma_gainer_init(&cfg, NULL, &m_gainer);
            ma_gainer_set_master_volume(&m_gainer, 1.0f);
            ma_gainer_set_gain(&m_gainer, 0.0f);
        }

    }

    ~Player() { 
        ma_device_uninit(&m_device); 
    }

    void set_volume(float vol) { 

        m_volume.store(vol, std::memory_order_relaxed);
        //m_w = m_r;
        //ma_gainer_set_gain(&gainer, vol);
    }


    void step() {
        auto ch = channels();
        while (m_w - m_r.load(std::memory_order_relaxed) < TOTAL_CHUNKS) {
            auto& chunk = m_chunks[m_w % TOTAL_CHUNKS];

            // стандартный генератор
            ma_noise_read_pcm_frames(&noise, &chunk[0], CHUNK_SIZE, 0);

            //for (int64_t i = 0; i < CHUNK_SIZE; i++) {
            //    auto left = red1[0].Next();
            //    auto right = red1[1].Next();

            //    //auto left = voss[0].generate_smooth_voss() * m_volume;
            //    //auto right = voss[1].generate_smooth_voss() * m_volume;

            //    if (ch == 6) { // Спец-обработка для 5.1
            //        chunk[i * ch + 0] = left;              // Front L
            //        chunk[i * ch + 1] = right;             // Front R
            //        chunk[i * ch + 2] = (left + right) * 0.5f; // Center
            //        chunk[i * ch + 3] = (left + right) * 0.5f; // LFE (Саб)
            //        chunk[i * ch + 4] = left;              // Surround L
            //        chunk[i * ch + 5] = right;             // Surround R
            //    }
            //    else {
            //        for (int64_t j = 0; j < ch; j++) {
            //            chunk[i * ch + j] = (j & 1) ? right : left;
            //        }
            //    }
            //}

            // накладываем ФВЧ: централизуем график возле 0 (нормализуем мембрану), а также убираем весь неслышимый инфра-мусор (звук меньше 10 ГЦ).
            ma_hpf_process_pcm_frames(&hpf, &chunk[0], &chunk[0], CHUNK_SIZE);

            //ma_gainer_process_pcm_frames(&gainer, &chunk[0], &chunk[0], CHUNK_SIZE);

#ifdef _DEBUG
            for (int64_t i = 0; i < ssize(chunk); i++) {
                if (chunk[i] > 1.0 || chunk[i] < -1.0) {
                    std::terminate();
                }
            }
#endif
            

            m_w++;
        }
    }
};