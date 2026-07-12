#include "Player.hxx"

#include <algorithm>
#include <cmath>

Player::Player() {
	position = Vector3{0.0f, 1.0f, 7.0f};
	camera.position = position;
	camera.target = Vector3{0.0f, 1.0f, 6.0f};
	camera.up = Vector3{0.0f, 1.0f, 0.0f};
	camera.fovy = 75.0f;
	camera.projection = CAMERA_PERSPECTIVE;
}

void Player::setPosition(Vector3 value) {
	position = value;
	camera.position = position;
	camera.target = Vector3Add(position, forward());
}

Vector3 Player::forward() const {
	const float pitch = look.y;
	return Vector3{cosf(pitch) * sinf(look.x), sinf(pitch), -cosf(pitch) * cosf(look.x)};
}

void Player::update(float dt, const InputHandler::InputState &input, bool rummageState) {
	rummaging = rummageState;
	look.x -= input.lookInput.x * 0.0025f;
	look.y = std::clamp(look.y - input.lookInput.y * 0.0025f, -1.2f, 1.2f);
	Vector3 right{cosf(look.x), 0.0f, sinf(look.x)};
	Vector3 heading{forward().x, 0.0f, forward().z};
	Vector3 move = Vector3Add(Vector3Scale(heading, input.moveInput.x), Vector3Scale(right, input.moveInput.y));
	if (Vector3Length(move) > 1.0f) move = Vector3Normalize(move);
	const float speed = rummaging ? 2.0f : (IsKeyDown(KEY_LEFT_SHIFT) ? 7.0f : 4.2f);
	position = Vector3Add(position, Vector3Scale(move, speed * dt));
	if (!rummaging && grounded && IsKeyPressed(KEY_SPACE)) { verticalSpeed = 5.2f; grounded = false; }
	verticalSpeed -= 12.0f * dt;
	position.y += verticalSpeed * dt;
	if (position.y <= 1.0f) { position.y = 1.0f; verticalSpeed = 0.0f; grounded = true; }
	if (wardActive) { wardTimer = std::max(0.0f, wardTimer - dt); if (wardTimer <= 0.0f || ward <= 0) wardActive = false; }
	if (!wardActive && ward < maxWard) {
		wardRegen += dt * 5.0f;
		const int recovered = static_cast<int>(wardRegen);
		if (recovered > 0) { ward = std::min(maxWard, ward + recovered); wardRegen -= recovered; }
	}
	camera.position = position;
	camera.target = Vector3Add(position, forward());
}

void Player::damage(int value) {
	if (wardActive) { ward = std::max(0, ward - value); wardActive = ward > 0; return; }
	health = std::max(0, health - value);
}

void Player::gainWard(int value) { ward = std::min(maxWard, ward + value); }

void Player::addReagent(ReagentType type, int amount) {
	int &stack = inventory[static_cast<int>(type)];
	stack = std::min(getReagentDefinition(type).maxStack, stack + amount);
}

void Player::activateWard() {
	if (ward > 0 && wardTimer <= 0.0f) { wardActive = true; wardTimer = 4.0f; }
}

float Player::wardCooldownRatio() const { return wardActive ? wardTimer / 4.0f : ward / static_cast<float>(maxWard); }

void Player::consumeSpellReagents() {
	for (int slot : selectedSpell.getReagents()) if (slot >= 0) inventory[slot] = std::max(0, inventory[slot] - 1);
	selectedSpell.clearReagents();
}

void Player::drawHands() const {
	const int w = GetScreenWidth();
	const int h = GetScreenHeight();
	const Color ink = wardActive ? Color{125, 220, 255, 255} : Color{238, 214, 178, 255};
	DrawLine(w / 2 - 100, h - 20, w / 2 - 55, h - 110, ink);
	DrawLine(w / 2 - 55, h - 110, w / 2 - 18, h - 150, ink);
	DrawLine(w / 2 + 100, h - 20, w / 2 + 55, h - 110, ink);
	DrawLine(w / 2 + 55, h - 110, w / 2 + 18, h - 150, ink);
	DrawCircle(w / 2 - 18, h - 150, 7.0f, ink);
	DrawCircle(w / 2 + 18, h - 150, 7.0f, ink);
	if (wardActive) { DrawCircleLines(w / 2, h / 2, 58.0f, Fade(ink, 0.7f)); DrawCircleLines(w / 2, h / 2, 66.0f, Fade(ink, 0.3f)); }
}
