#include "GUI.h"

void GUI::DrawControls()
{
    ImGui::Begin("Controls");
    ImGui::Text("Current Song: %s", G::currentSongName.c_str());
    ImGui::ProgressBar(G::currentSong->getPlayingOffset().asSeconds() / G::currentSong->getDuration().asSeconds(), ImVec2(0, 0.5));

    if (ImGui::Button("Refresh Songs")) {
        Playlist::refreshSongs(G::playlists);
    }

    if (ImGui::Button("Create Playlist")) { G::createPlaylist = true; }

    if (ImGui::Button("Add Song")) { G::addSong = true; }
    ImGui::Separator();

    ImGui::Checkbox("Loop Playlist", &G::songState.loop);
    if (G::songState.loop) {
        ImGui::TreePush();
        ImGui::Checkbox("Loop Song", &G::songState.loopSong);
        ImGui::TreePop();
    }
    ImGui::Checkbox("Pause", &G::pause);


    if (ImGui::Button("Prev Song")) {
        Song prev = Playlist::getSong(G::playlists, Where::PREV, G::songState);
        G::currentSong->stop();
        G::currentSong = prev.music;
        G::currentSong->play();
        G::currentSongName = prev.name;
    }
    ImGui::SameLine(); if (ImGui::Button("Next Song")) {
        Song next = Playlist::getSong(G::playlists, Where::NEXT, G::songState);
        G::currentSong->stop();
        G::currentSong = next.music;
        G::currentSong->play();
        G::currentSongName = next.name;
    }

    ImGui::DragInt("Volume", &G::volume, 1, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
    ImGui::Separator();
    if (G::createPlaylist) {

        ImGui::InputText("Playlist Name", G::playlist_input, 32);
        if (ImGui::Button("Create")) {
            G::createPlaylist = false;
            std::filesystem::create_directory("Songs\\" + std::string(G::playlist_input));
            memset(G::playlist_input, 0, 32);
            Playlist::refreshSongs(G::playlists);
        }
        ImGui::SameLine();  if (ImGui::Button("Cancel")) {
            G::createPlaylist = false;
        }
        ImGui::Separator();
    }
    if (G::addSong) {
        ImGui::InputText("Playlist Name", G::playlist_input, 32);
        ImGui::InputText("YT Link", G::yt_link, 128);
        if (ImGui::Button("Add")) {
            G::addSong = false;
            std::string command = ".\\youtube-dl.exe --extract-audio --audio-format vorbis -o \"" + G::currentPath + "/Songs/" + std::string(G::playlist_input) + "/%(title)s.%(ext)s" + "\" " + std::string(G::yt_link);
            std::cout << command << std::endl;
            G::ytdl_thread = std::thread([command]() {
                system(command.c_str());
                Playlist::refreshSongs(G::playlists);
                std::cout << "done downloading" << std::endl;
                G::ytdl_thread.detach();
            });
        }
        ImGui::SameLine();  if (ImGui::Button("Cancel")) {
            G::addSong = false;
        }
        ImGui::Separator();
    }
    ImGui::End();
}

void GUI::DrawPlaylists()
{
    ImGui::Begin("Playlists");
    for (size_t i = 0; i < G::playlists.size(); i++) {
        if (ImGui::TreeNode(G::playlists[i].name.c_str())) {
            for (size_t k = 0; k < G::playlists[i].songs.size(); k++) {
                ImGui::Bullet(); if (ImGui::SmallButton(G::playlists[i].songs[k].name.c_str())) {
                    G::currentSong->stop();
                    G::currentSong = G::playlists[i].songs[k].music;
                    G::currentSongName = G::playlists[i].songs[k].name;
                    G::currentSong->play();
                    G::pause = false;
                    G::hasPlayed = true;
                    G::songState.playlist_index = i;
                    G::songState.song_index = k;
                };
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}
