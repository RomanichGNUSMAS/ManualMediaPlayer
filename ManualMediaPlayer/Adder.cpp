#include "Adder.h"
#include <string>
#include <fstream>
#include <stdexcept> 
#include <iostream>

std::string Add(std::string musicName) {
    std::ofstream ofs("./musics/playlist.txt", std::ofstream::app);
    try {
        if (!ofs.is_open()) {
            throw std::runtime_error("Cannot open playlist.txt");
        }

        if (musicName.size() < 4) {
            throw std::runtime_error("Invalid file format. File name too short.");
        }

        size_t expectedPos = musicName.size() - 4;

        bool isMp3 = (musicName.rfind(".mp3") == expectedPos);
        bool isWav = (musicName.rfind(".wav") == expectedPos);

        if (!isMp3 && !isWav) {
            throw std::runtime_error("Invalid file format. Please provide an .mp3 or .wav file.");
        }
        ofs << musicName << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
    ofs.close();

    return "Added " + musicName + " to playlist";
}
