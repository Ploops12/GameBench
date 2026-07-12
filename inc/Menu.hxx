#pragma once

// Base class for simple active/inactive UI screens.

class Menu {
public:
	virtual ~Menu() = default;
	virtual void update(float dt) = 0;
	virtual void draw() const = 0;

	bool isActive() const;
	void setActive(bool value);

protected:
	bool active{true};
};
