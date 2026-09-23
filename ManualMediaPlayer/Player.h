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

class Player {
public:
	Player();
	~Player();
	bool load(const std::string&);
	void find_and_play(const std::string& );
	void Play(const std::string&, int);
	void Pause();
	void Stop();

protected:
	std::unique_ptr<std::unordered_map<std::string, std::string>> playlist;
	std::thread t1;
	ma_engine engine;
	ma_sound sound;
	std::atomic<bool> isLoaded;
	std::atomic<bool> isPlaying;
	std::ifstream ifs;
};

#endif
