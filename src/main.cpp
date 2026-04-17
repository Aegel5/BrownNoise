#include <iostream>
#include <map>
#include <random>
#include <cstdint>
#include <vector>
#include <memory>
#include <cmath>
#include <numbers>
#include <filesystem>
#include <fstream>
#include <format>
#include <future>
#include <charconv> // std::from_chars для float

namespace fs = std::filesystem;

#include <simple_enum/simple_enum.hpp>

#include "json.hpp"
using json = nlohmann::json;

#include "GetDir.h"
#include "Settings.h"
#include "Rand.h"
#include "Player.h"


int main(int argc, char* argv[])
{
    setlocale(LC_ALL, ".utf8");

    Settings::Load();

    // Лямбда-функция прямо внутри main для создания задачи чтения
    auto get_input = []() {
        return std::async(std::launch::async, []() {
            std::string s;
            return std::getline(std::cin, s) ? s : std::string("exit");
            });
        };

    std::cout << std::format("Brown Noise Version 1.5\n");

    Player player;

    auto set_volume = [&](float f) {
        f = std::clamp(f, 0.0f, 200.0f);
        std::cout << std::format("Volume: {}\n", f);
        player.set_volume(f / 100);
        if (f != Settings::data.volume) {
            Settings::data.volume = f;
            Settings::Save();
        }
        };

    set_volume(Settings::data.volume);
    player.start();

    std::cout << "Введите команду: число для установки Volume или exit для выхода" << std::endl;


    // Инициализируем первое ожидание
    auto future_input = get_input();

    while (true) {
        // Проверяем, готов ли результат (ждем 0 миллисекунд)
        if (future_input.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            std::string result = future_input.get();

            if (result == "exit") {
                break;
            }

            // Если хотим читать дальше, нужно перезапустить future
            future_input = get_input();

            float val = 0;
            // Пытаемся преобразовать строку в число
            auto [ptr, ec] = std::from_chars(result.data(), result.data() + result.size(), val);

            // Если ошибок нет (ec == success) и мы дошли до конца строки (ptr == end)
            if (ec == std::errc() && ptr == result.data() + result.size()) {
                set_volume(val);
                continue;
            }

        }

        player.step();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}

