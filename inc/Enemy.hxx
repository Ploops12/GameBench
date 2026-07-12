#pragma once

#include <raylib.h>

enum class EnemyKind {
	TorchMob,
	Musketeer,
	WardEngineer
};

struct Enemy {
	EnemyKind kind{EnemyKind::TorchMob};
	Vector3 position{};
	Vector3 velocity{};
	float radius{0.45f};
	float health{35.0f};
	float maxHealth{35.0f};
	float attackCooldown{0.0f};
	float telegraphTimer{0.0f};
	float snareTimer{0.0f};
	float fearTimer{0.0f};
	float poisonTimer{0.0f};
	float poisonDps{0.0f};
	bool alive{true};
	bool droppedLoot{false};
};
