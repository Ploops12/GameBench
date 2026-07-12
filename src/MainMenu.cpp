#include "MainMenu.h"

void MainMenu::setCanStart(bool value) {
	startEnabled = value;
}

bool MainMenu::canStart() const {
	return startEnabled;
}
