#pragma once

class Menu {
public:
	virtual ~Menu() = default;

	bool isVisible() const;
	void setVisible(bool value);
protected:
	bool visible {false};
};
