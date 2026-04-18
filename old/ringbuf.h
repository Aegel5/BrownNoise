config.dataCallback = [](ma_device* pD, void* pOut_, const void*, ma_uint32 fCount) {

    auto* self = (Player*)pD->pUserData;
    uint8_t* out = (uint8_t*)pOut_;

    // громоксть - самая частая операция, делаем прямо в audio-потоке, чтобы не перегенеривать весь буфер.
    {
        auto vol = self->m_volume.load(std::memory_order_relaxed);
        if (vol != self->m_last_volume) {
            self->m_last_volume = vol;
            ma_gainer_set_gain(&self->m_gainer, vol);
        }
    }

    while (fCount > 0) {
        auto to_read = fCount;
        void* input;
        auto res = ma_pcm_rb_acquire_read(&self->rb, &to_read, &input);
        if (res != MA_SUCCESS || to_read == 0) break;
        ma_gainer_process_pcm_frames(&self->m_gainer, out, input, to_read);
        out += to_read;
        fCount -= to_read;
        ma_pcm_rb_commit_read(&self->rb, to_read);
    }

    if (fCount > 0) {
        // underflow
        memset(out, 0, fCount * self->bytes_per_frame());
    }

    };