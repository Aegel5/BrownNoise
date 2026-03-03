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
    Controller controller;

    while (true) {
        controller.Process();
        sf::sleep(sf::seconds(0.01));
    }

    return 0;
}

