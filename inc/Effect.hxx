#pragma once

#include <vector>

#include <raylib.h>

#include "CurseVector.hxx"

class Enemy;

enum class EffectKind {
	ThrownBottle,
	PoppetNeedle,
	PlacedSpider,
	Burst,
	LingeringHazard,
	SelfHexAura
};

class Effect {
public:
	static Effect makeThrownBottle(Vector3 position, Vector3 direction, const SpellStats& spell);
	static Effect makePoppetNeedle(Vector3 position, Vector3 direction, const SpellStats& spell);
	static Effect makePlacedSpider(Vector3 position, const SpellStats& spell);
	static Effect makeBurst(Vector3 position, const SpellStats& spell);
	static Effect makeSelfHexAura(Vector3 position, const SpellStats& spell);

	void update(float deltaTime, const std::vector<BoundingBox>& solidBounds, std::vector<Enemy>& enemies);
	void draw(const Camera3D& camera) const;
	bool isActive() const;
	EffectKind getKind() const;
	const Vector3& getPosition() const;

private:
	Effect(EffectKind kind, Vector3 position, Vector3 velocity, SpellStats spell,
		float remainingLife, float armingTime = 0.0f);

	void explode(std::vector<Enemy>& enemies);
	void applyAt(std::vector<Enemy>& enemies, float strength);
	Enemy* findHomingTarget(std::vector<Enemy>& enemies) const;
	bool touchesWorld(const std::vector<BoundingBox>& solidBounds) const;

	EffectKind kind;
	Vector3 position {};
	Vector3 velocity {};
	SpellStats spell {};
	float remainingLife {0.0f};
	float armingTime {0.0f};
	float tickTimer {0.0f};
	float visualAge {0.0f};
	bool burstApplied {false};
	bool active {true};
};
