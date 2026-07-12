#include "Game.h"
#include "Animation.hxx"
#include "MainMenu.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#include <raymath.h>
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#include <utility>

namespace {
constexpr float PI_F = 3.1415926535f;
constexpr float PLAYER_RADIUS = 0.34f;
constexpr float GROUND_Y = 0.0f;
constexpr float GRAVITY = 23.0f;
constexpr float WALK_SPEED = 6.3f;
constexpr float SPRINT_SPEED = 9.1f;
constexpr float RUMMAGE_SPEED_FACTOR = 0.48f;

float clamp01(float value) {
	return std::clamp(value, 0.0f, 1.0f);
}

float distanceXZ(Vector3 a, Vector3 b) {
	const float dx = a.x - b.x;
	const float dz = a.z - b.z;
	return std::sqrt(dx * dx + dz * dz);
}

Vector3 flatten(Vector3 value) {
	value.y = 0.0f;
	return value;
}

Vector3 safeNormalize(Vector3 value) {
	const float length = Vector3Length(value);
	if (length <= 0.0001f) return {};
	return Vector3Scale(value, 1.0f / length);
}

Color lineWhite() {
	return Color{222, 239, 222, 255};
}

Color wardBlue() {
	return Color{82, 206, 255, 255};
}

Color curseGreen() {
	return Color{108, 247, 151, 255};
}

void drawBar(int x, int y, int width, int height, float ratio, Color color, const char* label) {
	DrawRectangleLines(x, y, width, height, lineWhite());
	DrawRectangle(x + 2, y + 2, static_cast<int>((width - 4) * clamp01(ratio)), height - 4, color);
	DrawText(label, x + 6, y + 4, 12, lineWhite());
}

void drawScreenPanel(Rectangle rec, Color color) {
	DrawRectangleRec(rec, Fade(Color{0, 0, 0, 255}, 0.72f));
	DrawRectangleLinesEx(rec, 2.0f, color);
}

void drawBillboardCard(const Camera3D& camera, Vector3 center, float width, float height, Color fill, Color outline) {
	Vector3 toCamera = flatten(Vector3Subtract(camera.position, center));
	if (Vector3Length(toCamera) < 0.001f) toCamera = {0.0f, 0.0f, 1.0f};
	toCamera = Vector3Normalize(toCamera);
	Vector3 right = Vector3Normalize(Vector3CrossProduct({0.0f, 1.0f, 0.0f}, toCamera));
	Vector3 up{0.0f, 1.0f, 0.0f};
	Vector3 left = Vector3Scale(right, -width * 0.5f);
	Vector3 rght = Vector3Scale(right, width * 0.5f);
	Vector3 bottom = Vector3Scale(up, -height * 0.5f);
	Vector3 top = Vector3Scale(up, height * 0.5f);
	Vector3 p1 = Vector3Add(Vector3Add(center, left), bottom);
	Vector3 p2 = Vector3Add(Vector3Add(center, rght), bottom);
	Vector3 p3 = Vector3Add(Vector3Add(center, rght), top);
	Vector3 p4 = Vector3Add(Vector3Add(center, left), top);

	DrawTriangle3D(p1, p2, p3, fill);
	DrawTriangle3D(p1, p3, p4, fill);
	DrawLine3D(p1, p2, outline);
	DrawLine3D(p2, p3, outline);
	DrawLine3D(p3, p4, outline);
	DrawLine3D(p4, p1, outline);
	DrawLine3D(Vector3Add(center, Vector3Scale(left, 0.55f)), Vector3Add(center, Vector3Scale(rght, 0.55f)), outline);
	DrawLine3D(Vector3Add(Vector3Add(center, Vector3Scale(left, 0.35f)), Vector3Scale(up, -0.45f)), Vector3Add(center, Vector3Scale(up, 0.35f)), outline);
	DrawLine3D(Vector3Add(center, Vector3Scale(up, 0.35f)), Vector3Add(Vector3Add(center, Vector3Scale(rght, 0.35f)), Vector3Scale(up, -0.45f)), outline);
}

void drawHands(const Player& player, bool rummaging) {
	const int sw = GetScreenWidth();
	const int sh = GetScreenHeight();
	const float pulse = std::sin(GetTime() * 11.0) * 5.0f + player.castFlash * 18.0f;
	Color hand = rummaging ? Color{250, 210, 128, 255} : lineWhite();
	Color shadow = Color{28, 36, 32, 210};

	Vector2 leftPalm{sw * 0.24f, sh - 72.0f - pulse};
	Vector2 rightPalm{sw * 0.76f, sh - 72.0f - pulse};
	DrawCircleV(leftPalm, 25.0f, shadow);
	DrawCircleLines(static_cast<int>(leftPalm.x), static_cast<int>(leftPalm.y), 25.0f, hand);
	DrawCircleV(rightPalm, 25.0f, shadow);
	DrawCircleLines(static_cast<int>(rightPalm.x), static_cast<int>(rightPalm.y), 25.0f, hand);
	for (int i = 0; i < 5; ++i) {
		const float offset = (i - 2) * 9.0f;
		DrawLineEx({leftPalm.x + offset, leftPalm.y - 18.0f}, {leftPalm.x + offset - 18.0f, leftPalm.y - 58.0f - i * 3.0f}, 3.0f, hand);
		DrawLineEx({rightPalm.x + offset, rightPalm.y - 18.0f}, {rightPalm.x + offset + 18.0f, rightPalm.y - 58.0f - i * 3.0f}, 3.0f, hand);
	}

	if (player.isWardActive()) {
		DrawCircleLines(sw / 2, sh / 2, 74.0f + std::sin(GetTime() * 9.0) * 3.0f, wardBlue());
		DrawCircleLines(sw / 2, sh / 2, 92.0f, Fade(wardBlue(), 0.55f));
	}
}

void drawReticle(const Player& player) {
	const int cx = GetScreenWidth() / 2;
	const int cy = GetScreenHeight() / 2;
	const int spread = 9 + static_cast<int>(player.castFlash * 8.0f);
	Color color = player.isWardActive() ? wardBlue() : curseGreen();
	DrawLine(cx - spread - 7, cy, cx - spread, cy, color);
	DrawLine(cx + spread, cy, cx + spread + 7, cy, color);
	DrawLine(cx, cy - spread - 7, cx, cy - spread, color);
	DrawLine(cx, cy + spread, cx, cy + spread + 7, color);
	DrawCircleLines(cx, cy, 3.0f, color);
}
}

Object::Object(Vector3 startPosition, float radius) : position(startPosition), collisionRadius(radius) {}

void Object::update(float) {}

void Object::draw(const Camera3D&) const {}

Vector3 Object::getPosition() const {
	return position;
}

void Object::setPosition(Vector3 value) {
	position = value;
}

float Object::getCollisionRadius() const {
	return collisionRadius;
}

void Object::setCollisionRadius(float value) {
	collisionRadius = value;
}

bool Object::isActive() const {
	return active;
}

void Object::setActive(bool value) {
	active = value;
}

bool Object::isDrawable() const {
	return drawable;
}

void Object::setDrawable(bool value) {
	drawable = value;
}

bool Object::hasCollision() const {
	return collisionEnabled;
}

void Object::setCollisionEnabled(bool value) {
	collisionEnabled = value;
}

bool Object::overlaps(const Object& other) const {
	return active && other.active && collisionEnabled && other.collisionEnabled &&
		Vector3Distance(position, other.position) <= collisionRadius + other.collisionRadius;
}

Actor::Actor(Vector3 startPosition, float radius, float actorMaxHealth) :
	Object(startPosition, radius),
	health(actorMaxHealth),
	maxHealth(actorMaxHealth) {}

void Actor::update(float dt) {
	position = Vector3Add(position, Vector3Scale(velocity, dt));
}

void Actor::applyDamage(float amount) {
	health = std::max(0.0f, health - amount);
	if (health <= 0.0f) active = false;
}

void Actor::heal(float amount) {
	health = std::min(maxHealth, health + amount);
}

