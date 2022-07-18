#include "include/MainApp.h"

MainApp* MainApp::instance = nullptr;

int main(int argc, char** argv) {
	MainApp* app = MainApp::getInstance();
	app->runApp();

	return 0;
}