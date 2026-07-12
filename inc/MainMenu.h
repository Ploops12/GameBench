#pragma once

#include "Menu.hxx"

class MainMenu : public Menu {
public:
	MainMenu();

	void update(float deltaTime) override;
	void draw() const override;
	bool wantsToStart() const;

private:
	bool startRequested{false};
};
