#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "Player.h"
#include <string>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <vector>

std::mutex mtx; // Мьютекс для защиты потокоопасных данных (playlist, Processes)

Player::Player() : playlist(std::make_unique<std::unordered_map<std::string, std::string>>()) {
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        std::cerr << "Cannot install audiodriver!\n";
    }
    this->isLoaded = false;
    this->isPlaying = false;
    this->isPaused = false;

    std::ifstream ifs("./musics/playlist.txt");
    if (!ifs.is_open()) {
        std::filesystem::path dir_path = "./musics";
        std::filesystem::path file_path = dir_path / "playlist.txt";

        try {
            if (std::filesystem::create_directories(dir_path)) {
                std::cout << "Директория успешно создана: " << dir_path << std::endl;
            }

            std::ofstream file(file_path);
            if (file.is_open()) {
                file << "a.mp3\nb.mp3\n";
                file.close();
                std::cout << "Файл успешно создан: " << file_path << std::endl;
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cout << e.what() << std::endl;
        }
    }
    else {
        std::string line;
        while (std::getline(ifs, line)) {
            while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' ')) {
                line.pop_back();
            }
            if (!line.empty()) {
                this->playlist->insert({ line, line });
            }
        }
    }
}

bool Player::load(std::string& filepath) {
    if (this->isLoaded) {
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
        this->isLoaded = false;
    }

    std::string cleanPath = filepath;
    while (!cleanPath.empty() && (cleanPath.back() == '\r' || cleanPath.back() == '\n' || cleanPath.back() == ' ')) {
        cleanPath.pop_back();
    }

    std::string final = "musics/" + cleanPath;

    ma_result res = ma_sound_init_from_file(&engine, final.c_str(), 0, NULL, NULL, &this->sound);
    if (res == MA_SUCCESS) {
        this->isLoaded = true;
        return true;
    }

    std::cout << "[DEBUG] Ошибка miniaudio code: " << res << " по пути: [" << final << "]" << std::endl;
    return false;
}

void Player::playTheWave() {
    std::vector<std::string> tracksToPlay;

    {
        std::lock_guard<std::mutex> g(mtx);
        if (!this->playlist || this->playlist->empty()) {
            throw std::runtime_error("Playlist is empty or not initialized!");
        }
        for (const auto& [name, path] : *(this->playlist)) {
            tracksToPlay.push_back(path);
        }
    }

    for ( auto& path : tracksToPlay) {
        try {
            if (this->load(path)) {
                ma_sound_start(&sound);
                this->isPlaying = true;
                this->isPaused = false;

                while (this->isPlaying) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));

                    if (this->isPaused) {
                        continue;
                    }

                    if (ma_sound_at_end(&sound)) {
                        break;
                    }
                }
            }
        }
        catch (const std::exception& e) {
            std::cout << "[Wave Track Error]: " << e.what() << std::endl;
        }

        if (!this->isPlaying) break;
    }

    this->isPlaying = false;
    this->isPaused = false;
}

void Player::Play(const std::string& filename, int flag) {
    std::string trackPath;

    if (filename == "wave") {
        try {
            this->playTheWave();
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
    else if (flag < 1) {
        {
            std::lock_guard<std::mutex> g(mtx);
            if (!this->playlist) {
                std::cout << "Error: Playlist is not initialized!" << std::endl;
                return;
            }

            auto it = this->playlist->find(filename);
            if (it != this->playlist->end()) {
                trackPath = it->second;
            }
            else {
                std::cout << "Failed to load audio file: file not found in playlist\n";
                return;
            }
        }

        if (!trackPath.empty()) {
            if (this->load(trackPath)) {
                ma_sound_start(&sound);
                this->isPlaying = true;
                this->isPaused = false;
            }
        }
    }
    else if (filename.empty() && flag > 1) {
        if (!this->isPlaying && this->isLoaded) {
            this->isPlaying = true;
            this->isPaused = false;
            ma_sound_start(&sound);
        }
    }

    if (this->Processes) {
        std::lock_guard<std::mutex> g(mtx);
        this->Processes->push_back(Queue{ "Play", {filename} });
    }
}

void Player::Stop() {
    if (this->isLoaded) {
        ma_sound_stop(&sound);
        ma_sound_seek_to_pcm_frame(&sound, 0);
        this->isPlaying = false;
        this->isPaused = false;
        std::cout << "[Player]: Остановлено" << std::endl;
    }
}

void Player::Pause() {
    if (this->isLoaded && this->isPlaying && !this->isPaused) {
        ma_sound_stop(&sound);
        this->isPaused = true;

        if (this->Processes) {
            std::lock_guard<std::mutex> g(mtx);
            this->Processes->push_back(Queue{ "Pause" });
        }
        std::cout << "[Player]: Пауза" << std::endl;
    }
}

void Player::Resume() {
    if (this->isLoaded && this->isPaused) {
        ma_sound_start(&sound);
        this->isPaused = false;

        if (this->Processes) {
            std::lock_guard<std::mutex> g(mtx);
            this->Processes->push_back(Queue{ "Resume" });
        }
        std::cout << "[Player]: Воспроизведение продолжено" << std::endl;
    }
}

Player::~Player() {
    if (this->isLoaded) {
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
        this->isLoaded = false;
    }
    ma_engine_uninit(&engine);
    this->isPlaying = false;
    this->isPaused = false;
}