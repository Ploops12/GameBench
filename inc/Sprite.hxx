#pragma once

// Texture/sprite-sheet wrapper for optional bitmap assets.

#include <raylib.h>

class Sprite {
public:
	Sprite() = default;
	explicit Sprite(const char* path, int frameCount = 1);
	~Sprite();

	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;
	Sprite(Sprite&& other) noexcept;
	Sprite& operator=(Sprite&& other) noexcept;

	bool load(const char* path, int frameCount = 1);
	void unload();
	bool isLoaded() const;
	int frames() const;
	Rectangle frameRect(int frameIndex) const;
	void draw(Rectangle destination, int frameIndex = 0, Color tint = WHITE) const;
	const Texture2D& texture() const;
	void setFrameCountForTest(int frameCount);

private:
	Texture2D image{};
	int frameTotal{1};
	bool loaded{};
};
