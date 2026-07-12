#include "Player.hxx"
#include <algorithm>

Player::Player() {
	camera.position = Vector3{0.0f, 1.75f, 0.0f};
	camera.target = Vector3{0.0f, 1.75f, 1.0f};
	camera.up = Vector3{0.0f, 1.0f, 0.0f};
	camera.fovy = 72.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	draft.base = BaseVector::Poppet;
}

Vector3 Player::position() const {
	return camera.position;
}

void Player::setPosition(Vector3 position) {
	Vector3 forward{
		camera.target.x - camera.position.x,
		camera.target.y - camera.position.y,
		camera.target.z - camera.position.z
	};
	camera.position = position;
	camera.target = Vector3{
		position.x + forward.x,
		position.y + forward.y,
		position.z + forward.z
	};
}

void Player::giveReagent(ReagentType reagent, int count) {
	reagents[static_cast<int>(reagent)] += count;
}

bool Player::hasDraftReagents() const {
	std::array<int, REAGENT_COUNT> needed{};
	for (int i = 0; i < draft.reagentCount; ++i) {
		needed[static_cast<int>(draft.reagents[i])]++;
	}

	for (int i = 0; i < REAGENT_COUNT; ++i) {
		if (needed[i] > reagents[i]) return false;
	}

	return true;
}

bool Player::consumeDraftReagents() {
	if (!hasDraftReagents()) return false;

	for (int i = 0; i < draft.reagentCount; ++i) {
		reagents[static_cast<int>(draft.reagents[i])]--;
	}

	return true;
}

void Player::applyDamage(float amount) {
	if (amount <= 0.0f) return;

	if (wardActive && ward > 0.0f) {
		float absorbed = std::min(amount * 0.75f, ward);
		ward -= absorbed;
		amount -= absorbed;
		wardFlash = 0.25f;
	}

	health = std::max(0.0f, health - amount);
	hurtFlash = 0.25f;
}

void Player::updateWard(float dt) {
	if (wardFlash > 0.0f) wardFlash -= dt;
	if (hurtFlash > 0.0f) hurtFlash -= dt;
	if (wardCooldown > 0.0f) wardCooldown -= dt;

	if (wardActive && ward > 0.0f) {
		ward = std::max(0.0f, ward - 14.0f * dt);
		if (ward <= 0.0f) {
			wardActive = false;
			wardCooldown = 1.0f;
		}
	} else if (wardCooldown <= 0.0f) {
		ward = std::min(maxWard, ward + 18.0f * dt);
	}
}
