#include "Enemy.hxx"

#include <algorithm>
#include <cmath>

namespace {

constexpr float TWO_PI = 6.28318530718f;
constexpr int RING_SEGMENTS = 24;

float horizontalLength(Vector3 value) {
	return std::sqrt(value.x * value.x + value.z * value.z);
}

Vector3 horizontalDirection(Vector3 from, Vector3 to) {
	const Vector3 difference {to.x - from.x, 0.0f, to.z - from.z};
	const float length = horizontalLength(difference);
	if (length <= 0.0001f) return {};
	return {difference.x / length, 0.0f, difference.z / length};
}

Vector3 directionTo(Vector3 from, Vector3 to) {
	const Vector3 difference {to.x - from.x, to.y - from.y, to.z - from.z};
	const float length = std::sqrt(difference.x * difference.x + difference.y * difference.y +
		difference.z * difference.z);
	if (length <= 0.0001f) return {0.0f, 0.0f, 1.0f};
	return {difference.x / length, difference.y / length, difference.z / length};
}

Color scaledAlpha(Color color, float scale) {
	color.a = static_cast<unsigned char>(static_cast<float>(color.a) *
		std::clamp(scale, 0.0f, 1.0f));
	return color;
}

void drawGroundRing(Vector3 centre, float radius, Color color) {
	centre.y += 0.035f;
	Vector3 previous {
		centre.x + radius,
		centre.y,
		centre.z
	};
	for (int segment = 1; segment <= RING_SEGMENTS; ++segment) {
		const float angle = TWO_PI * static_cast<float>(segment) /
			static_cast<float>(RING_SEGMENTS);
		const Vector3 next {
			centre.x + std::cos(angle) * radius,
			centre.y,
			centre.z + std::sin(angle) * radius
		};
		DrawLine3D(previous, next, color);
		previous = next;
	}
}

Vector3 cameraRight(const Camera3D& camera) {
	const Vector3 forward = horizontalDirection(camera.position, camera.target);
	if (horizontalLength(forward) <= 0.0001f) return {1.0f, 0.0f, 0.0f};
	return {forward.z, 0.0f, -forward.x};
}

Vector3 offset(Vector3 point, Vector3 direction, float amount) {
	return {
		point.x + direction.x * amount,
		point.y + direction.y * amount,
		point.z + direction.z * amount
	};
}

void drawFallbackHunter(const Camera3D& camera, Vector3 position, Vector3 size,
	EnemyKind kind, Color color) {
	const Vector3 right = cameraRight(camera);
	const float bottom = position.y - size.y * 0.5f;
	const Vector3 head {position.x, bottom + size.y * 0.78f, position.z};
	const Vector3 shoulders {position.x, bottom + size.y * 0.61f, position.z};
	const Vector3 hips {position.x, bottom + size.y * 0.27f, position.z};
	const Vector3 leftShoulder = offset(shoulders, right, -size.x * 0.48f);
	const Vector3 rightShoulder = offset(shoulders, right, size.x * 0.48f);
	const Vector3 leftFoot = offset({position.x, bottom, position.z}, right, -size.x * 0.30f);
	const Vector3 rightFoot = offset({position.x, bottom, position.z}, right, size.x * 0.30f);

	DrawSphereWires(head, size.x * 0.22f, 5, 7, color);
	DrawLine3D(leftShoulder, rightShoulder, color);
	DrawLine3D(leftShoulder, hips, color);
	DrawLine3D(rightShoulder, hips, color);
	DrawLine3D(hips, leftFoot, color);
	DrawLine3D(hips, rightFoot, color);

	if (kind == EnemyKind::TorchBearer) {
		const Vector3 torchBottom = offset(leftShoulder, right, -size.x * 0.20f);
		const Vector3 torchTop {torchBottom.x, head.y + size.y * 0.36f, torchBottom.z};
		DrawLine3D(torchBottom, torchTop, ORANGE);
		DrawSphereWires(torchTop, 0.12f, 4, 6, GOLD);
	} else {
		const Vector3 weaponLeft = offset(shoulders, right, -size.x * 0.58f);
		const Vector3 weaponRight = offset(shoulders, right, size.x * 0.58f);
		DrawLine3D(weaponLeft, weaponRight, SKYBLUE);
		DrawLine3D(offset(shoulders, {0.0f, 1.0f, 0.0f}, -0.12f),
			offset(shoulders, {0.0f, 1.0f, 0.0f}, 0.12f), SKYBLUE);
	}
}

} // namespace

