#include "Playlist.h"

void Playlist::refreshSongs(std::vector<Playlist>& playlists)
{
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

Song Playlist::getSong(std::vector<Playlist>& playlists, Where w, bool loop, bool loopSong, size_t playlist_index, size_t song_index)
{
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