bool Actor::isAlive() const {
	return active && health > 0.0f;
}

const ReagentDefinition& reagentDefinition(ReagentType type) {
	static const std::array<ReagentDefinition, REAGENT_COUNT> definitions{{
		{ReagentType::GraveSalt, "Grave Salt", "Salt", "Z", "+salt-burn damage and bite radius", Color{222, 228, 196, 255}},
		{ReagentType::SaintAsh, "Saint Ash", "Ash", "X", "+ward stolen from the curse", Color{178, 193, 202, 255}},
		{ReagentType::BlackWax, "Black Wax", "Wax", "C", "+rot and lingering floor hex", Color{92, 80, 132, 255}},
		{ReagentType::WidowNettle, "Widow Nettle", "Nettle", "V", "+thorn snare duration", Color{90, 214, 92, 255}},
		{ReagentType::MothDust, "Moth Dust", "Moth", "B", "+panic/fear duration", Color{214, 184, 96, 255}},
		{ReagentType::BellBronze, "Bell Bronze", "Bronze", "N", "+seeking and resonant radius", Color{229, 136, 62, 255}},
	}};
	return definitions[reagentIndex(type)];
}

ReagentType reagentFromIndex(int index) {
	return static_cast<ReagentType>(std::clamp(index, 0, REAGENT_COUNT - 1));
}

int reagentIndex(ReagentType type) {
	return std::clamp(static_cast<int>(type), 0, REAGENT_COUNT - 1);
}

std::string reagentListText(const std::array<int, REAGENT_COUNT>& counts) {
	std::string text;
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		if (counts[i] <= 0) continue;
		if (!text.empty()) text += ", ";
		text += reagentDefinition(reagentFromIndex(i)).shortName;
		if (counts[i] > 1) text += "x" + std::to_string(counts[i]);
	}
	return text.empty() ? "none" : text;
}

const BaseVectorDefinition& baseVectorDefinition(BaseVector type) {
	static const std::array<BaseVectorDefinition, static_cast<int>(BaseVector::Count)> definitions{{
		{BaseVector::Bottle, "Bottle", "1", "thrown glass curse projectile", Color{104, 235, 169, 255}},
		{BaseVector::Apple, "Apple", "2", "bite for self-centered hex/ward", Color{238, 68, 83, 255}},
		{BaseVector::Spider, "Spider", "3", "place a skittering mine/trap", Color{190, 120, 255, 255}},
		{BaseVector::Poppet, "Poppet", "4", "always-ready weak pin curse", Color{238, 226, 164, 255}},
	}};
	return definitions[std::clamp(static_cast<int>(type), 0, static_cast<int>(BaseVector::Count) - 1)];
}

BaseVector baseVectorFromIndex(int index) {
	return static_cast<BaseVector>(std::clamp(index, 0, static_cast<int>(BaseVector::Count) - 1));
}

void CurseSpell::setBase(BaseVector value) {
	base = value;
	if (base == BaseVector::Poppet) reagents.clear();
}

BaseVector CurseSpell::getBase() const {
	return base;
}

bool CurseSpell::addReagent(ReagentType type, const ReagentInventory& inventory) {
	if (base == BaseVector::Poppet) base = BaseVector::Bottle;
	if (static_cast<int>(reagents.size()) >= MAX_REAGENTS) return false;
	const auto selected = selectedCounts();
	const int index = reagentIndex(type);
	if (selected[index] >= inventory[index]) return false;
	reagents.push_back(type);
	return true;
}

void CurseSpell::clearReagents() {
	reagents.clear();
}

const std::vector<ReagentType>& CurseSpell::getReagents() const {
	return reagents;
}

bool CurseSpell::canAfford(const ReagentInventory& inventory) const {
	const auto selected = selectedCounts();
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		if (selected[i] > inventory[i]) return false;
	}
	return true;
}

void CurseSpell::consume(ReagentInventory& inventory) const {
	const auto selected = selectedCounts();
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		inventory[i] = std::max(0, inventory[i] - selected[i]);
	}
}

std::array<int, REAGENT_COUNT> CurseSpell::selectedCounts() const {
	std::array<int, REAGENT_COUNT> counts{};
	for (ReagentType reagent : reagents) {
		++counts[reagentIndex(reagent)];
	}
	return counts;
}

SpellStats CurseSpell::stats() const {
	SpellStats result{};
	result.radius = 0.65f;
	result.projectileSpeed = 16.0f;
	result.mineArmingSeconds = 0.35f;

	switch (base) {
	case BaseVector::Bottle:
		result.damage = 24.0f;
		result.radius = 0.85f;
		result.projectileSpeed = 18.5f;
		break;
	case BaseVector::Apple:
		result.damage = 8.0f;
		result.radius = 2.0f;
		result.wardGain = 12.0f;
		break;
	case BaseVector::Spider:
		result.damage = 18.0f;
		result.radius = 1.45f;
		result.mineArmingSeconds = 0.55f;
		break;
	case BaseVector::Poppet:
		result.damage = 11.0f;
		result.radius = 0.0f;
		result.projectileSpeed = 0.0f;
		break;
	case BaseVector::Count:
		break;
	}

	const auto counts = selectedCounts();
	result.damage += counts[reagentIndex(ReagentType::GraveSalt)] * 17.0f;
	result.radius += counts[reagentIndex(ReagentType::GraveSalt)] * 0.12f;
	result.wardGain += counts[reagentIndex(ReagentType::SaintAsh)] * 18.0f;
	result.rotSeconds += counts[reagentIndex(ReagentType::BlackWax)] * 3.0f;
	result.rotDps += counts[reagentIndex(ReagentType::BlackWax)] * 7.0f;
	result.lingerSeconds += counts[reagentIndex(ReagentType::BlackWax)] * 2.8f;
	result.snareSeconds += counts[reagentIndex(ReagentType::WidowNettle)] * 1.65f;
	result.fearSeconds += counts[reagentIndex(ReagentType::MothDust)] * 1.45f;
	result.homingStrength += counts[reagentIndex(ReagentType::BellBronze)] * 3.5f;
	result.radius += counts[reagentIndex(ReagentType::BellBronze)] * 0.32f;
	result.projectileSpeed += counts[reagentIndex(ReagentType::BellBronze)] * 1.3f;

	if (base == BaseVector::Apple) {
		result.damage *= 0.65f;
		result.radius += 0.65f;
		result.wardGain += 8.0f;
	}

	return result;
}

std::string CurseSpell::previewName() const {
	std::string result = baseVectorDefinition(base).name;
	const auto counts = selectedCounts();
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		for (int j = 0; j < counts[i]; ++j) {
			result += j == 0 ? " + " : " + more ";
			result += reagentDefinition(reagentFromIndex(i)).shortName;
		}
	}
	return result;
}

std::string CurseSpell::previewDetails() const {
	const SpellStats s = stats();
	char buffer[256];
	std::snprintf(buffer, sizeof(buffer), "dmg %.0f  rad %.1f  ward +%.0f  rot %.0fs  snare %.0fs  fear %.0fs  seek %.0f",
		s.damage, s.radius, s.wardGain, s.rotSeconds, s.snareSeconds, s.fearSeconds, s.homingStrength);
	return buffer;
}

Enemy::Enemy(Vector3 startPosition, std::string label, float enemyHealth, float enemySpeed, float enemyDamage) :
	Actor(startPosition, 0.48f, enemyHealth),
	name(std::move(label)),
	speed(enemySpeed),
	damage(enemyDamage) {}

void Enemy::updateToward(Vector3 playerPosition, float dt) {
	if (!isAlive()) return;
	if (attackCooldown > 0.0f) attackCooldown = std::max(0.0f, attackCooldown - dt);
	tickStatus(dt);

	Vector3 toPlayer = Vector3Subtract(playerPosition, position);
	toPlayer.y = 0.0f;
	const float distance = Vector3Length(toPlayer);
	if (distance < 0.001f) return;
	Vector3 direction = Vector3Scale(toPlayer, 1.0f / distance);
	if (fearSeconds > 0.0f) direction = Vector3Scale(direction, -1.0f);
	const float slowFactor = snareSeconds > 0.0f ? 0.28f : 1.0f;
	if (distance > attackRange * 0.82f || fearSeconds > 0.0f) {
		position = Vector3Add(position, Vector3Scale(direction, speed * slowFactor * dt));
	}
}

