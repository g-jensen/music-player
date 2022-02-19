#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <thread>

#include <windows.h>

enum class Where {
    NEXT,
    PREV
};

typedef struct Song {
    sf::Music* music;
    std::string name;
} Song;

typedef struct Playlist {
    std::string name;
    std::vector<Song> songs;
} Playlist;

std::vector<Playlist> playlists;
size_t playlist_index;
size_t song_index;

sf::Music* currentSong;
std::string currentSongName = "None";

bool loop = false;
bool loopSong = false;
bool pause = false;
bool hasPlayed = false;

Song getSong(Where w) {
    if (loopSong) { return playlists[playlist_index].songs[song_index]; }
    if (w == Where::NEXT) {
        if (song_index >= playlists[playlist_index].songs.size() - 1) {
            if (loop) {
                song_index = 0;
            }
        }
        else {
            song_index++;
        }
    }
    else {
        if (song_index <= 0) {
            if (loop) {
                song_index = playlists[playlist_index].songs.size() - 1;
            }
        }
        else {
            song_index--;
        }
    }
    return playlists[playlist_index].songs[song_index];
}

void refreshSongs() {
    playlists.clear();
    for (const auto& directory : std::filesystem::directory_iterator::directory_iterator("Songs")) {
        if (directory.is_directory()) {
            Playlist p;
            p.name = directory.path().filename().string();
            for (const auto& file : std::filesystem::directory_iterator::directory_iterator(directory.path())) {
                if (file.is_regular_file()) {
                    Song s;
                    s.music = new sf::Music();
                    s.music->openFromFile(file.path().string());
                    s.name = file.path().filename().string();
                    p.songs.push_back(s);
                }
            }
            playlists.push_back(p);
        }
    }
}

