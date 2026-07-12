#pragma once

#include <vector>
#include "Object.hxx"
#include "CurseVector.hxx"

class Player;
class Enemy;

enum class EffectKind {
	Projectile,
	Pulse,
	Mine,
	Hazard
};

class Effect : public Object {
public:
	Effect();

	static Effect MakeProjectile(Vector3 position, Vector3 direction, const SpellPreview& spell);
	static Effect MakePulse(Vector3 position, const SpellPreview& spell);
	static Effect MakeMine(Vector3 position, const SpellPreview& spell);
	static Effect MakeHazard(Vector3 position, const SpellPreview& spell, float radius, float duration);

	void update(float dt) override;
	void tick(float dt, Player& player, std::vector<Enemy>& enemies, const std::vector<BoundingBox>& blockers);
	void draw3D(const Camera3D& camera) const override;

	bool hasExpired() const;
private:
	void detonate(std::vector<Enemy>& enemies, Player& player, bool allowHazard);
	void applyArea(std::vector<Enemy>& enemies, Player& player);
	Vector3 nearestEnemyDirection(const std::vector<Enemy>& enemies) const;
	void collideWithWorld(const std::vector<BoundingBox>& blockers, bool& hitWall);

	EffectKind kind {EffectKind::Pulse};
	SpellPreview spell {};
	Vector3 direction {};
	float speed {0.0f};
	float remainingTime {0.0f};
	float triggerRadius {0.0f};
	float pulseRadius {0.0f};
	float armDelay {0.0f};
	bool triggered {false};
	bool appliedPulse {false};
};
