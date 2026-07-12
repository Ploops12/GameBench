#pragma once

#include <string>
#include <raylib.h>

class Sprite {
	// Move-only Raylib texture wrapper supporting regular images and uniform sheets.
public:
	Sprite() = default;
	Sprite(const std::string& path, int columns = 1, int rows = 1);
	~Sprite();
	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;
	Sprite(Sprite&& other) noexcept;
	Sprite& operator=(Sprite&& other) noexcept;

	bool load(const std::string& path, int columns = 1, int rows = 1);
	void unload();
	bool isLoaded() const;
	Rectangle getFrameSource(int frame) const;
	void draw2D(int frame, Rectangle destination, Color tint = WHITE) const;
	void drawBillboard(const Camera3D& camera, int frame, Vector3 position, Vector2 size,
		Color tint = WHITE) const;
	const Texture2D& getTexture() const;
	int getFrameCount() const;

private:
	Texture2D texture {};
	int columns {1};
	int rows {1};
};
