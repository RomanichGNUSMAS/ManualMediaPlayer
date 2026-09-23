#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <fstream>
#include <thread>
#include <atomic>
#include "miniaudio.h"


struct Queue {
	std::string OperationName;
	std::vector<std::string> args;
};

class Player {
public:
	Player();
	~Player();
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;
	bool load(std::string&);
	void find_and_play(const std::string& );
	void Play(const std::string&, int);
	void Pause();
	void Resume();
	void Stop();

protected:
	std::unique_ptr<std::vector<Queue>> Processes;
	std::unique_ptr<std::unordered_map<std::string, std::string>> playlist;
	std::thread t1;
	ma_engine engine;
	ma_sound sound;
	std::atomic<bool> isLoaded;
	std::atomic<bool> isPlaying;
	std::atomic<bool> isPaused;
	std::ifstream ifs;
	void playTheWave();
};

#endif
