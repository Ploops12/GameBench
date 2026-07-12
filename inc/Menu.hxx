#pragma once

class Menu {
public:
	virtual ~Menu() = default;

	virtual void setVisible(bool value);
	virtual bool isVisible() const;

protected:
	bool visible{false};
};