void Enemy::draw(const Camera3D& camera) const {
	if (!isAlive()) return;
	Color fill = fearSeconds > 0.0f ? Fade(Color{130, 80, 255, 255}, 0.38f) : Fade(Color{140, 20, 24, 255}, 0.42f);
	Color outline = snareSeconds > 0.0f ? Color{88, 255, 104, 255} : Color{255, 68, 68, 255};
	if (rotSeconds > 0.0f) outline = Color{120, 76, 255, 255};
	drawBillboardCard(camera, Vector3Add(position, {0.0f, 1.0f, 0.0f}), 0.85f, 1.85f, fill, outline);

	Vector3 toCamera = flatten(Vector3Subtract(camera.position, position));
	if (Vector3Length(toCamera) < 0.001f) toCamera = {0.0f, 0.0f, 1.0f};
	Vector3 right = Vector3Normalize(Vector3CrossProduct({0.0f, 1.0f, 0.0f}, Vector3Normalize(toCamera)));
	Vector3 barCenter = Vector3Add(position, {0.0f, 2.05f, 0.0f});
	Vector3 barLeft = Vector3Add(barCenter, Vector3Scale(right, -0.42f));
	Vector3 barRight = Vector3Add(barCenter, Vector3Scale(right, 0.42f));
	const float ratio = maxHealth <= 0.0f ? 0.0f : health / maxHealth;
	Vector3 barFill = Vector3Lerp(barLeft, barRight, clamp01(ratio));
	DrawLine3D(barLeft, barRight, Color{45, 20, 24, 255});
	DrawLine3D(barLeft, barFill, outline);
	if (snareSeconds > 0.0f || fearSeconds > 0.0f || rotSeconds > 0.0f) {
		DrawCircle3D(Vector3Add(barCenter, {0.0f, 0.1f, 0.0f}), 0.16f, {1.0f, 0.0f, 0.0f}, 90.0f, outline);
	}
}

bool Enemy::canAttack(Vector3 playerPosition) const {
	return isAlive() && attackCooldown <= 0.0f && fearSeconds <= 0.0f && distanceXZ(position, playerPosition) <= attackRange;
}

float Enemy::consumeAttackDamage() {
	attackCooldown = attackPeriod;
	return damage;
}

void Enemy::applyStatus(float snare, float fear, float dps, float seconds) {
	snareSeconds = std::max(snareSeconds, snare);
	fearSeconds = std::max(fearSeconds, fear);
	if (seconds > rotSeconds) {
		rotSeconds = seconds;
		rotDps = std::max(rotDps, dps);
	}
}

void Enemy::tickStatus(float dt) {
	if (snareSeconds > 0.0f) snareSeconds = std::max(0.0f, snareSeconds - dt);
	if (fearSeconds > 0.0f) fearSeconds = std::max(0.0f, fearSeconds - dt);
	if (rotSeconds > 0.0f) {
		applyDamage(rotDps * dt);
		rotSeconds = std::max(0.0f, rotSeconds - dt);
	}
}

bool Enemy::shouldDrop() const {
	return dropPending && !isAlive();
}

ReagentType Enemy::dropType() const {
	return heldDrop;
}

void Enemy::setDropType(ReagentType type) {
	heldDrop = type;
}

Pickup::Pickup(Vector3 startPosition, ReagentType reagentType, int amount) :
	Object(startPosition, 0.45f),
	reagent(reagentType),
	quantity(amount) {}

void Pickup::update(float dt) {
	bobTime += dt;
}

void Pickup::draw(const Camera3D& camera) const {
	if (!active) return;
	const ReagentDefinition& def = reagentDefinition(reagent);
	Vector3 drawPos = Vector3Add(position, {0.0f, 0.55f + std::sin(bobTime * 5.0f) * 0.08f, 0.0f});
	drawBillboardCard(camera, drawPos, 0.48f, 0.48f, Fade(def.color, 0.28f), def.color);
	DrawCircle3D(drawPos, 0.22f, {1.0f, 0.0f, 0.0f}, 90.0f, def.color);
}

Scenery::Scenery(Vector3 center, Vector3 scenerySize, Color lineColor, bool collidable, std::string label) :
	Object(center, std::max(scenerySize.x, scenerySize.z) * 0.5f),
	size(scenerySize),
	color(lineColor),
	name(std::move(label)) {
	collisionEnabled = collidable;
}

void Scenery::draw(const Camera3D&) const {
	Color fill = Fade(color, 0.16f);
	DrawCubeV(position, size, fill);
	DrawCubeWiresV(position, size, color);
	if (!name.empty()) {
		DrawLine3D(Vector3Add(position, {-size.x * 0.4f, size.y * 0.58f, -size.z * 0.52f}), Vector3Add(position, {size.x * 0.4f, size.y * 0.58f, -size.z * 0.52f}), color);
	}
}

BoundingBox Scenery::bounds() const {
	return {
		Vector3Subtract(position, Vector3Scale(size, 0.5f)),
		Vector3Add(position, Vector3Scale(size, 0.5f))
	};
}

bool Scenery::containsXZ(Vector3 point, float radius) const {
	if (!collisionEnabled) return false;
	BoundingBox box = bounds();
	return point.x > box.min.x - radius && point.x < box.max.x + radius &&
		point.z > box.min.z - radius && point.z < box.max.z + radius &&
		point.y < box.max.y + 1.2f;
}

Player::Player() : Actor({0.0f, GROUND_Y, -6.0f}, PLAYER_RADIUS, 100.0f) {
	camera = {};
	camera.up = {0.0f, 1.0f, 0.0f};
	camera.fovy = 74.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	yaw = 0.0f;
	pitch = 0.0f;
	refreshCamera();
}

void Player::updateCameraFromLook(Vector2 lookDelta) {
	constexpr float sensitivity = 0.0024f;
	yaw += lookDelta.x * sensitivity;
	pitch -= lookDelta.y * sensitivity;
	pitch = std::clamp(pitch, -1.28f, 1.28f);
	refreshCamera();
}

Vector3 Player::forward() const {
	const float cp = std::cos(pitch);
	return Vector3Normalize({std::sin(yaw) * cp, std::sin(pitch), std::cos(yaw) * cp});
}

Vector3 Player::flatForward() const {
	return safeNormalize({std::sin(yaw), 0.0f, std::cos(yaw)});
}

Vector3 Player::right() const {
	return safeNormalize({std::cos(yaw), 0.0f, -std::sin(yaw)});
}

Camera3D& Player::getCamera() {
	return camera;
}

const Camera3D& Player::getCamera() const {
	return camera;
}

void Player::refreshCamera() {
	camera.position = Vector3Add(position, {0.0f, eyeHeight, 0.0f});
	camera.target = Vector3Add(camera.position, forward());
}

void Player::addReagent(ReagentType type, int amount) {
	inventory[reagentIndex(type)] += amount;
}

int Player::totalReagents() const {
	int total = 0;
	for (int count : inventory) total += count;
	return total;
}

void Player::takeDamage(float amount) {
	float remaining = amount;
	if (ward > 0.0f) {
		const float wardAbsorb = wardActive ? 0.85f : 0.35f;
		const float blocked = std::min(ward, amount * wardAbsorb);
		ward -= blocked;
		remaining -= blocked;
	}
	applyDamage(std::max(0.0f, remaining));
	damageFlash = 0.5f;
}

void Player::gainWard(float amount) {
	ward = std::min(maxWard, ward + amount);
}

