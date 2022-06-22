#include "include/MainApp.h"


MainApp* MainApp::instance = nullptr;

enum Rendom {
	jedan = 1,
	dva = 2,
	tri = 3
};

int main(int argc, char** argv) {
	//MainApp* app = MainApp::getInstance();
	//app->runApp();

	DateTime dt;
	std::cout << dt.getFormat(true) << std::endl;

	return 0;
}