#pragma once

#include <raylib.h>
#include <string>

class Sprite {
public:
	Sprite() = default;
	Sprite(const std::string& path, int columns = 1, int rows = 1);
	~Sprite();
	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;
	Sprite(Sprite&& other) noexcept;
	Sprite& operator=(Sprite&& other) noexcept;

	[[nodiscard]] bool load(const std::string& path, int columns = 1, int rows = 1);
	void unload();
	void setFrame(int frame);
	[[nodiscard]] int frame() const;
	[[nodiscard]] int frameCount() const;
	[[nodiscard]] bool isLoaded() const;
	[[nodiscard]] Rectangle source() const;

	void draw(Vector2 position, float scale, Color tint = WHITE) const;
	void drawPro(Rectangle destination, Vector2 origin, float rotation, Color tint = WHITE) const;
	void drawBillboard(const Camera3D& camera, Vector3 position, float scale, Color tint = WHITE) const;

private:
	Texture2D texture_ {};
	int columns_ {1};
	int rows_ {1};
	int frame_ {0};
};