void Player::updateWard(const InputHandler::InputState& input, float dt) {
	if (wardCooldown > 0.0f) wardCooldown = std::max(0.0f, wardCooldown - dt);
	const bool wantsWard = input.wardDown && ward > 4.0f && wardCooldown <= 0.0f;
	wardActive = wantsWard;
	if (wardActive) {
		ward = std::max(0.0f, ward - 28.0f * dt);
		if (ward <= 0.0f) {
			wardActive = false;
			wardCooldown = 1.2f;
		}
	} else {
		ward = std::min(maxWard, ward + 9.0f * dt);
	}
	damageFlash = std::max(0.0f, damageFlash - dt);
	castFlash = std::max(0.0f, castFlash - dt * 4.0f);
}

bool Player::isWardActive() const {
	return wardActive;
}

float Player::wardRatio() const {
	return maxWard <= 0.0f ? 0.0f : ward / maxWard;
}

float Player::healthRatio() const {
	return maxHealth <= 0.0f ? 0.0f : health / maxHealth;
}

void CoatMenu::update(const InputHandler::InputState& input, Player& player) {
	open = input.rummageDown;
	pulse += GetFrameTime();
	if (!open) return;
	int mouseBase = -1;
	int mouseReagent = -1;
	if (input.mousePressed) {
		for (int i = 0; i < static_cast<int>(BaseVector::Count); ++i) {
			Rectangle slot{static_cast<float>(52 + i * 155), 154.0f, 138.0f, 54.0f};
			if (CheckCollisionPointRec(input.mousePosition, slot)) mouseBase = i;
		}
		for (int i = 0; i < REAGENT_COUNT; ++i) {
			Rectangle slot{static_cast<float>(52 + (i % 3) * 210), static_cast<float>(266 + (i / 3) * 82), 190.0f, 64.0f};
			if (CheckCollisionPointRec(input.mousePosition, slot)) mouseReagent = i;
		}
	}
	if (input.selectedBase >= 0) {
		player.spell.setBase(baseVectorFromIndex(input.selectedBase));
	}
	if (mouseBase >= 0) {
		player.spell.setBase(baseVectorFromIndex(mouseBase));
	}
	if (input.clearPressed) {
		player.spell.clearReagents();
	}
	if (input.selectedReagent >= 0) {
		player.spell.addReagent(reagentFromIndex(input.selectedReagent), player.inventory);
	}
	if (mouseReagent >= 0) {
		player.spell.addReagent(reagentFromIndex(mouseReagent), player.inventory);
	}
}

void CoatMenu::draw(const Player& player) const {
	if (!open) return;

	const int sw = GetScreenWidth();
	const int sh = GetScreenHeight();
	Rectangle panel{28.0f, 46.0f, static_cast<float>(sw - 56), static_cast<float>(sh - 112)};
	drawScreenPanel(panel, Color{230, 210, 140, 255});
	DrawText("COAT RUMMAGE - world is still moving", 48, 62, 22, Color{250, 220, 128, 255});
	DrawText("Slowed movement. Sprint disabled. Pick fast.", 48, 90, 16, Color{255, 126, 96, 255});

	DrawText("BASE VECTOR", 52, 128, 18, lineWhite());
	for (int i = 0; i < static_cast<int>(BaseVector::Count); ++i) {
		const auto& def = baseVectorDefinition(baseVectorFromIndex(i));
		const int x = 52 + i * 155;
		const bool selected = player.spell.getBase() == def.type;
		Rectangle slot{static_cast<float>(x), 154.0f, 138.0f, 54.0f};
		drawScreenPanel(slot, selected ? def.color : Fade(lineWhite(), 0.55f));
		DrawText(TextFormat("%s  %s", def.keyName, def.name), x + 9, 162, 17, selected ? def.color : lineWhite());
		DrawText(def.deliveryLine, x + 9, 184, 10, Fade(lineWhite(), 0.82f));
	}

	DrawText("REAGENTS (Z X C V B N or click; add up to 3; repeats strengthen)", 52, 236, 18, lineWhite());
	const auto selectedCounts = player.spell.selectedCounts();
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		const auto& def = reagentDefinition(reagentFromIndex(i));
		const int x = 52 + (i % 3) * 210;
		const int y = 266 + (i / 3) * 82;
		Rectangle slot{static_cast<float>(x), static_cast<float>(y), 190.0f, 64.0f};
		drawScreenPanel(slot, selectedCounts[i] > 0 ? def.color : Fade(lineWhite(), 0.45f));
		DrawText(TextFormat("%s  %s", def.keyName, def.name), x + 9, y + 8, 16, def.color);
		DrawText(TextFormat("held %d   picked %d", player.inventory[i], selectedCounts[i]), x + 9, y + 28, 13, lineWhite());
		DrawText(def.effectLine, x + 9, y + 45, 9, Fade(lineWhite(), 0.72f));
	}

	Rectangle preview{52.0f, static_cast<float>(sh - 190), static_cast<float>(sw - 104), 110.0f};
	drawScreenPanel(preview, curseGreen());
	DrawText("SPELL PREVIEW", 70, sh - 176, 18, curseGreen());
	DrawText(player.spell.previewName().c_str(), 70, sh - 148, 20, lineWhite());
	DrawText(player.spell.previewDetails().c_str(), 70, sh - 120, 16, Color{205, 255, 214, 255});
	DrawText("Release TAB then LMB casts. BACKSPACE clears ingredients. Poppet is always free.", 70, sh - 94, 14, Fade(lineWhite(), 0.78f));
}

bool CoatMenu::isOpen() const {
	return open;
}

void HUD::draw(const Player& player, const CoatMenu& coatMenu, const std::vector<Enemy>&, int livingEnemies) const {
	drawBar(18, 18, 210, 24, player.healthRatio(), Color{198, 32, 42, 255}, TextFormat("HEALTH %.0f", player.health));
	drawBar(18, 48, 210, 22, player.wardRatio(), player.isWardActive() ? wardBlue() : Color{58, 138, 204, 255}, TextFormat("WARD %.0f", player.ward));
	DrawText(TextFormat("Hunters: %d", livingEnemies), 18, 77, 16, lineWhite());
	DrawText(TextFormat("Spell: %s", player.spell.previewName().c_str()), 18, GetScreenHeight() - 80, 16, lineWhite());
	DrawText(player.spell.previewDetails().c_str(), 18, GetScreenHeight() - 58, 13, Fade(lineWhite(), 0.8f));
	DrawText("WASD move  SHIFT sprint  SPACE jump  TAB rummage  LMB cast/click  RMB/F ward  R reset  ESC mouse", 18, GetScreenHeight() - 28, 13, Fade(lineWhite(), 0.74f));

	const int startX = GetScreenWidth() - 178;
	DrawText("POCKETS", startX, 18, 14, lineWhite());
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		const auto& def = reagentDefinition(reagentFromIndex(i));
		DrawText(TextFormat("%s %s: %d", def.keyName, def.shortName, player.inventory[i]), startX, 40 + i * 18, 13, def.color);
	}

	drawReticle(player);
	if (player.damageFlash > 0.0f) {
		DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(Color{180, 0, 0, 255}, player.damageFlash * 0.22f));
	}
	coatMenu.draw(player);
	drawHands(player, coatMenu.isOpen());
}

Game::Game() : input(KEY_NULL) {
	seedWorld();
}

void Game::resetRun() {
	seedWorld();
	message = "New Salem district. Collect reagents, then hold TAB to rummage.";
	messageTimer = 2.0f;
}

