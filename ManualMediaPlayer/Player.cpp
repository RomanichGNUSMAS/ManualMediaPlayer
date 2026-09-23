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

std::mutex mtx;

Player::Player() : playlist(std::make_unique<std::unordered_map<std::string, std::string>>()) {
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        std::cerr << "Cannot install audiodriver!\n";
    }
    this->isLoaded = false;
    this->isPlaying = false;
   	std::ifstream ifs("./musics/playlist.txt");
	if (!ifs.is_open()) {
        std::filesystem::path dir_path = "./musics";
        std::filesystem::path file_path = dir_path / "playlist.txt"; 

        try {
            if (std::filesystem::create_directories(dir_path)) {
                std::cout << "Директория успешно создана: " << dir_path << std::endl;
            }
            else {
                std::cout << "Директория уже существует или не создана" << std::endl;
            }

            std::ofstream file(file_path);
            if (file.is_open()) {
                file << "Привет из C++!\n";
                file.close();
                std::cout << "Файл успешно создан: " << file_path << std::endl;
            }
            else {
                std::cerr << "Не удалось открыть/создать файл." << std::endl;
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
			std::cout << e.what() << std::endl;
        }
	}
	else {
		std::string line;
		while (std::getline(ifs, line)) {
			this->playlist->insert({ line, line });
		}
	}
}

bool Player::load(const std::string& filepath) {
    if (this->isLoaded) {
        ma_sound_uninit(&sound);
        this->isLoaded = false;
    }
    ma_result res = ma_sound_init_from_file(&engine, filepath.c_str(), 0, NULL, NULL, &sound);
    if (res == MA_SUCCESS) {
        this->isLoaded = true;
        return true;
    }
    return false;
}
void Player::Play(const std::string& filename, int flag) {
    std::string trackPath;
    std::vector<std::string> wavePaths;

    // 1. Быстро забираем нужные пути под мьютексом
    {
        std::lock_guard<std::mutex> g(mtx);

        if (!this->playlist) {
            std::cout << "Error: Playlist is not initialized!" << std::endl;
            return;
        }
        std::cout << filename << "tran";
        if (filename == "wave") {
            // Копируем пути, чтобы не держать мьютекс во время проигрывания
            for (const auto& [key, value] : *(this->playlist)) {
                wavePaths.push_back(value);
            }
        }
        else if (flag < 1) {
            auto it = this->playlist->find(filename);
            if (it != this->playlist->end()) {
                trackPath = it->second; // Берем ПУТЬ (value), а не имя (key)
            }
            else {
                // std::runtime_error — кроссплатформенный стандарт для C++
                throw std::runtime_error("Failed to load audio file: file not found in playlist");
            }
        }
    } // <-- МЬЮТЕКС ТУТ АВТОМАТИЧЕСКИ ОСВОБОЖДАЕТСЯ!

    // 2. Воспроизведение без блокировки других потоков (Adder и UI свободно работают)
    if (filename == "wave") {
        for (const auto& path : wavePaths) {
            std::thread loader_thread([this, path]() { this->load(path); });
            this->isPlaying = false;
            loader_thread.join();

            if (this->isLoaded) {
                ma_sound_start(&sound);
                this->isPlaying = true;
                while (this->isPlaying) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    if (!ma_sound_is_playing(&sound)) {
                        this->isPlaying = false;
                    }
                }
            }
        }
    }
    else if (flag < 1 && !trackPath.empty()) {
        std::thread loader_thread([this, trackPath]() { this->load(trackPath); });
        this->isPlaying = false;
        loader_thread.join();

        if (this->isLoaded) {
            ma_sound_start(&sound);
            this->isPlaying = true;
        }
    }
}

void Player::Pause() {
    if (this->isPlaying && this->isLoaded) {
        ma_sound_stop(&sound);
        this->isPlaying = false;
    }
}

void Player::Stop() {
	if (this->isPlaying && this->isLoaded) {
		ma_sound_uninit(&sound);
		this->isPlaying = false;
	}
}

Player::~Player() {
    if (this->isLoaded || this->isPlaying) {
		ma_sound_uninit(&sound);
		ma_engine_uninit(&engine);
		isLoaded = false;
        isPlaying = false;
    }
}