Enemy::Enemy(EnemyKind kind, Vector3 position, ReagentType dropType,
	const Texture2D* spriteSheet)
	: Actor(position,
		kind == EnemyKind::TorchBearer ? Vector3 {0.82f, 1.75f, 0.82f} :
			Vector3 {0.9f, 1.9f, 0.9f},
		kind == EnemyKind::TorchBearer ? 52.0f : 72.0f),
	  kind(kind),
	  dropType(static_cast<std::size_t>(dropType) < REAGENT_TYPE_COUNT ? dropType :
		  ReagentType::BoneShards),
	  spriteSheet(spriteSheet) {
	moveSpeed = kind == EnemyKind::TorchBearer ? 3.35f : 2.35f;
	attackCooldown = kind == EnemyKind::TorchBearer ? 0.35f : 0.75f;
	setCollidable(true);
}

EnemyAttackEvent Enemy::updateAI(float deltaTime, Vector3 playerPosition,
	const std::vector<BoundingBox>& solidBounds) {
	EnemyAttackEvent event;
	if (!isActive() || deltaTime <= 0.0f) return event;

	Actor::update(deltaTime);
	animationTime += deltaTime;
	attackCooldown = std::max(0.0f, attackCooldown - deltaTime);

	if (!isAlive()) {
		velocity = {};
		attackPending = false;
		telegraphTime = 0.0f;
		return event;
	}

	const Vector3 towardPlayer = horizontalDirection(position, playerPosition);
	const float distance = horizontalLength({
		playerPosition.x - position.x,
		0.0f,
		playerPosition.z - position.z
	});

	if (getFearTime() > 0.0f) {
		if (attackPending) attackCooldown = std::max(attackCooldown, 0.65f);
		attackPending = false;
		telegraphTime = 0.0f;
		velocity = {
			-towardPlayer.x * moveSpeed * 1.18f * getMovementMultiplier(),
			0.0f,
			-towardPlayer.z * moveSpeed * 1.18f * getMovementMultiplier()
		};
	} else if (attackPending) {
		velocity = {};
		telegraphTime = std::max(0.0f, telegraphTime - deltaTime);
		if (telegraphTime <= 0.0f) {
			attackPending = false;
			const Vector3 origin {
				position.x,
				position.y + (kind == EnemyKind::TorchBearer ? 0.18f : 0.28f),
				position.z
			};

			if (kind == EnemyKind::TorchBearer) {
				attackCooldown = 0.82f;
				if (distance <= 2.25f) {
					event.valid = true;
					event.projectile = false;
					event.origin = origin;
					event.direction = directionTo(origin, playerPosition);
					event.damage = 11.0f;
				}
			} else {
				attackCooldown = 1.35f;
				event.valid = true;
				event.projectile = true;
				event.origin = origin;
				event.direction = directionTo(origin, playerPosition);
				event.damage = 13.0f;
			}
		}
	} else {
		velocity = {};
		if (kind == EnemyKind::TorchBearer) {
			if (distance <= 2.0f && attackCooldown <= 0.0f) {
				attackPending = true;
				telegraphTime = 0.34f;
			} else if (distance > 1.85f) {
				velocity = {
					towardPlayer.x * moveSpeed * getMovementMultiplier(),
					0.0f,
					towardPlayer.z * moveSpeed * getMovementMultiplier()
				};
			}
		} else {
			if (distance <= 13.5f && distance >= 1.6f && attackCooldown <= 0.0f) {
				attackPending = true;
				telegraphTime = 0.68f;
			} else if (distance > 9.5f) {
				velocity = {
					towardPlayer.x * moveSpeed * getMovementMultiplier(),
					0.0f,
					towardPlayer.z * moveSpeed * getMovementMultiplier()
				};
			} else if (distance < 4.2f) {
				velocity = {
					-towardPlayer.x * moveSpeed * getMovementMultiplier(),
					0.0f,
					-towardPlayer.z * moveSpeed * getMovementMultiplier()
				};
			} else {
				const float strafeSign = (static_cast<std::size_t>(dropType) % 2 == 0) ? 1.0f : -1.0f;
				velocity = {
					-towardPlayer.z * moveSpeed * 0.28f * strafeSign * getMovementMultiplier(),
					0.0f,
					towardPlayer.x * moveSpeed * 0.28f * strafeSign * getMovementMultiplier()
				};
			}
		}
	}

	if (horizontalLength(velocity) > 0.0001f) {
		Vector3 moved = position;
		Vector3 candidate = moved;
		candidate.x += velocity.x * deltaTime;
		if (!collidesAt(candidate, solidBounds)) moved.x = candidate.x;

		candidate = moved;
		candidate.z += velocity.z * deltaTime;
		if (!collidesAt(candidate, solidBounds)) moved.z = candidate.z;
		position = moved;
	}

	return event;
}

