class Controller {

    struct PlayEntry {
        AudioStreamAutoGen::Mode mode;
        std::string file;
        bool isFile() { return !file.empty(); };
        std::string name() {
            if (isFile()) return file;
            return (std::string)simple_enum::enum_name(mode);
        }
    };

    std::vector<PlayEntry> entries{ 
        {AudioStreamAutoGen::Mode::Red1}, 
        {AudioStreamAutoGen::Mode::Red2}, 
        {AudioStreamAutoGen::Mode::Red3},
        {AudioStreamAutoGen::Mode::Red4},
    };

    std::unique_ptr<sf::SoundStream> stream;

    bool keys[sf::Keyboard::KeyCount] = { false };

    void NextTrack(int forward = 0) {

        if (stream) {
            stream->stop();
        }

        int& iEntry = Settings::data.last_mode;

        if (forward>0) {
            iEntry++;
            if (iEntry >= entries.size()) iEntry = 0;
        }
        if(forward<0){
            iEntry--;
            if (iEntry < 0) iEntry = entries.size() - 1;
        }

        auto& cur = entries[iEntry];

        if (cur.isFile()) {
        }
        else {
            auto s = std::make_unique<AudioStreamAutoGen>();
            s->SetMode(cur.mode);
            stream = std::move(s);

        }

        stream->setVolume(Settings::data.volume);
        stream->play();

        Settings::Save();
        std::cout << std::format("Now playing: {}. Volume: {}\n", cur.name(), stream->getVolume());

    }
    bool was_changes(sf::Keyboard::Key k) {
        auto cur = sf::Keyboard::isKeyPressed(k);
        if (keys[(int)k] != cur) {
            keys[(int)k] = cur;
            return true;
        }
        return false;
    }
    bool was_down(sf::Keyboard::Key k) {
        if (!was_changes(k))
            return false;
        return keys[(int)k] && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F1);
    }

    void set_vol(float delt) {
        if (stream->getVolume() <= 5) {
            delt /= 2;
        }
        auto v = stream->getVolume() + delt;
        auto v2 = std::round(v);
        if (std::abs(v - v2) <= 0.001f) v = v2;
        if (v > 5) v = std::round(v + 0.1f);
        SetVolume(v);
    }

public:

    void SetVolume(float v) {
        v = std::clamp(v, 0.0f, 100.0f);
        if (v == stream->getVolume()) return;
        stream->setVolume(v);
        std::cout << "Set volume " << stream->getVolume() << "\n";
        Settings::data.volume = stream->getVolume();
        Settings::Save();
    }

    void Process() {
        int step = 1;

        if (was_down(sf::Keyboard::Key::Up)) {
            set_vol(1);
        }

        else if (was_down(sf::Keyboard::Key::Down)) {
            set_vol(-1);
        }

        else if (was_down(sf::Keyboard::Key::Right)) {
            NextTrack(1);
        }

        else if (was_down(sf::Keyboard::Key::Left)) {
            NextTrack(-1);
        }

        else if (was_down(sf::Keyboard::Key::Space) || was_down(sf::Keyboard::Key::Enter)) {
            if (stream->getStatus() == sf::SoundSource::Status::Playing) {
                std::cout << "Stop" << "\n";
                stream->stop();
            }
            else {
                std::cout << "Resume" << "\n";
                stream->play();
            }
        }
    }

    Controller() {
        Settings::Load();
        NextTrack();
    }
};