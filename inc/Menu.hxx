#pragma once

class Menu {
public:
	virtual ~Menu() = default;

	bool isActive() const { return active; }
	void setActive(bool value) { active = value; }

protected:
	bool active {true};
};
