#include <iostream>
#include "Controller.h"

int main() {
	try {
		std::cout << "Welcome to the Manual Media Player!" << std::endl;
		ControllerLoop();
		return 0;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}