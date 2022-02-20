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

Song Playlist::getSong(std::vector<Playlist>& playlists, Where w, SongState &songState)
{
    if (songState.loopSong) { return playlists[songState.playlist_index].songs[songState.song_index]; }
    if (w == Where::NEXT) {
        if (songState.song_index >= playlists[songState.playlist_index].songs.size() - 1) {
            if (songState.loop) {
                songState.song_index = 0;
            }
        }
        else {
            songState.song_index++;
        }
    }
    else {
        if (songState.song_index <= 0) {
            if (songState.loop) {
                songState.song_index = playlists[songState.playlist_index].songs.size() - 1;
            }
        }
        else {
            songState.song_index--;
        }
    }
    return playlists[songState.playlist_index].songs[songState.song_index];
}
