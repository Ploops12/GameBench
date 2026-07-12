#pragma once

#include <raylib.h>

#include <string>

class Sprite {
public:
	Sprite() = default;
	explicit Sprite(const std::string& fileName);
	~Sprite();

	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;
	Sprite(Sprite&& other) noexcept;
	Sprite& operator=(Sprite&& other) noexcept;

	bool load(const std::string& fileName);
	void unload();
	bool isReady() const;
	Texture2D getTexture() const;
	int getWidth() const;
	int getHeight() const;
	void draw(Rectangle destination, Color tint = WHITE) const;
	void drawFrame(Rectangle source, Rectangle destination, Color tint = WHITE) const;

private:
	Texture2D texture {};
	bool loaded {false};
};
