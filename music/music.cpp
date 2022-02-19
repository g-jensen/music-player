#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "Globals.h"
//#include "GUI.h"

using namespace Globals;

// skip songs with keyboard keys
LRESULT __stdcall k_Callback1(int nCode, WPARAM wParam, LPARAM lParam)
{
    PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
    //a key was pressed
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION)
    {
        if (key->vkCode == VK_MEDIA_NEXT_TRACK && hasPlayed) {
            Song next = Playlist::getSong(playlists,Where::NEXT,loop,loopSong,playlist_index,song_index);
            currentSong->stop();
            currentSong = next.music;
            currentSong->play();
            currentSongName = next.name;
        }
        else if (key->vkCode == VK_MEDIA_PREV_TRACK && hasPlayed) {
            Song prev = Playlist::getSong(playlists, Where::PREV, loop, loopSong, playlist_index, song_index);
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

void DrawControls() {
    ImGui::Begin("Controls");
    ImGui::Text("Current Song: %s", currentSongName.c_str());
    ImGui::ProgressBar(currentSong->getPlayingOffset().asSeconds() / currentSong->getDuration().asSeconds(), ImVec2(0, 0.5));

    if (ImGui::Button("Refresh Songs")) {
        Playlist::refreshSongs(playlists);
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
        Song prev = Playlist::getSong(playlists, Where::PREV, loop, loopSong, playlist_index, song_index);
        currentSong->stop();
        currentSong = prev.music;
        currentSong->play();
        currentSongName = prev.name;
    }
    ImGui::SameLine(); if (ImGui::Button("Next Song")) {
        Song next = Playlist::getSong(playlists, Where::NEXT, loop, loopSong, playlist_index, song_index);
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
            Playlist::refreshSongs(playlists);
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
            ytdl_thread = std::thread([command]() {
                system(command.c_str());
                Playlist::refreshSongs(playlists);
                std::cout << "done downloading" << std::endl;
                ytdl_thread.detach();
                });
        }
        ImGui::SameLine();  if (ImGui::Button("Cancel")) {
            addSong = false;
        }
        ImGui::Separator();
    }
    ImGui::End();
}

void DrawPlaylists() {
    ImGui::Begin("Playlists");
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
    ImGui::End();
}

void UpdateCurrentSong() {
    if (hasPlayed) {
        if (currentSong != nullptr) {
            if (currentSong->getStatus() == sf::Music::Status::Stopped) {
                Song next = Playlist::getSong(playlists, Where::NEXT, loop, loopSong, playlist_index, song_index);
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
}

int main()
{

    memset(playlist_input, 0, 32);
    memset(yt_link, 0, 128);

    _k_hook = SetWindowsHookEx(WH_KEYBOARD_LL, k_Callback1, NULL, 0);

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Music");
    ImGui::SFML::Init(window);

    Playlist::refreshSongs(playlists);

    for (size_t i = 0; i < currentPath.size(); i++) {
        if (currentPath[i] == '\\') {
            currentPath[i] = '/';
        }
    }


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
        DrawControls();

        // playlists
        DrawPlaylists();

        // update volume, song, etc
        UpdateCurrentSong();

        window.clear(sf::Color::Black);

        // window.draw(...);
        ImGui::SFML::Render(window);

        window.display();
    }
    
    ResourceCleanUp();

    return 0;
}