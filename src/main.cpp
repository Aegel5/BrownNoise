#include <iostream>
#include <map>
#include <random>
#include <cstdint>
#include <vector>
#include <memory>
#include <cmath>
#include <numbers>

#include <simple_enum/simple_enum.hpp>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

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