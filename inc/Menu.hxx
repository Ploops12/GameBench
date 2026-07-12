#pragma once

class Menu {
	// Shared active-state base for screen-space interfaces.
public:
	virtual ~Menu() = default;
	virtual void update(float deltaTime);
	virtual void draw() const;
	bool isActive() const;

protected:
	void setActive(bool value);
	bool active {false};
};
