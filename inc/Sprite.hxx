#pragma once

#include <string>
#include <raylib.h>

class Sprite {
public:
	Sprite() = default;
	~Sprite();

	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;
	Sprite(Sprite&& other) noexcept;
	Sprite& operator=(Sprite&& other) noexcept;

	bool loadFromFile(const std::string& path);
	void unload();

	bool isLoaded() const;
	const Texture2D& getTexture() const;
	Texture2D& getTexture();
	Rectangle frame(int columns, int rows, int index) const;
private:
	Texture2D texture {};
	bool loaded {false};
};
