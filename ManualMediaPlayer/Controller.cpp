#include "Controller.h"
#include "Player.h"
#include "Adder.h"
#include <iostream>
#include <string>
#include <thread>

int ControllerLoop() {
	std::string input;
	std::jthread t1;
	Player player;
	while (true) {
		std::cout << "Commands:" << std::endl;
		std::cout << "p <filename> - Play a file" << std::endl;
		std::cout << "s - Stop playback" << std::endl;
		std::cout << "a <filename> - Add a file to the playlist" << std::endl;
		std::cout << "q - Quit the player" << std::endl;
		std::string input;
		std::getline(std::cin, input);
		switch (input[0]) {
		case 'p': {
			if (input.length() < 3) break;
			std::string filename = input.substr(2);

			// Присваивание нового потока автоматически остановит (join) старый t1
			t1 = std::jthread([&player, &filename]() {
				player.Play(filename == "wave" ? "" : filename, 0);
				});
			break;
		}

		case 's': {
			t1 = std::jthread(&Player::Stop, &player);
			;
			break;
		}
		case 'a': {
			std::string filename = input.substr(2);
			std::string result;
			std::jthread temp([&]() {
				result = Add(filename);
			});
			std::cout << result << std::endl;
			break;
		}
		case 'q':
			return 0;
		default:
			return 1;
		}
	}
	return 0;
}