void Game::seedWorld() {
	player = Player();
	districtCleared = false;
	gameOver = false;
	scenery.clear();
	enemies.clear();
	pickups.clear();
	projectiles.clear();
	mines.clear();
	hazards.clear();
	flashes.clear();

	scenery.emplace_back(Vector3{0.0f, 1.1f, 17.5f}, Vector3{10.0f, 2.2f, 1.0f}, Color{116, 158, 128, 255}, true, "meeting house");
	scenery.emplace_back(Vector3{-9.5f, 1.2f, 5.0f}, Vector3{1.0f, 2.4f, 10.5f}, Color{110, 160, 120, 255}, true, "fence");
	scenery.emplace_back(Vector3{9.5f, 1.2f, 3.5f}, Vector3{1.0f, 2.4f, 9.0f}, Color{110, 160, 120, 255}, true, "fence");
	scenery.emplace_back(Vector3{-4.5f, 1.0f, -0.5f}, Vector3{2.0f, 2.0f, 2.0f}, Color{116, 158, 128, 255}, true, "well");
	scenery.emplace_back(Vector3{5.8f, 1.6f, 10.0f}, Vector3{3.5f, 3.2f, 3.0f}, Color{116, 158, 128, 255}, true, "salt shed");
	scenery.emplace_back(Vector3{-6.5f, 1.6f, 12.0f}, Vector3{3.0f, 3.2f, 2.8f}, Color{116, 158, 128, 255}, true, "pyre cart");

	for (int i = 0; i < REAGENT_COUNT; ++i) {
		const float angle = i * (2.0f * PI_F / REAGENT_COUNT);
		pickups.emplace_back(Vector3{std::sin(angle) * 3.2f, 0.0f, -1.5f + std::cos(angle) * 3.2f}, reagentFromIndex(i), 2);
	}

	const Vector3 starts[] = {
		{-5.0f, 0.0f, 8.0f}, {4.0f, 0.0f, 9.0f}, {0.0f, 0.0f, 13.0f},
		{-8.0f, 0.0f, 18.0f}, {8.0f, 0.0f, 18.0f}, {-1.5f, 0.0f, 23.0f},
		{6.5f, 0.0f, 25.0f}, {-7.0f, 0.0f, 27.0f}
	};
	for (int i = 0; i < 8; ++i) {
		Enemy enemy(starts[i], i % 3 == 0 ? "Musketeer" : "Torch Mob", i % 3 == 0 ? 70.0f : 48.0f, i % 3 == 0 ? 2.15f : 2.75f, i % 3 == 0 ? 14.0f : 9.0f);
		enemy.setDropType(reagentFromIndex(i % REAGENT_COUNT));
		enemies.push_back(enemy);
	}
}

void Game::update(float dt) {
	dt = std::min(dt, 0.05f);
	InputHandler::InputState inputState = input.poll();
	updateWithInput(inputState, dt);
}

void Game::updateWithInput(const InputHandler::InputState& inputState, float dt) {
	if (inputState.resetPressed || ((gameOver || districtCleared) && inputState.mousePressed)) {
		resetRun();
		return;
	}
	if (!player.isAlive()) {
		gameOver = true;
		message = "The hunters drag the witch down. Press R to restart.";
		messageTimer = 999.0f;
		updateFlashes(dt);
		return;
	}
	if (districtCleared) {
		message = "District cleared. Press R to run it again.";
		messageTimer = 999.0f;
		updatePickups(dt);
		updateFlashes(dt);
		return;
	}
	player.updateCameraFromLook(inputState.lookInput);
	coatMenu.update(inputState, player);
	player.updateWard(inputState, dt);
	updatePlayer(inputState, dt);
	updateEnemies(dt);
	resolvePlayerEnemyCollisions();
	updatePickups(dt);
	updateProjectiles(dt);
	updateMines(dt);
	updateHazards(dt);
	updateFlashes(dt);
	if (inputState.castPressed && player.isAlive() && !coatMenu.isOpen()) {
		castCurrentSpell();
	}
	if (livingEnemyCount() <= 0) {
		districtCleared = true;
		message = "District cleared. Press R to run it again.";
		messageTimer = 999.0f;
	}
	messageTimer = std::max(0.0f, messageTimer - dt);
}

void Game::resolvePlayerEnemyCollisions() {
	for (const Enemy& enemy : enemies) {
		if (!enemy.isAlive()) continue;
		const float minimumDistance = PLAYER_RADIUS + enemy.getCollisionRadius();
		const float distance = distanceXZ(player.position, enemy.getPosition());
		if (distance >= minimumDistance) continue;
		Vector3 away = distance <= 0.0001f ? Vector3Scale(player.flatForward(), -1.0f) : safeNormalize(Vector3Subtract(player.position, enemy.getPosition()));
		away.y = 0.0f;
		const float push = minimumDistance - distance;
		Vector3 next = Vector3Add(player.position, Vector3Scale(away, push));
		if (!playerBlockedAt(next)) {
			player.position = next;
			player.refreshCamera();
		}
	}
}

void Game::updatePlayer(const InputHandler::InputState& inputState, float dt) {
	Vector3 wish{};
	wish = Vector3Add(wish, Vector3Scale(player.flatForward(), inputState.moveInput.x));
	wish = Vector3Add(wish, Vector3Scale(player.right(), inputState.moveInput.y));
	wish.y = 0.0f;
	wish = safeNormalize(wish);
	const bool sprinting = inputState.sprintDown && !coatMenu.isOpen();
	float speed = sprinting ? SPRINT_SPEED : WALK_SPEED;
	if (coatMenu.isOpen()) speed *= RUMMAGE_SPEED_FACTOR;

	Vector3 horizontal = Vector3Scale(wish, speed * dt);
	movePlayerAxis({horizontal.x, 0.0f, 0.0f});
	movePlayerAxis({0.0f, 0.0f, horizontal.z});

	if (player.grounded && inputState.jumpPressed && !coatMenu.isOpen()) {
		player.velocity.y = 8.2f;
		player.grounded = false;
	}
	player.velocity.y -= GRAVITY * dt;
	player.position.y += player.velocity.y * dt;
	if (player.position.y <= GROUND_Y) {
		player.position.y = GROUND_Y;
		player.velocity.y = 0.0f;
		player.grounded = true;
	}
	player.refreshCamera();
}

void Game::movePlayerAxis(Vector3 delta) {
	Vector3 next = Vector3Add(player.position, delta);
	if (!playerBlockedAt(next)) {
		player.position = next;
	}
}

bool Game::playerBlockedAt(Vector3 position) const {
	if (position.x < -18.0f || position.x > 18.0f || position.z < -14.0f || position.z > 34.0f) return true;
	for (const Scenery& item : scenery) {
		if (item.containsXZ(position, PLAYER_RADIUS)) return true;
	}
	return false;
}

void Game::updateEnemies(float dt) {
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) continue;
		enemy.updateToward(player.position, dt);
		if (enemy.canAttack(player.position)) {
			player.takeDamage(enemy.consumeAttackDamage());
			message = player.isWardActive() ? "Ward catches the hunter's blow." : "A hunter strikes through your coat.";
			messageTimer = 1.0f;
		}
	}

	for (Enemy& enemy : enemies) {
		if (enemy.shouldDrop()) {
			dropReagent(enemy.getPosition(), enemy.dropType(), 1 + (GetRandomValue(0, 2) == 0 ? 1 : 0));
			enemy.dropPending = false;
		}
	}
}

void Game::updatePickups(float dt) {
	for (Pickup& pickup : pickups) {
		if (!pickup.isActive()) continue;
		pickup.update(dt);
		if (Vector3Distance(player.position, pickup.getPosition()) < 1.15f) {
			player.addReagent(pickup.reagent, pickup.quantity);
			pickup.setActive(false);
			message = TextFormat("Collected %d %s.", pickup.quantity, reagentDefinition(pickup.reagent).name);
			messageTimer = 1.4f;
		}
	}
	pickups.erase(std::remove_if(pickups.begin(), pickups.end(), [](const Pickup& pickup) { return !pickup.isActive(); }), pickups.end());
}

