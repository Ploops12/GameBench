#include "Actor.hxx"
#include "Animation.hxx"
#include "MainMenu.h"
#include "Menu.hxx"
#include "Object.hxx"
#include "Sprite.hxx"

#include <raylib.h>
#include <raymath.h>

#include <algorithm>

Object::Object(Vector3 initialPosition) : position(initialPosition) {
}

void Object::update(float) {
}

void Object::draw() const {
}

Vector3 Object::getPosition() const {
	return position;
}

void Object::setPosition(Vector3 newPosition) {
	position = newPosition;
}

bool Object::isActive() const {
	return active;
}

void Object::setActive(bool value) {
	active = value;
}

Actor::Actor(Vector3 initialPosition) : Object(initialPosition) {
}

void Actor::update(float deltaTime) {
	position = Vector3Add(position, Vector3Scale(velocity, deltaTime));
}

void Actor::takeDamage(float amount) {
	if (!isAlive()) return;
	currentHealth = std::max(0.0f, currentHealth - std::max(0.0f, amount));
	if (currentHealth <= 0.0f) active = false;
}

float Actor::health() const {
	return currentHealth;
}

float Actor::maxHealth() const {
	return maximumHealth;
}

bool Actor::isAlive() const {
	return active && currentHealth > 0.0f;
}

float Actor::collisionRadius() const {
	return radius;
}

Animation::Animation(int count, float duration)
	: frameCount(std::max(1, count)), frameDuration(std::max(0.001f, duration)) {
}

void Animation::reset() {
	currentFrame = 0;
	elapsed = 0.0f;
}

void Animation::update(float deltaTime) {
	if (finished()) return;
	elapsed += deltaTime;
	while (elapsed >= frameDuration && !finished()) {
		elapsed -= frameDuration;
		++currentFrame;
	}
}

int Animation::frame() const {
	return currentFrame;
}

bool Animation::finished() const {
	return currentFrame >= frameCount - 1;
}

Sprite::~Sprite() {
	unload();
}

bool Sprite::load(const char* fileName) {
	unload();
	texture = LoadTexture(fileName);
	return texture.id > 0;
}

void Sprite::unload() {
	if (texture.id > 0) {
		UnloadTexture(texture);
		texture = {};
	}
}

bool Sprite::isLoaded() const {
	return texture.id > 0;
}

void Sprite::draw(Rectangle source, Rectangle destination, Color tint) const {
	if (!isLoaded()) return;
	DrawTexturePro(texture, source, destination, {destination.width * 0.5f, destination.height * 0.5f}, 0.0f, tint);
}

int Sprite::width() const {
	return texture.width;
}

int Sprite::height() const {
	return texture.height;
}

bool Menu::isVisible() const {
	return visible;
}

void Menu::setVisible(bool value) {
	visible = value;
}

MainMenu::MainMenu() {
	visible = true;
}

void MainMenu::update(float) {
	if (visible && (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
		startRequested = true;
		visible = false;
	}
}

void MainMenu::draw() const {
	if (!visible) return;
	const int screenWidth = GetScreenWidth();
	const int screenHeight = GetScreenHeight();
	DrawRectangle(0, 0, screenWidth, screenHeight, {8, 10, 18, 255});
	DrawText("SALEM HEXSHOOTER", screenWidth / 2 - MeasureText("SALEM HEXSHOOTER", 42) / 2, screenHeight / 3, 42, {235, 238, 220, 255});
	DrawText("press ENTER to enter the hostile road", screenWidth / 2 - MeasureText("press ENTER to enter the hostile road", 20) / 2, screenHeight / 2, 20, {224, 154, 90, 255});
}

bool MainMenu::wantsToStart() const {
	return startRequested;
}
