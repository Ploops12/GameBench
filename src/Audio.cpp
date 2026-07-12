#include "Audio.hxx"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace {
constexpr unsigned int SAMPLE_RATE = 22050;

Sound MakeTone(float startFrequency, float endFrequency, float duration, float noiseAmount, float volume) {
	const unsigned int frameCount = static_cast<unsigned int>(duration * static_cast<float>(SAMPLE_RATE));
	std::vector<short> samples(frameCount);
	float phase = 0.0f;
	std::uint32_t noiseState = 0x9e3779b9u;
	for (unsigned int i = 0; i < frameCount; ++i) {
		const float progress = static_cast<float>(i) / static_cast<float>(std::max(1u, frameCount - 1));
		const float frequency = startFrequency + (endFrequency - startFrequency) * progress;
		phase += 2.0f * PI * frequency / static_cast<float>(SAMPLE_RATE);
		noiseState = noiseState * 1664525u + 1013904223u;
		const float noise = (static_cast<float>((noiseState >> 8u) & 0xffffu) / 32767.5f) - 1.0f;
		const float attack = std::min(1.0f, progress * 18.0f);
		const float decay = std::pow(1.0f - progress, 1.7f);
		const float overtone = std::sin(phase * 2.01f) * 0.22f + std::sin(phase * 0.49f) * 0.12f;
		const float signal = (std::sin(phase) + overtone) * (1.0f - noiseAmount) + noise * noiseAmount;
		samples[i] = static_cast<short>(std::clamp(signal * attack * decay * volume, -1.0f, 1.0f) * 32767.0f);
	}
	Wave wave {};
	wave.frameCount = frameCount;
	wave.sampleRate = SAMPLE_RATE;
	wave.sampleSize = 16;
	wave.channels = 1;
	wave.data = samples.data();
	return LoadSoundFromWave(wave);
}
}

AudioFeedback::AudioFeedback() {
	initialize();
}

AudioFeedback::~AudioFeedback() {
	shutdown();
}

void AudioFeedback::initialize() {
	if (ready_ || !IsAudioDeviceReady()) {
		return;
	}
	sounds_[static_cast<std::size_t>(GameSound::PoppetCast)] = MakeTone(410.0f, 170.0f, 0.10f, 0.30f, 0.52f);
	sounds_[static_cast<std::size_t>(GameSound::BottleCast)] = MakeTone(170.0f, 520.0f, 0.16f, 0.18f, 0.48f);
	sounds_[static_cast<std::size_t>(GameSound::AppleCast)] = MakeTone(230.0f, 95.0f, 0.30f, 0.08f, 0.42f);
	sounds_[static_cast<std::size_t>(GameSound::SpiderCast)] = MakeTone(620.0f, 105.0f, 0.18f, 0.42f, 0.45f);
	sounds_[static_cast<std::size_t>(GameSound::HexImpact)] = MakeTone(105.0f, 42.0f, 0.19f, 0.55f, 0.58f);
	sounds_[static_cast<std::size_t>(GameSound::Pickup)] = MakeTone(480.0f, 880.0f, 0.14f, 0.05f, 0.38f);
	sounds_[static_cast<std::size_t>(GameSound::Hurt)] = MakeTone(92.0f, 48.0f, 0.24f, 0.68f, 0.62f);
	sounds_[static_cast<std::size_t>(GameSound::WardRaise)] = MakeTone(180.0f, 760.0f, 0.22f, 0.08f, 0.38f);
	sounds_[static_cast<std::size_t>(GameSound::WardImpact)] = MakeTone(920.0f, 320.0f, 0.11f, 0.12f, 0.42f);
	sounds_[static_cast<std::size_t>(GameSound::EnemyDeath)] = MakeTone(150.0f, 34.0f, 0.34f, 0.45f, 0.55f);
	sounds_[static_cast<std::size_t>(GameSound::NewWave)] = MakeTone(72.0f, 210.0f, 0.52f, 0.16f, 0.48f);
	ready_ = true;
}

void AudioFeedback::shutdown() {
	if (!ready_) {
		return;
	}
	for (Sound& sound : sounds_) {
		if (sound.frameCount > 0) {
			UnloadSound(sound);
			sound = {};
		}
	}
	ready_ = false;
}

void AudioFeedback::play(GameSound sound) {
	if (!ready_) {
		return;
	}
	Sound& cue = sounds_[static_cast<std::size_t>(sound)];
	if (cue.frameCount > 0) {
		PlaySound(cue);
	}
}

bool AudioFeedback::isReady() const {
	return ready_;
}
