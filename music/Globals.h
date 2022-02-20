#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include <thread>
#include <windows.h>
#include <vector>
#include <string>

#include "Playlist.h"

class G {
public:
    static std::vector<Playlist> playlists;
    static SongState songState;

    static sf::Music* currentSong;
    static std::string currentSongName;
    static std::string currentPath;

    static bool pause;
    static bool hasPlayed;

    static bool createPlaylist;
    static bool addSong;
    static int volume;
    static bool checkSongEvent;

    static std::thread ytdl_thread;
    static std::thread song_event_thread;

    static char* yt_link;
    static char* playlist_input;

    static HHOOK _k_hook;

    static void ResourceCleanUp();
    static void UpdateCurrentSong();
    
    // handle weird key presses
    static LRESULT __stdcall k_Callback1(int nCode, WPARAM wParam, LPARAM lParam);
};