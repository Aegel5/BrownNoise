enum SoundType {
    SND_Voss,
    SND_BrownStandart,
    SND_PinkStandart,
    SND_BrownRibbon
};
inline const char* SoundTypeName(SoundType t) {
    if (t == SND_Voss) return "SND_Voss";
    if (t == SND_BrownStandart) return "SND_BrownStandart";
    if (t == SND_PinkStandart) return "SND_PinkStandart";
    if (t == SND_BrownRibbon) return "SND_BrownRibbon";
    return "ERR";
}

struct _Settings {
    float volume = 5;
    SoundType last_mode = SND_Voss;
};

namespace nlohmann {
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
        _Settings,
        volume,
        last_mode
    )
}

namespace Settings {

    inline _Settings data;
    inline float init_vol = 0;

    inline void Save() {
        fs::path path = getExeDirectory() / "BrownNoise.json";
        std::ofstream outp(path, std::ios::binary);
        json js = data;
        outp << std::setw(2) << js << std::endl;
    }

    inline void Load() {
        fs::path path = getExeDirectory() / "BrownNoise.json";
        if (!std::filesystem::is_regular_file(path)) {
            return;
        }
        std::ifstream ifs(path, std::ios::binary);
        json js = json::parse(ifs, nullptr, true, true);
        js.get_to(data);

        init_vol = data.volume;
    }
}



