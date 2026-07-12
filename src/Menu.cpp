#include "Menu.hxx"

void Menu::update(float deltaTime) {
	(void)deltaTime;
}

void Menu::draw() const {
}

bool Menu::isActive() const {
	return active;
}

void Menu::setActive(bool value) {
	active = value;
}
