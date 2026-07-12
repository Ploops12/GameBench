#pragma once

class Menu {
public:
	virtual ~Menu() = default;
	virtual void draw() const = 0;
	[[nodiscard]] bool isActive() const;
	void setActive(bool active);

protected:
	bool active_ {false};
};
