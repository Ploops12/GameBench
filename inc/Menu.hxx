#pragma once

class Menu {
public:
	virtual ~Menu() = default;
	virtual void update(float deltaTime) = 0;
	virtual void draw() const = 0;

	bool isVisible() const;
	void setVisible(bool visible);

protected:
	bool visible{false};
};
