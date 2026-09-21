#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include <memory>
#include <string>
#include <unordered_map>


class Player {

public:
	virtual ~Player() = default;
	std::unique_ptr<std::unordered_map<std::string, std::string>> playlist;
	void find_and_play(std::string filename);
	void Play(std::string filename);
	void Pause();
	void Stop();
};




#endif