void Game::updateProjectiles(float dt) {
	for (Projectile& projectile : projectiles) {
		if (!projectile.active) continue;
		projectile.age += dt;
		if (projectile.age > projectile.maxAge) {
			projectile.active = false;
			continue;
		}
		SpellStats stats = projectile.spell.stats();
		if (stats.homingStrength > 0.0f) {
			if (Enemy* target = nearestEnemy(projectile.position, 9.0f + stats.homingStrength)) {
				Vector3 desired = safeNormalize(Vector3Subtract(Vector3Add(target->getPosition(), {0.0f, 0.9f, 0.0f}), projectile.position));
				Vector3 current = safeNormalize(projectile.velocity);
				Vector3 steered = safeNormalize(Vector3Lerp(current, desired, std::min(1.0f, stats.homingStrength * dt * 0.24f)));
				projectile.velocity = Vector3Scale(steered, stats.projectileSpeed);
			}
		}
		Vector3 next = Vector3Add(projectile.position, Vector3Scale(projectile.velocity, dt));
		if (lineHitsScenery(projectile.position, next)) {
			resolveImpact(projectile.position, projectile.spell);
			projectile.active = false;
			continue;
		}
		projectile.position = next;
		for (Enemy& enemy : enemies) {
			if (!enemy.isAlive()) continue;
			if (Vector3Distance(Vector3Add(enemy.getPosition(), {0.0f, 0.85f, 0.0f}), projectile.position) <= enemy.getCollisionRadius() + 0.25f) {
				resolveImpact(projectile.position, projectile.spell);
				projectile.active = false;
				break;
			}
		}
	}
	projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& projectile) { return !projectile.active; }), projectiles.end());
}

void Game::updateMines(float dt) {
	for (SpiderMine& mine : mines) {
		if (!mine.active) continue;
		mine.age += dt;
		mine.armed = mine.age >= mine.spell.stats().mineArmingSeconds;
		if (mine.age > 14.0f) {
			resolveImpact(mine.position, mine.spell);
			mine.active = false;
			continue;
		}
		if (!mine.armed) continue;
		for (const Enemy& enemy : enemies) {
			if (enemy.isAlive() && distanceXZ(enemy.getPosition(), mine.position) <= 1.45f) {
				resolveImpact(mine.position, mine.spell);
				mine.active = false;
				break;
			}
		}
	}
	mines.erase(std::remove_if(mines.begin(), mines.end(), [](const SpiderMine& mine) { return !mine.active; }), mines.end());
}

void Game::updateHazards(float dt) {
	for (LingeringHazard& hazard : hazards) {
		if (!hazard.active) continue;
		hazard.age += dt;
		hazard.pulseTimer -= dt;
		if (hazard.age >= hazard.stats.lingerSeconds) {
			hazard.active = false;
			continue;
		}
		if (hazard.pulseTimer <= 0.0f) {
			hazard.pulseTimer = 0.32f;
			for (Enemy& enemy : enemies) {
				if (enemy.isAlive() && distanceXZ(enemy.getPosition(), hazard.position) <= hazard.stats.radius) {
					enemy.applyDamage(std::max(1.0f, hazard.stats.rotDps * 0.32f));
					enemy.applyStatus(hazard.stats.snareSeconds * 0.25f, hazard.stats.fearSeconds * 0.15f, 0.0f, 0.0f);
				}
			}
		}
	}
	hazards.erase(std::remove_if(hazards.begin(), hazards.end(), [](const LingeringHazard& hazard) { return !hazard.active; }), hazards.end());
}

void Game::updateFlashes(float dt) {
	for (ImpactFlash& flash : flashes) {
		flash.age += dt;
		if (flash.age >= flash.lifetime) flash.active = false;
	}
	flashes.erase(std::remove_if(flashes.begin(), flashes.end(), [](const ImpactFlash& flash) { return !flash.active; }), flashes.end());
}

void Game::castCurrentSpell() {
	if (!player.spell.canAfford(player.inventory)) {
		message = "Not enough of that reagent in your coat.";
		messageTimer = 1.2f;
		return;
	}
	if (player.totalReagents() <= 0 || player.spell.getBase() == BaseVector::Poppet) {
		castPoppet();
		return;
	}

	const CurseSpell spell = player.spell;
	const SpellStats stats = spell.stats();
	player.spell.consume(player.inventory);
	player.castFlash = 1.0f;

	switch (spell.getBase()) {
	case BaseVector::Bottle: {
		Projectile projectile;
		projectile.position = Vector3Add(player.getCamera().position, Vector3Scale(player.forward(), 0.8f));
		projectile.velocity = Vector3Scale(player.forward(), stats.projectileSpeed);
		projectile.spell = spell;
		projectiles.push_back(projectile);
		message = "Bottle curse thrown.";
		break;
	}
	case BaseVector::Apple:
		player.gainWard(stats.wardGain);
		applySpellAt(player.position, spell, true);
		message = "You bite the hex apple. It blooms outward.";
		break;
	case BaseVector::Spider: {
		SpiderMine mine;
		Vector3 place = Vector3Add(player.position, Vector3Scale(player.flatForward(), 1.65f));
		place.y = GROUND_Y + 0.05f;
		mine.position = place;
		mine.spell = spell;
		mines.push_back(mine);
		message = "Spider mine tucked into the dirt.";
		break;
	}
	case BaseVector::Poppet:
	case BaseVector::Count:
		castPoppet();
		break;
	}
	messageTimer = 1.0f;
}

void Game::castPoppet() {
	player.castFlash = 0.7f;
	Vector3 start = player.getCamera().position;
	Vector3 dir = player.forward();
	Enemy* best = nullptr;
	float bestDot = 0.985f;
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) continue;
		Vector3 toEnemy = safeNormalize(Vector3Subtract(Vector3Add(enemy.getPosition(), {0.0f, 0.9f, 0.0f}), start));
		const float dot = Vector3DotProduct(dir, toEnemy);
		const float dist = Vector3Distance(enemy.getPosition(), player.position);
		if (dot > bestDot && dist < 18.0f) {
			bestDot = dot;
			best = &enemy;
		}
	}
	if (best) {
		best->applyDamage(CurseSpell{}.stats().damage);
		flashes.push_back({Vector3Add(best->getPosition(), {0.0f, 0.9f, 0.0f}), baseVectorDefinition(BaseVector::Poppet).color});
		message = "Poppet pin finds a hunter.";
	} else {
		message = "Poppet pin cuts empty air.";
	}
	messageTimer = 0.8f;
}

void Game::resolveImpact(Vector3 impactPosition, const CurseSpell& spell) {
	applySpellAt(impactPosition, spell, true);
	flashes.push_back({impactPosition, baseVectorDefinition(spell.getBase()).color});
}

void Game::applySpellAt(Vector3 position, const CurseSpell& spell, bool spawnHazard) {
	const SpellStats stats = spell.stats();
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) continue;
		if (distanceXZ(enemy.getPosition(), position) <= stats.radius + enemy.getCollisionRadius()) {
			enemy.applyDamage(stats.damage);
			enemy.applyStatus(stats.snareSeconds, stats.fearSeconds, stats.rotDps, stats.rotSeconds);
			if (stats.wardGain > 0.0f) player.gainWard(stats.wardGain * 0.35f);
		}
	}
	if (spawnHazard && stats.lingerSeconds > 0.0f) {
		hazards.push_back({position, stats});
	}
}

void Game::dropReagent(Vector3 position, ReagentType type, int quantity) {
	position.y = 0.0f;
	pickups.emplace_back(position, type, quantity);
}

bool Game::lineHitsScenery(Vector3 start, Vector3 end) const {
	Ray ray{start, safeNormalize(Vector3Subtract(end, start))};
	const float length = Vector3Distance(start, end);
	for (const Scenery& item : scenery) {
		if (!item.hasCollision()) continue;
		RayCollision hit = GetRayCollisionBox(ray, item.bounds());
		if (hit.hit && hit.distance <= length) return true;
	}
	return false;
}

Enemy* Game::nearestEnemy(Vector3 position, float maxDistance) {
	Enemy* best = nullptr;
	float bestDistance = maxDistance;
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) continue;
		const float distance = Vector3Distance(enemy.getPosition(), position);
		if (distance < bestDistance) {
			bestDistance = distance;
			best = &enemy;
		}
	}
	return best;
}

int Game::livingEnemyCount() const {
	int count = 0;
	for (const Enemy& enemy : enemies) {
		if (enemy.isAlive()) ++count;
	}
	return count;
}

