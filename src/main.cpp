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

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "GetDir.h"
#include "Settings.h"
#include "AudioStreamAutoGen.h"
#include "AudioStreamFile.h"
#include "Controller.h"

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, ".utf8");

    // Лямбда-функция прямо внутри main для создания задачи чтения
    auto get_input = []() {
        return std::async(std::launch::async, []() {
            std::string s;
            return std::getline(std::cin, s) ? s : std::string("exit");
            });
        };


    Controller controller;

    std::cout << "Введите команду: число для установки Volume или exit\n";

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
                controller.SetVolume(val);
                continue;
            }

        }

        controller.Process();
        sf::sleep(sf::seconds(0.02));
    }

    return 0;
}

