#include "GUI.h"

void GUI::DrawInfo()
{
    ImGui::Begin("Info");
    if (ImGui::Button("How it works")) {
        G::showInfo = !G::showInfo;
    }
    ImGui::End();
    if (G::showInfo) {
        ImGui::Begin("How it works");
        ImGui::Text("Music player by Greg (www.github.com/g-jensen/music-player)");
        ImGui::Separator();
        ImGui::NewLine();
        ImGui::Text("When you click 'Add Song' you input:");
        ImGui::TreePush();
        ImGui::BulletText("Playlist Name - playlist you want to add to (creates the playlist if it doesnt already exist).");
        ImGui::BulletText("YT Link - link to youtube video you want to download the audio for.");
        ImGui::TreePop();
        ImGui::NewLine();
        ImGui::Text("The program will then use youtube-dl to download the audio.");
        ImGui::Text("You can see debugging info in the second window.");
        ImGui::NewLine();
        ImGui::Text("Any folder in 'Songs' will be treated as a playlist.");
        ImGui::Text("Any .ogg file in a folder in 'Songs' will be treated as a song so you may also import that way.");
        ImGui::Separator();
        ImGui::NewLine();
        ImGui::Text("Credits: (all linked in README.md)");
        ImGui::TreePush();
        ImGui::BulletText("SFML: opening window, managing audio files");
        ImGui::BulletText("Dear ImGui: GUI boxes");
        ImGui::BulletText("imgui-sfml: Compatibility between SFML and ImGui");
        ImGui::BulletText("youtube-dl: Downloading audio from youtube links");
        ImGui::TreePop();
        ImGui::End();
    }
}

void GUI::DrawControls()
{
    ImGui::Begin("Controls");
    ImGui::Text("Current Song: %s", G::currentSongName.c_str());

    ImGui::ProgressBar(G::currentSong->getPlayingOffset() / G::currentSong->getDuration(),ImVec2(0,0.5));
    
    if (ImGui::SmallButton("-5 sec") && G::currentSong->getPlayingOffset().asSeconds() > 5) {
        G::currentSong->setPlayingOffset(sf::seconds(G::currentSong->getPlayingOffset().asSeconds() - 5));
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("+5 sec") && G::currentSong->getPlayingOffset().asSeconds() < G::currentSong->getDuration().asSeconds() - 5) {
        G::currentSong->setPlayingOffset(sf::seconds(G::currentSong->getPlayingOffset().asSeconds() + 5));
    }

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
        G::PrevSong();
    }
    ImGui::SameLine(); if (ImGui::Button("Next Song")) {
        G::NextSong();
    }

    ImGui::DragInt("Volume", &G::volume, 1, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
    ImGui::Separator();
    if (G::createPlaylist) {
        ImGui::InputText("Playlist Name", G::playlist_input, 32);
        if (ImGui::Button("Create")) {
            G::CreatePlaylist(G::playlist_input);
            G::createPlaylist = false;
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
            G::AddSong(G::playlist_input,G::yt_link);
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

                if (G::currentSong == G::playlists[i].songs[k].music) {
                    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(7.0f, 0.6f, 0.6f));
                    ImGui::Bullet(); if (ImGui::SmallButton(G::playlists[i].songs[k].name.c_str())) {
                        G::PlaySong(i, k);
                    };
                    ImGui::PopStyleColor(1);
                }
                else {
                    ImGui::Bullet(); if (ImGui::SmallButton(G::playlists[i].songs[k].name.c_str())) {
                        G::PlaySong(i, k);
                    };
                }

            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}