void Game::draw() const {
	BeginDrawing();
	ClearBackground(Color{5, 7, 9, 255});
	BeginMode3D(player.getCamera());
	drawWorld();
	for (const Pickup& pickup : pickups) pickup.draw(player.getCamera());
	for (const Enemy& enemy : enemies) enemy.draw(player.getCamera());
	drawEffects();
	EndMode3D();
	if (messageTimer > 0.0f) {
		DrawRectangle(18, 104, 460, 30, Fade(Color{0, 0, 0, 255}, 0.72f));
		DrawRectangleLines(18, 104, 460, 30, Color{230, 210, 140, 255});
		DrawText(message.c_str(), 28, 112, 14, Color{250, 220, 128, 255});
	}
	hud.draw(player, coatMenu, enemies, livingEnemyCount());
	if (gameOver || districtCleared) {
		const char* title = gameOver ? "WITCH DOWN" : "DISTRICT CLEARED";
		const char* prompt = "Press R or click to restart the combat slice";
		const int sw = GetScreenWidth();
		const int sh = GetScreenHeight();
		Rectangle panel{sw * 0.5f - 230.0f, sh * 0.5f - 58.0f, 460.0f, 116.0f};
		drawScreenPanel(panel, gameOver ? Color{255, 80, 80, 255} : curseGreen());
		DrawText(title, static_cast<int>(panel.x + 30), static_cast<int>(panel.y + 22), 32, gameOver ? Color{255, 80, 80, 255} : curseGreen());
		DrawText(prompt, static_cast<int>(panel.x + 30), static_cast<int>(panel.y + 70), 18, lineWhite());
	}
	EndDrawing();
}

void Game::drawWorld() const {
	DrawPlane({0.0f, -0.01f, 10.0f}, {42.0f, 58.0f}, Color{9, 14, 12, 255});
	DrawGrid(48, 1.0f);
	for (const Scenery& item : scenery) item.draw(player.getCamera());

	DrawLine3D({-18.0f, 0.03f, -14.0f}, {18.0f, 0.03f, -14.0f}, Color{80, 120, 96, 255});
	DrawLine3D({18.0f, 0.03f, -14.0f}, {18.0f, 0.03f, 34.0f}, Color{80, 120, 96, 255});
	DrawLine3D({18.0f, 0.03f, 34.0f}, {-18.0f, 0.03f, 34.0f}, Color{80, 120, 96, 255});
	DrawLine3D({-18.0f, 0.03f, 34.0f}, {-18.0f, 0.03f, -14.0f}, Color{80, 120, 96, 255});
}

void Game::drawEffects() const {
	for (const Projectile& projectile : projectiles) {
		Color color = baseVectorDefinition(projectile.spell.getBase()).color;
		DrawSphere(projectile.position, 0.12f, Fade(color, 0.45f));
		DrawSphereWires(projectile.position, 0.18f, 8, 8, color);
	}
	for (const SpiderMine& mine : mines) {
		Color color = mine.armed ? Color{190, 120, 255, 255} : Fade(lineWhite(), 0.55f);
		DrawCircle3D(mine.position, 0.46f, {1.0f, 0.0f, 0.0f}, 90.0f, color);
		for (int i = 0; i < 8; ++i) {
			const float angle = i * PI_F * 0.25f;
			DrawLine3D(mine.position, Vector3Add(mine.position, {std::cos(angle) * 0.65f, 0.0f, std::sin(angle) * 0.65f}), color);
		}
	}
	for (const LingeringHazard& hazard : hazards) {
		const float ratio = 1.0f - hazard.age / std::max(0.01f, hazard.stats.lingerSeconds);
		Color color = Fade(Color{110, 78, 255, 255}, 0.25f + ratio * 0.35f);
		DrawCircle3D(Vector3Add(hazard.position, {0.0f, 0.04f, 0.0f}), hazard.stats.radius, {1.0f, 0.0f, 0.0f}, 90.0f, color);
		DrawCircle3D(Vector3Add(hazard.position, {0.0f, 0.06f, 0.0f}), hazard.stats.radius * 0.62f, {1.0f, 0.0f, 0.0f}, 90.0f, color);
	}
	for (const ImpactFlash& flash : flashes) {
		const float ratio = 1.0f - flash.age / flash.lifetime;
		DrawSphereWires(flash.position, 0.35f + ratio * 1.4f, 12, 12, Fade(flash.color, ratio));
	}
}

