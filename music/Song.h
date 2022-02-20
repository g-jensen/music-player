#pragma once

#include <string>
#include "SongState.h"
#include <SFML/Audio/Music.hpp>

typedef struct Song {
    sf::Music* music;
    std::string name;
} Song;