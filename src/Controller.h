class Controller {

    struct PlayEntry {
        AudioStreamAutoGen::Mode mode;
        std::string file;
        bool isFile() { return !file.empty(); };
        std::string name() {
            if (isFile()) return name();
            return (std::string)simple_enum::enum_name(mode);
        }
    };

    std::vector<PlayEntry> entries{ 
        {AudioStreamAutoGen::Mode::Red1}, 
        {AudioStreamAutoGen::Mode::Red2}, 
        {AudioStreamAutoGen::Mode::Red3},
        {AudioStreamAutoGen::Mode::Red4},
    };
    int iEntry = -1;

    std::unique_ptr<sf::SoundStream> stream;

    bool keys[sf::Keyboard::KeyCount] = { false };

    void NextTrack(bool forward = true) {

        auto vol = 5;

        if (stream) {
            vol = stream->getVolume();
            stream->stop();
        }

        if (forward) {
            iEntry++;
            if (iEntry >= entries.size()) iEntry = 0;
        }
        else {
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

        stream->setVolume(vol);
        stream->play();


        std::cout << "--------- Start play " << cur.name() << "\n";

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

public:

    void Process() {
        int step = 1;

        if (was_down(sf::Keyboard::Key::Up)) {
            stream->setVolume(std::min(100.0f, std::round(stream->getVolume() + step)));
            std::cout << "Set volume " << stream->getVolume() << "\n";
        }

        else if (was_down(sf::Keyboard::Key::Down)) {
            stream->setVolume(std::round(std::max(0.0f, stream->getVolume() - step)));
            std::cout << "Set volume " << stream->getVolume() << "\n";
        }

        else if (was_down(sf::Keyboard::Key::Right)) {
            NextTrack();
        }

        else if (was_down(sf::Keyboard::Key::Left)) {
            NextTrack(false);
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
        NextTrack();
    }
};