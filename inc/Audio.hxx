#pragma once

#include <array>
#include <cstddef>
#include <raylib.h>

enum class GameSound : std::size_t {
	PoppetCast,
	BottleCast,
	AppleCast,
	SpiderCast,
	HexImpact,
	Pickup,
	Hurt,
	WardRaise,
	WardImpact,
	EnemyDeath,
	NewWave,
	Count
};

class AudioFeedback {
public:
	AudioFeedback();
	~AudioFeedback();
	AudioFeedback(const AudioFeedback&) = delete;
	AudioFeedback& operator=(const AudioFeedback&) = delete;

	void initialize();
	void shutdown();
	void play(GameSound sound);
	[[nodiscard]] bool isReady() const;

private:
	std::array<Sound, static_cast<std::size_t>(GameSound::Count)> sounds_ {};
	bool ready_ {false};
};
