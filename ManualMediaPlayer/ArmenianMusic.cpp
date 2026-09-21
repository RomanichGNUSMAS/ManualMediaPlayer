#include "Player.h";
#include <memory>
#include <string>
#include <fstream>

class ArmenianPlayer : public Player {


public:
	ArmenianPlayer() {
		std::ifstream file("ArmenianPlaylist.txt");
		char ch;
		std::string str;
		while (file.get(ch)) {
			if (ch == '\n') {
				this->playlist->insert({ str, str });
				str.clear();
			}
			else {
				str += ch;
			}
		}
	}

};