struct _Settings {
    float volume = 5;
    int last_mode = 0;
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

    inline void Save() {
        fs::path path = fs::current_path() / "BrownNoise.json";
        std::ofstream outp(path, std::ios::binary);
        json js = data;
        outp << std::setw(2) << js << std::endl;
    }

    inline void Load() {
        fs::path path = fs::current_path() / "BrownNoise.json";
        if (!std::filesystem::is_regular_file(path)) {
            return;
        }
        std::ifstream ifs(path, std::ios::binary);
        json js = json::parse(ifs, nullptr, true, true);
        js.get_to(data);
    }
}



