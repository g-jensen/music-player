#pragma once

#include <string>
#include <SFML/Audio/Music.hpp>

typedef struct Song {
    sf::Music* music;
    std::string name;
} Song;