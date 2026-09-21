#include "Player.h"
#include <string>
#include <iostream>
#include <thread>

class Player {
public:
	virtual ~Player() = default;
	void find_and_play(std::string filename) {
		if (playlist->find(filename) != playlist->end()) {
			this->Play(filename);
		}
		else {
			std::cout << "File not found in playlist: " << filename << std::endl;
		}
	}
	void Play(std::string filename = '\0') {
		std::cout << "Playing: " << filename << std::endl;
		
	}
	void Pause() {
		std::cout << "Paused" << std::endl;
	}
	void Stop() {
		std::cout << "Stopped" << std::endl;
	}

protected:
	std::unique_ptr<std::unordered_map<std::string, std::string>> playlist =
		std::make_unique<std::unordered_map<std::string, std::string>>();
	std::thread t1;
};