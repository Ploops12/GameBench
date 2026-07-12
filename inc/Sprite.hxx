#pragma once

#include <raylib.h>

#include <string>

// Owns a texture and optionally divides it into a uniform sprite-sheet grid.
// A Sprite must be loaded after InitWindow() and destroyed (or unloaded) before
// CloseWindow() whenever possible.
class Sprite {
public:
	Sprite() noexcept = default;
	explicit Sprite(const std::string& filePath, int columns = 1, int rows = 1);
	~Sprite() noexcept;

	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;

	Sprite(Sprite&& other) noexcept;
	Sprite& operator=(Sprite&& other) noexcept;

	// Loads a whole image or a uniformly gridded sprite sheet. A failed load
	// leaves an already loaded sprite intact.
	bool load(const std::string& filePath, int columns = 1, int rows = 1);
	void unload() noexcept;

	bool loaded() const noexcept;
	const std::string& path() const noexcept;
	const Texture2D& texture() const noexcept;

	int width() const noexcept;
	int height() const noexcept;
	int frameWidth() const noexcept;
	int frameHeight() const noexcept;
	int columns() const noexcept;
	int rows() const noexcept;
	int frameCount() const noexcept;
	bool isFrameValid(int frameIndex) const noexcept;
	Rectangle sourceRect(int frameIndex = 0) const noexcept;

	// 2D screen-space drawing helpers.
	void draw(Vector2 position, Color tint = WHITE) const;
	void drawFrame(int frameIndex, Vector2 position, Color tint = WHITE) const;
	void drawFramePro(int frameIndex, Rectangle destination,
		Vector2 origin = { 0.0f, 0.0f }, float rotation = 0.0f,
		Color tint = WHITE) const;

	// 3D camera-facing billboard helpers. The simple version treats size as the
	// billboard height and preserves the selected frame's aspect ratio.
	void drawBillboard(const Camera3D& camera, Vector3 position,
		float height, Color tint = WHITE) const;
	void drawBillboardFrame(const Camera3D& camera, int frameIndex,
		Vector3 position, float height, Color tint = WHITE) const;
	void drawBillboardFramePro(const Camera3D& camera, int frameIndex,
		Vector3 position, Vector3 up, Vector2 size,
		Vector2 origin = { 0.0f, 0.0f }, float rotation = 0.0f,
		Color tint = WHITE) const;

private:
	void takeFrom(Sprite&& other) noexcept;

	Texture2D texture_ {};
	std::string path_ {};
	int columns_ { 1 };
	int rows_ { 1 };
	bool loaded_ { false };
};
