#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include <thread>
#include <windows.h>

#include "Playlist.h"

namespace Globals {
	std::vector<Playlist> playlists;
	size_t playlist_index;
	size_t song_index;

	sf::Music* currentSong = new sf::Music();
	std::string currentSongName = "None";
    std::string currentPath = std::filesystem::current_path().string();

	bool loop = false;
	bool loopSong = false;
	bool pause = false;
	bool hasPlayed = false;

	bool createPlaylist = false;
	bool addSong = false;
	int volume = 50;
	bool checkSongEvent = false;

    std::thread ytdl_thread;
    std::thread song_event_thread;

    char* yt_link = (char*)malloc(sizeof(char) * 128);
    char* playlist_input = (char*)malloc(sizeof(char) * 32);
    
    HHOOK _k_hook;

	void ResourceCleanUp() {
        ImGui::SFML::Shutdown();

        for (Playlist& p : playlists) {
            for (Song s : p.songs) {
                delete s.music;
            }
        }

        if (song_event_thread.joinable()) {
            song_event_thread.join();
        }
        if (ytdl_thread.joinable()) {
            ytdl_thread.join();
        }

        delete currentSong;
        free(playlist_input);
        free(yt_link);

        if (_k_hook) {
            UnhookWindowsHookEx(_k_hook);
        }
	}
}