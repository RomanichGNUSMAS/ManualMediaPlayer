#include "Controller.h"
#include "Player.h"
#include "Adder.h"
#include <iostream>
#include <string>
#include <thread>

int ControllerLoop() {
    std::string input;
    std::jthread playThread;
    Player player;

    while (true) {
        std::cout << "\nCommands:\n"
            << "s <filename> - Play a file (or 's wave')\n"
            << "p            - Pause playback\n"
            << "c            - Continue playback\n"
            << "e            - Stop playback\n"
            << "a <filename> - Add a file to the playlist\n"
            << "q            - Quit\n> ";

        if (!std::getline(std::cin, input) || input.empty()) {
            continue; 
        }

        switch (input[0]) {
        case 's': {
            if (input.length() < 2) break;

            std::string filename = (input.length() > 2) ? input.substr(2) : "";
            if (filename.empty()) break;

            player.Stop();

            playThread = std::jthread([&player, filename]() {
                player.Play(filename, 0);
                });
            break;
        }
        case 'p': {
            player.Pause();
            break;
        }
        case 'c': {
            player.Resume();
            break;
        }
        case 'e': {
            player.Stop();
            break;
        }
        case 'a': {
            if (input.length() < 3) break;
            std::string filename = input.substr(2);
            std::string result;

            {
                std::jthread temp([&result, filename]() {
                    result = Add(filename);
                    });
            }
            std::cout << "[Adder]: " << result << std::endl;
            break;
        }
        case 'q': {
            player.Stop();
            return 0;
        }
        default:
            std::cout << "Неизвестная команда!" << std::endl;
            break;
        }
    }
    return 0;
}