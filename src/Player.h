#include "miniaudio.h"
#include <vector>
#include <atomic>
#include "Red1.h"
#include "SinGenerate.h"

class Player {
    static constexpr uint32_t CHUNK_SIZE = 1024; 
    static constexpr size_t TOTAL_CHUNKS = 256;

    ma_device m_device;
    std::vector<std::vector<float>> m_chunks;
    std::atomic<int64_t> m_r{ 0 };
    int64_t m_w = 0;
    float m_volume { 0.1f };
    Red1 red1;
    SineGenerator sin;

    int channels() const { return m_device.playback.channels; }

public:
    void start() {


        step(); // забиваем буфер
        ma_device_start(&m_device);
    }
    Player() {
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
            memcpy(pOut, chunkData, fCount * sizeof(float) * self->channels());
            self->m_r.store(r + 1, std::memory_order_relaxed);

            };
        config.pUserData = this;

        ma_device_init(NULL, &config, &m_device);

        m_chunks.resize(TOTAL_CHUNKS, std::vector<float>(CHUNK_SIZE*channels()));

    }

    ~Player() { ma_device_uninit(&m_device); }

    void set_volume(float vol) { 

        m_volume = vol; 
        m_w = m_r;
    }


    void step() {
        auto ch = channels();
        while (m_w - m_r.load(std::memory_order_relaxed) < TOTAL_CHUNKS) {
            auto& chunk = m_chunks[m_w % TOTAL_CHUNKS];
            for (int64_t i = 0; i < CHUNK_SIZE; i++) {
                auto cur = red1.Next() * m_volume;
                for (int64_t j = 0; j < ch; j++) {
                    chunk[i * ch + j] = cur;
                }
            }

            m_w++;
        }
    }
};