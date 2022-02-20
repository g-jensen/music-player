#include "Globals.h"

std::vector<Playlist> G::playlists;

sf::Music* G::currentSong = new sf::Music();
std::string G::currentSongName = "None";
std::string G::currentPath = std::filesystem::current_path().string();

SongState G::songState;
bool G::pause = false;
bool G::hasPlayed = false;

bool G::createPlaylist = false;
bool G::addSong = false;
int G::volume = 50;
bool G::checkSongEvent = false;

std::thread G::ytdl_thread;
std::thread G::song_event_thread;

char* G::yt_link = (char*)malloc(sizeof(char) * 128);
char* G::playlist_input = (char*)malloc(sizeof(char) * 32);

HHOOK G::_k_hook;

void G::ResourceCleanUp() {

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

void G::UpdateCurrentSong()
{
    if (hasPlayed) {
        if (currentSong != nullptr) {
            if (currentSong->getStatus() == sf::Music::Status::Stopped) {
                Song next = Playlist::getSong(playlists, Where::NEXT, G::songState);
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

LRESULT __stdcall G::k_Callback1(int nCode, WPARAM wParam, LPARAM lParam)
{
    PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
    //a key was pressed
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION)
    {
        if (key->vkCode == VK_MEDIA_NEXT_TRACK && G::hasPlayed) {
            Song next = Playlist::getSong(G::playlists, Where::NEXT, G::songState);
            G::currentSong->stop();
            G::currentSong = next.music;
            G::currentSong->play();
            G::currentSongName = next.name;
        }
        else if (key->vkCode == VK_MEDIA_PREV_TRACK && G::hasPlayed) {
            Song prev = Playlist::getSong(G::playlists, Where::PREV, G::songState);
            G::currentSong->stop();
            G::currentSong = prev.music;
            G::currentSong->play();
            G::currentSongName = prev.name;
        }
        else if (key->vkCode == VK_MEDIA_PLAY_PAUSE && G::hasPlayed) {
            G::pause = !G::pause;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