// skip songs with keyboard keys
HHOOK _k_hook;
LRESULT __stdcall k_Callback1(int nCode, WPARAM wParam, LPARAM lParam)
{
    PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
    //a key was pressed
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION)
    {
        if (key->vkCode == VK_MEDIA_NEXT_TRACK && hasPlayed) {
            Song next = getSong(Where::NEXT);
            currentSong->stop();
            currentSong = next.music;
            currentSong->play();
            currentSongName = next.name;
        }
        else if (key->vkCode == VK_MEDIA_PREV_TRACK && hasPlayed) {
            Song prev = getSong(Where::PREV);
            currentSong->stop();
            currentSong = prev.music;
            currentSong->play();
            currentSongName = prev.name;
        }
        else if (key->vkCode == VK_MEDIA_PLAY_PAUSE && hasPlayed) {
            pause = !pause;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main()
{
    _k_hook = SetWindowsHookEx(WH_KEYBOARD_LL, k_Callback1, NULL, 0);
    MSG msg;

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Music");
    ImGui::SFML::Init(window);

    refreshSongs();

    std::thread ytdl_thread;
    std::thread song_event_thread;

    currentSong = new sf::Music();
    std::string currentPath = std::filesystem::current_path().string();
    for (int i = 0; i < currentPath.size(); i++) {
        if (currentPath[i] == '\\') {
            currentPath[i] = '/';
        }
    }

    bool createPlaylist = false;
    bool addSong = false;
    int volume = 50;
    bool checkSongEvent = false;

    char* yt_link = (char*)malloc(sizeof(char) * 128);
    memset(yt_link, 0, 128);

    char* playlist_input = (char*)malloc(sizeof(char) * 32);
    memset(playlist_input, 0, 32);


    sf::Clock deltaClock;

    // run the program as long as the window is open
    while (window.isOpen())
    {
        window.setFramerateLimit(60);

        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
                checkSongEvent = false;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Key::Space) {
                    pause = !pause;
                }
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        // controls
        {ImGui::Begin("Controls");
        ImGui::Text("Current Song: %s", currentSongName.c_str());
        ImGui::ProgressBar(currentSong->getPlayingOffset().asSeconds() / currentSong->getDuration().asSeconds(), ImVec2(0, 0.5));

        if (ImGui::Button("Refresh Songs")) {
            refreshSongs();
        }

        if (ImGui::Button("Create Playlist")) { createPlaylist = true; }

        if (ImGui::Button("Add Song")) { addSong = true; }
        ImGui::Separator();

        ImGui::Checkbox("Loop Playlist", &loop);
        if (loop) {
            ImGui::TreePush();
            ImGui::Checkbox("Loop Song", &loopSong);
            ImGui::TreePop();
        }
        ImGui::Checkbox("Pause", &pause);
        
        
        if (ImGui::Button("Prev Song")) {
            Song prev = getSong(Where::PREV);
            currentSong->stop();
            currentSong = prev.music;
            currentSong->play();
            currentSongName = prev.name;
        }
        ImGui::SameLine(); if (ImGui::Button("Next Song")) {
            Song next = getSong(Where::NEXT);
            currentSong->stop();
            currentSong = next.music;
            currentSong->play();
            currentSongName = next.name;
        }

        ImGui::DragInt("Volume", &volume, 1, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
        ImGui::Separator();
        if (createPlaylist) {

            ImGui::InputText("Playlist Name", playlist_input, 32);
            if (ImGui::Button("Create")) {
                createPlaylist = false;
                std::filesystem::create_directory("Songs\\" + std::string(playlist_input));
                memset(playlist_input, 0, 32);
                refreshSongs();
            }
            ImGui::SameLine();  if (ImGui::Button("Cancel")) {
                createPlaylist = false;
            }
            ImGui::Separator();
        }
        if (addSong) {
            ImGui::InputText("Playlist Name", playlist_input, 32);
            ImGui::InputText("YT Link", yt_link, 128);
            if (ImGui::Button("Add")) {
                addSong = false;
                std::string command = ".\\youtube-dl.exe --extract-audio --audio-format vorbis -o \"" + currentPath + "/Songs/" + std::string(playlist_input) + "/%(title)s.%(ext)s" + "\" " + std::string(yt_link);
                std::cout << command << std::endl;
                ytdl_thread = std::thread([command, &ytdl_thread]() {
                    system(command.c_str());
                    refreshSongs();
                    std::cout << "done downloading" << std::endl;
                    ytdl_thread.detach();
                });
            }
            ImGui::SameLine();  if (ImGui::Button("Cancel")) {
                addSong = false;
            }
            ImGui::Separator();
        }
        ImGui::End(); }


        // playlists
        {ImGui::Begin("Playlists");
        for (size_t i = 0; i < playlists.size(); i++) {
            if (ImGui::TreeNode(playlists[i].name.c_str())) {
                for (size_t k = 0; k < playlists[i].songs.size(); k++) {
                    ImGui::Bullet(); if (ImGui::SmallButton(playlists[i].songs[k].name.c_str())) {
                        currentSong->stop();
                        currentSong = playlists[i].songs[k].music;
                        currentSongName = playlists[i].songs[k].name;
                        currentSong->play();
                        pause = false;
                        hasPlayed = true;
                        playlist_index = i;
                        song_index = k;
                    };
                }
                ImGui::TreePop();
            }
        }
        ImGui::End(); }



        if (hasPlayed) {
            if (currentSong != nullptr) {
                if (currentSong->getStatus() == sf::Music::Status::Stopped) {
                    Song next = getSong(Where::NEXT);
                    currentSong->stop();
                    currentSong = next.music;
                    currentSong->play();
                    currentSongName = next.name;
                }
                currentSong->setVolume(volume);
                if (pause && currentSong->getStatus() != sf::Music::Status::Paused) {
                    currentSong->pause();
                }
                else if (!pause && currentSong->getStatus() == sf::Music::Status::Paused) {
                    currentSong->play();
                }
            }
        }

        window.clear(sf::Color::Black);

        // window.draw(...);
        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();

    for (Playlist &p : playlists) {
        for (Song s: p.songs) {
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

    return 0;
}