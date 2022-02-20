#pragma once

#include "Song.h"
#include <filesystem>

enum class Where {
    NEXT,
    PREV
};

class Playlist {
public:
    std::string name;
    std::vector<Song> songs;
    static void refreshSongs(std::vector<Playlist>& playlists);
    static Song getSong(std::vector<Playlist>& playlists, Where w, SongState &songState);
};