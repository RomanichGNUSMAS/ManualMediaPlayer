#include <iostream>
#include "Controller.h"
#include <Windows.h>

int main() {
	try {
		SetConsoleCP(65001);
		SetConsoleOutputCP(65001);
		std::cout << "Welcome to the Manual Media Player!" << std::endl;
		ControllerLoop();
		return 0;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}