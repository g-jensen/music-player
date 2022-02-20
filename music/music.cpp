#include <SFML/Graphics.hpp>

#include <iostream>
#include <ctime>

#include "GUI.h"
#include "discord_rp/discord_rpc.h"
#include "discord_rp/discord_register.h"

int main()
{

    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    Discord_Initialize("944819240114987028", &handlers, 1, NULL);

    memset(&G::discordPresence, 0, sizeof(G::discordPresence));
    G::discordPresence.startTimestamp = std::time(0);
    G::discordPresence.details = "Idling";
    G::discordPresence.largeImageKey = "images_large_";
    Discord_UpdatePresence(&G::discordPresence);

    Discord_RunCallbacks();
    

    memset(G::playlist_input, 0, 32);
    memset(G::yt_link, 0, 128);

    G::_k_hook = SetWindowsHookEx(WH_KEYBOARD_LL, G::k_Callback1, NULL, 0);

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Music");
    ImGui::SFML::Init(window);

    Playlist::refreshSongs(G::playlists);

    for (size_t i = 0; i < G::currentPath.size(); i++) {
        if (G::currentPath[i] == '\\') {
            G::currentPath[i] = '/';
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
                G::checkSongEvent = false;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Key::Space) {
                    G::pause = !G::pause;
                }
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());
        Discord_RunCallbacks();

        // controls
        GUI::DrawControls();

        // playlists
        GUI::DrawPlaylists();

        // info
        GUI::DrawInfo();

        // update volume, song, etc
        G::UpdateCurrentSong();

        window.clear(sf::Color::Black);

        ImGui::SFML::Render(window);

        window.display();
    }
    
    G::ResourceCleanUp();

    return 0;
}