#pragma once

#include <raylib.h>

class Sprite {
public:
	Sprite() = default;
	explicit Sprite(const char* path, int columns = 1, int rows = 1);
	~Sprite();
	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;
	Sprite(Sprite&& other) noexcept;
	Sprite& operator=(Sprite&& other) noexcept;

	bool load(const char* path, int columns = 1, int rows = 1);
	void unload();
	bool isReady() const;
	void setFrame(int index);
	int getFrame() const { return frame; }
	int getFrameCount() const { return columns * rows; }
	Rectangle getSource() const;
	void draw(Vector2 position, float scale = 1.0f, Color tint = WHITE) const;
	void drawBillboard(const Camera3D& camera, Vector3 position, float size, Color tint = WHITE) const;

private:
	Texture2D texture {};
	int columns {1};
	int rows {1};
	int frame {};
};