void Enemy::draw3D(const Camera3D& camera) const {
	if (!isVisible() || !isActive() || !isAlive()) return;

	Color tint = kind == EnemyKind::TorchBearer ? Color {255, 226, 184, 255} :
		Color {205, 226, 255, 255};
	if (rotTime > 0.0f) tint = Color {194, 218, 104, 255};
	if (snareTime > 0.0f) tint = Color {150, 236, 117, 255};
	if (getFearTime() > 0.0f) tint = Color {228, 145, 245, 255};
	if (getHurtTime() > 0.0f) tint = Color {255, 112, 102, 255};

	const bool moving = horizontalLength(velocity) > 0.12f;
	const int row = attackPending ? 2 : (moving ? 1 : 0);
	const int frame = static_cast<int>(animationTime * (attackPending ? 11.0f : 8.0f)) % 8;
	if (spriteSheet != nullptr && spriteSheet->id != 0 && spriteSheet->width > 0 &&
		spriteSheet->height > 0) {
		const float frameWidth = static_cast<float>(spriteSheet->width) / 8.0f;
		const float frameHeight = static_cast<float>(spriteSheet->height) / 3.0f;
		const Rectangle source {
			frameWidth * static_cast<float>(frame),
			frameHeight * static_cast<float>(row),
			frameWidth,
			frameHeight
		};
		const Vector2 billboardSize = kind == EnemyKind::TorchBearer ?
			Vector2 {1.72f, 2.45f} : Vector2 {1.82f, 2.52f};
		Vector3 billboardPosition = position;
		billboardPosition.y += (billboardSize.y - size.y) * 0.5f;
		DrawBillboardRec(camera, *spriteSheet, source, billboardPosition, billboardSize, tint);
	} else {
		drawFallbackHunter(camera, position, size, kind, tint);
	}

	const Vector3 feet {position.x, position.y - size.y * 0.5f, position.z};
	if (attackPending) {
		const float pulse = 0.5f + 0.5f * std::sin(animationTime * 22.0f);
		const Color warning = kind == EnemyKind::TorchBearer ? ORANGE : SKYBLUE;
		drawGroundRing(feet, getCollisionRadius() + 0.18f + pulse * 0.12f,
			scaledAlpha(warning, 0.65f + pulse * 0.35f));
		DrawSphereWires({position.x, position.y + size.y * 0.42f, position.z},
			0.16f + pulse * 0.08f, 4, 7, warning);
	}
	if (getFearTime() > 0.0f) {
		drawGroundRing(feet, getCollisionRadius() + 0.12f, scaledAlpha(PURPLE, 0.8f));
	}

	if (getHealthRatio() < 0.999f) {
		const Vector3 right = cameraRight(camera);
		const Vector3 barCentre {position.x, position.y + size.y * 0.68f, position.z};
		const Vector3 barStart = offset(barCentre, right, -0.42f);
		const Vector3 barEnd = offset(barCentre, right, 0.42f);
		const Vector3 healthEnd = offset(barStart, right, 0.84f * getHealthRatio());
		DrawLine3D(barStart, barEnd, Color {45, 24, 28, 255});
		DrawLine3D(barStart, healthEnd, Color {226, 65, 75, 255});
	}
}

EnemyKind Enemy::getKind() const {
	return kind;
}

ReagentType Enemy::getDropType() const {
	return dropType;
}

bool Enemy::claimDrop() {
	if (isAlive() || dropClaimed) return false;
	dropClaimed = true;
	return true;
}

float Enemy::getCollisionRadius() const {
	return std::max(size.x, size.z) * 0.46f;
}

bool Enemy::collidesAt(Vector3 candidate, const std::vector<BoundingBox>& solidBounds) const {
	const Vector3 halfSize {size.x * 0.5f, size.y * 0.5f, size.z * 0.5f};
	const BoundingBox candidateBounds {
		{candidate.x - halfSize.x, candidate.y - halfSize.y, candidate.z - halfSize.z},
		{candidate.x + halfSize.x, candidate.y + halfSize.y, candidate.z + halfSize.z}
	};
	for (const BoundingBox& solid : solidBounds) {
		if (CheckCollisionBoxes(candidateBounds, solid)) return true;
	}
	return false;
}
