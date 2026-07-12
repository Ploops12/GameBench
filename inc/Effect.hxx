#pragma once

#include "CurseVector.hxx"
#include "Object.hxx"

class Effect : public Object {
public:
	Effect(Vector3 position, float radius, float lifetime, SpellStats payload = {});
	void update(float dt) override;
	[[nodiscard]] bool expired() const;
	[[nodiscard]] float age() const;
	[[nodiscard]] float lifetime() const;
	[[nodiscard]] const SpellStats& payload() const;
	[[nodiscard]] bool contains(Vector3 point, float targetRadius = 0.0f) const;

protected:
	SpellStats payload_ {};
	float lifetime_ {0.0f};
	float age_ {0.0f};
};

class ProjectileEffect final : public Effect {
public:
	ProjectileEffect(Vector3 position, Vector3 velocity, SpellStats payload);
	void updateProjectile(float dt, Vector3 homingTarget, bool hasTarget);
	[[nodiscard]] Vector3 velocity() const;
	[[nodiscard]] Vector3 previousPosition() const;

protected:
	void onDraw(const Camera3D& camera) const override;

private:
	Vector3 velocity_ {};
	Vector3 previousPosition_ {};
};

class TrapEffect final : public Effect {
public:
	TrapEffect(Vector3 position, SpellStats payload);
	void update(float dt) override;

protected:
	void onDraw(const Camera3D& camera) const override;
};

class HexFieldEffect final : public Effect {
public:
	HexFieldEffect(Vector3 position, SpellStats payload, float lifetime, bool followsPlayer);
	void updateField(float dt, Vector3 playerPosition);
	[[nodiscard]] bool takePulse();
	[[nodiscard]] bool followsPlayer() const;

protected:
	void onDraw(const Camera3D& camera) const override;

private:
	float pulseAccumulator_ {0.0f};
	bool followsPlayer_ {false};
};

class BurstEffect final : public Effect {
public:
	BurstEffect(Vector3 position, float radius, Color color);

protected:
	void onDraw(const Camera3D& camera) const override;

private:
	Color color_ {WHITE};
};

class TraceEffect final : public Effect {
public:
	TraceEffect(Vector3 start, Vector3 end, Color color);

protected:
	void onDraw(const Camera3D& camera) const override;

private:
	Vector3 end_ {};
	Color color_ {WHITE};
};