bool runVerticalSliceSelfTest(std::string* errorMessage) {
	auto fail = [errorMessage](const char* message) {
		if (errorMessage) *errorMessage = message;
		return false;
	};

	if (REAGENT_COUNT != 6) return fail("expected exactly six reagents");
	if (static_cast<int>(BaseVector::Count) != 4) return fail("expected bottle/apple/spider/poppet vectors");

	ReagentInventory inventory{};
	inventory[reagentIndex(ReagentType::GraveSalt)] = 2;
	inventory[reagentIndex(ReagentType::BlackWax)] = 1;
	inventory[reagentIndex(ReagentType::SaintAsh)] = 1;

	CurseSpell bottle;
	bottle.setBase(BaseVector::Bottle);
	const SpellStats plainBottle = bottle.stats();
	if (!bottle.addReagent(ReagentType::GraveSalt, inventory)) return fail("first grave salt should be selectable");
	if (!bottle.addReagent(ReagentType::GraveSalt, inventory)) return fail("repeated grave salt should be selectable up to inventory count");
	if (!bottle.addReagent(ReagentType::BlackWax, inventory)) return fail("third reagent should be selectable");
	if (bottle.addReagent(ReagentType::SaintAsh, inventory)) return fail("spell should reject a fourth reagent");
	const SpellStats saltWaxBottle = bottle.stats();
	if (saltWaxBottle.damage <= plainBottle.damage + 20.0f) return fail("repeated damage reagent did not strengthen damage enough");
	if (saltWaxBottle.lingerSeconds <= 0.0f || saltWaxBottle.rotDps <= 0.0f) return fail("black wax did not add rot/linger");
	if (!bottle.canAfford(inventory)) return fail("spell should be affordable before consuming");
	bottle.consume(inventory);
	if (inventory[reagentIndex(ReagentType::GraveSalt)] != 0) return fail("repeated reagent consumption failed");
	if (inventory[reagentIndex(ReagentType::BlackWax)] != 0) return fail("black wax consumption failed");

	CurseSpell apple;
	apple.setBase(BaseVector::Apple);
	ReagentInventory appleInventory{};
	appleInventory[reagentIndex(ReagentType::SaintAsh)] = 1;
	if (!apple.addReagent(ReagentType::SaintAsh, appleInventory)) return fail("apple should accept saint ash");
	if (apple.stats().wardGain <= plainBottle.wardGain) return fail("apple/sainthood ward gain did not compose");

	CurseSpell spider;
	spider.setBase(BaseVector::Spider);
	ReagentInventory spiderInventory{};
	spiderInventory[reagentIndex(ReagentType::WidowNettle)] = 1;
	spiderInventory[reagentIndex(ReagentType::MothDust)] = 1;
	if (!spider.addReagent(ReagentType::WidowNettle, spiderInventory)) return fail("spider should accept widow nettle");
	if (!spider.addReagent(ReagentType::MothDust, spiderInventory)) return fail("spider should accept moth dust");
	if (spider.stats().snareSeconds <= 0.0f || spider.stats().fearSeconds <= 0.0f) return fail("spider statuses did not compose");

	CurseSpell poppet;
	if (poppet.getBase() != BaseVector::Poppet) return fail("default spell should be poppet fallback");
	if (poppet.stats().damage <= 0.0f) return fail("poppet should do weak fallback damage");

	Player wardTester;
	const float healthBeforeHit = wardTester.health;
	wardTester.takeDamage(20.0f);
	const float unraisedWardHealthLoss = healthBeforeHit - wardTester.health;
	Player raisedWardTester;
	InputHandler::InputState wardInput{};
	wardInput.wardDown = true;
	raisedWardTester.updateWard(wardInput, 0.1f);
	if (!raisedWardTester.isWardActive()) return fail("ward should activate when held and resourced");
	const float raisedHealthBeforeHit = raisedWardTester.health;
	raisedWardTester.takeDamage(20.0f);
	const float raisedWardHealthLoss = raisedHealthBeforeHit - raisedWardTester.health;
	if (raisedWardHealthLoss >= unraisedWardHealthLoss) return fail("raised ward should absorb more damage than passive ward");
	if (raisedWardTester.ward >= wardTester.ward) return fail("raised ward should visibly spend ward resource");

	Player coatPlayer;
	coatPlayer.inventory[reagentIndex(ReagentType::GraveSalt)] = 1;
	CoatMenu testCoat;
	InputHandler::InputState mouseBaseInput{};
	mouseBaseInput.rummageDown = true;
	mouseBaseInput.mousePressed = true;
	mouseBaseInput.mousePosition = {52.0f + 155.0f + 10.0f, 154.0f + 10.0f};
	testCoat.update(mouseBaseInput, coatPlayer);
	if (coatPlayer.spell.getBase() != BaseVector::Apple) return fail("mouse coat base selection failed");
	InputHandler::InputState mouseReagentInput{};
	mouseReagentInput.rummageDown = true;
	mouseReagentInput.mousePressed = true;
	mouseReagentInput.mousePosition = {62.0f, 276.0f};
	testCoat.update(mouseReagentInput, coatPlayer);
	if (coatPlayer.spell.selectedCounts()[reagentIndex(ReagentType::GraveSalt)] != 1) return fail("mouse coat reagent selection failed");
	InputHandler::InputState keyReagentInput{};
	keyReagentInput.rummageDown = true;
	keyReagentInput.selectedReagent = reagentIndex(ReagentType::SaintAsh);
	testCoat.update(keyReagentInput, coatPlayer);
	if (coatPlayer.spell.selectedCounts()[reagentIndex(ReagentType::SaintAsh)] != 0) return fail("coat should not select unavailable reagent");

	Game movementGame;
	movementGame.scenery.clear();
	movementGame.enemies.clear();
	movementGame.pickups.clear();
	const float startZ = movementGame.player.getPosition().z;
	InputHandler::InputState walkInput{};
	walkInput.moveInput.x = 1.0f;
	movementGame.updateWithInput(walkInput, 0.1f);
	const float walkDelta = movementGame.player.getPosition().z - startZ;
	movementGame.resetRun();
	movementGame.scenery.clear();
	movementGame.enemies.clear();
	movementGame.pickups.clear();
	InputHandler::InputState sprintInput = walkInput;
	sprintInput.sprintDown = true;
	const float sprintStartZ = movementGame.player.getPosition().z;
	movementGame.updateWithInput(sprintInput, 0.1f);
	const float sprintDelta = movementGame.player.getPosition().z - sprintStartZ;
	movementGame.resetRun();
	movementGame.scenery.clear();
	movementGame.enemies.clear();
	movementGame.pickups.clear();
	InputHandler::InputState rummageMoveInput = sprintInput;
	rummageMoveInput.rummageDown = true;
	const float rummageStartZ = movementGame.player.getPosition().z;
	movementGame.updateWithInput(rummageMoveInput, 0.1f);
	const float rummageDelta = movementGame.player.getPosition().z - rummageStartZ;
	if (!(sprintDelta > walkDelta && walkDelta > rummageDelta && rummageDelta > 0.0f)) return fail("walk/sprint/rummage movement speeds are not ordered correctly");
	movementGame.resetRun();
	movementGame.scenery.clear();
	movementGame.enemies.clear();
	movementGame.pickups.clear();
	movementGame.districtCleared = false;
	InputHandler::InputState rummageJumpInput{};
	rummageJumpInput.rummageDown = true;
	rummageJumpInput.jumpPressed = true;
	movementGame.updateWithInput(rummageJumpInput, 0.1f);
	if (!movementGame.player.grounded || movementGame.player.getPosition().y != GROUND_Y) return fail("rummaging should disable jumping without immobilizing movement");

	Game collisionGame;
	collisionGame.scenery.clear();
	collisionGame.pickups.clear();
	collisionGame.enemies.clear();
	collisionGame.enemies.emplace_back(collisionGame.player.getPosition(), "Collision Hunter", 30.0f, 0.0f, 0.0f);
	collisionGame.resolvePlayerEnemyCollisions();
	if (distanceXZ(collisionGame.player.getPosition(), collisionGame.enemies.front().getPosition()) < PLAYER_RADIUS + collisionGame.enemies.front().getCollisionRadius() - 0.01f) {
		return fail("player/enemy collision pushout failed");
	}

	MainMenu menu;
	if (!menu.isActive()) return fail("main menu should start active");
	if (menu.consumePlayRequested()) return fail("main menu should not consume play before request");
	menu.requestPlay();
	if (!menu.consumePlayRequested() || menu.isActive()) return fail("main menu requested play should deactivate menu");

	Sprite sprite;
	sprite.setFrameCountForTest(4);
	if (sprite.frames() != 4) return fail("sprite frame count helper failed");
	Animation animation;
	animation.setFrameCountForTest(4);
	if (!animation.isLoaded()) return fail("animation test frame setup failed");
	animation.update(0.11f);
	if (animation.currentFrame() != 1) return fail("animation did not advance a frame");
	animation.update(0.31f);
	if (animation.currentFrame() != 0) return fail("animation did not wrap predictably");

	Game combatGame;
	const int initialPickupCount = static_cast<int>(combatGame.pickups.size());
	CurseSpell killSpell;
	killSpell.setBase(BaseVector::Bottle);
	ReagentInventory killInventory{};
	killInventory[reagentIndex(ReagentType::GraveSalt)] = 3;
	if (!killSpell.addReagent(ReagentType::GraveSalt, killInventory)) return fail("kill spell salt 1 failed");
	if (!killSpell.addReagent(ReagentType::GraveSalt, killInventory)) return fail("kill spell salt 2 failed");
	if (!killSpell.addReagent(ReagentType::GraveSalt, killInventory)) return fail("kill spell salt 3 failed");
	combatGame.applySpellAt(combatGame.enemies.front().getPosition(), killSpell, false);
	if (combatGame.enemies.front().isAlive()) return fail("direct spell application should kill first hunter");
	combatGame.updateEnemies(0.016f);
	if (static_cast<int>(combatGame.pickups.size()) <= initialPickupCount) return fail("dead enemies should drop reagents");
	for (Enemy& enemy : combatGame.enemies) {
		enemy.applyDamage(999.0f);
		enemy.dropPending = false;
	}
	InputHandler::InputState emptyInput{};
	combatGame.updateWithInput(emptyInput, 0.016f);
	if (!combatGame.districtCleared) return fail("game should enter district-cleared state when all enemies die");
	emptyInput.resetPressed = true;
	combatGame.updateWithInput(emptyInput, 0.016f);
	if (combatGame.districtCleared || combatGame.gameOver || combatGame.livingEnemyCount() != 8) return fail("reset should restore a fresh district");
	combatGame.player.applyDamage(999.0f);
	InputHandler::InputState deathInput{};
	combatGame.updateWithInput(deathInput, 0.016f);
	if (!combatGame.gameOver) return fail("game should enter death state when player dies");
	InputHandler::InputState clickRestartInput{};
	clickRestartInput.mousePressed = true;
	combatGame.updateWithInput(clickRestartInput, 0.016f);
	if (combatGame.gameOver || combatGame.districtCleared || combatGame.livingEnemyCount() != 8) return fail("clicking death/clear overlay should restart the district");

	Game poppetGame;
	poppetGame.scenery.clear();
	poppetGame.pickups.clear();
	poppetGame.enemies.clear();
	poppetGame.enemies.emplace_back(Vector3{0.0f, 0.0f, 4.0f}, "Poppet Target", 10.0f, 0.0f, 0.0f);
	poppetGame.castPoppet();
	if (poppetGame.enemies.front().isAlive()) return fail("free poppet fallback should damage/kill a weak target without reagents");

	return true;
}
