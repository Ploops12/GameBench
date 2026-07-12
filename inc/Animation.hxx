#pragma once

#include "Sprite.hxx"

#include <string>

// Advances frames from an owned Sprite at a fixed rate and exposes matching
// screen-space and billboard drawing helpers.
class Animation {
public:
	Animation() noexcept = default;
	explicit Animation(Sprite sprite, float framesPerSecond = 12.0f,
		bool looping = true);
	Animation(const std::string& filePath, int columns = 1, int rows = 1,
		float framesPerSecond = 12.0f, bool looping = true);

	Animation(const Animation&) = delete;
	Animation& operator=(const Animation&) = delete;
	Animation(Animation&&) noexcept = default;
	Animation& operator=(Animation&&) noexcept = default;

	bool load(const std::string& filePath, int columns = 1, int rows = 1,
		float framesPerSecond = 12.0f, bool looping = true);
	bool loaded() const noexcept;

	Sprite& sprite() noexcept;
	const Sprite& sprite() const noexcept;

	void setFramesPerSecond(float framesPerSecond) noexcept;
	float framesPerSecond() const noexcept;
	float frameDuration() const noexcept;

	void setLooping(bool looping) noexcept;
	bool looping() const noexcept;
	bool playing() const noexcept;
	bool finished() const noexcept;
	int currentFrame() const noexcept;

	void play() noexcept;
	void pause() noexcept;
	void stop() noexcept;
	void reset() noexcept;
	void setFrame(int frameIndex) noexcept;
	void update(float deltaSeconds) noexcept;

	void draw(Vector2 position, Color tint = WHITE) const;
	void drawPro(Rectangle destination, Vector2 origin = { 0.0f, 0.0f },
		float rotation = 0.0f, Color tint = WHITE) const;
	void drawBillboard(const Camera3D& camera, Vector3 position,
		float height, Color tint = WHITE) const;
	void drawBillboardPro(const Camera3D& camera, Vector3 position,
		Vector3 up, Vector2 size, Vector2 origin = { 0.0f, 0.0f },
		float rotation = 0.0f, Color tint = WHITE) const;

private:
	Sprite sprite_ {};
	float framesPerSecond_ { 12.0f };
	float elapsedInFrame_ { 0.0f };
	int currentFrame_ { 0 };
	bool looping_ { true };
	bool playing_ { false };
	bool finished_ { false };
};
