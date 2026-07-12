#pragma once

// Minimal line-art main menu with a play button for entering the combat slice.

#include "Menu.hxx"

#include <raylib.h>

class MainMenu : public Menu {
public:
	void update(float dt) override;
	void draw() const override;
	bool consumePlayRequested();
	void requestPlay();

private:
	Rectangle playButton() const;

	bool playRequested{};
	float pulse{};
};
