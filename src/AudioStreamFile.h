class AudioStreamFile : public sf::SoundStream {

    sf::SoundBuffer buff;
    bool first = false;

private:
    virtual bool onGetData(Chunk& data) override {
        if (first) return false;
        data.sampleCount = buff.getSampleCount();
        data.samples = buff.getSamples();
        first = true;
        return true;
    }

    virtual void onSeek(sf::Time timeOffset) override { return; }
public:
    bool Load(std::string s) {
        if (buff.loadFromFile(s)) {
            initialize(buff.getChannelCount(), buff.getSampleRate(), buff.getChannelMap());
            first = false;
            return true;
        }
        return false;
    }
    const auto* GetSamples() {
        return buff.getSamples();
    }
    int GetSamplesCount() {
        return buff.getSampleCount();
    }
    virtual ~AudioStreamFile() {}
};