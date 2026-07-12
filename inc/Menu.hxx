#pragma once

class Menu {
public:
	virtual ~Menu() = default;
	virtual void draw(int screenWidth, int screenHeight) const = 0;
	bool isOpen() const;

protected:
	bool open {false};
};
