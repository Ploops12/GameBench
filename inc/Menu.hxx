#pragma once

class Menu {
public:
	virtual ~Menu() = default;
	virtual bool isOpen() const = 0;
};
