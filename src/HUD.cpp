#include "HUD.h"

#include <algorithm>
#include <cmath>
#include <string>

#include "Player.hxx"

namespace {
constexpr Color INK {225, 232, 222, 255};
constexpr Color DIM_INK {128, 143, 134, 255};
constexpr Color HEALTH_COLOR {224, 86, 76, 255};
constexpr Color WARD_COLOR {84, 209, 231, 255};
constexpr Color CURSE_COLOR {241, 199, 88, 255};
constexpr Color PANEL_COLOR {7, 11, 12, 210};

bool textureReady(const Texture2D& texture) {
	return texture.id != 0 && texture.width > 0 && texture.height > 0;
}

void drawMeter(Rectangle bounds, float ratio, Color color, const char* label, const std::string& value) {
	ratio = std::clamp(ratio, 0.0f, 1.0f);
	DrawRectangleRec(bounds, PANEL_COLOR);
	DrawRectangleRec(Rectangle {bounds.x + 2.0f, bounds.y + 2.0f,
		(bounds.width - 4.0f) * ratio, bounds.height - 4.0f}, Color {color.r, color.g, color.b, 170});
	DrawRectangleLinesEx(bounds, 2.0f, color);
	DrawText(label, static_cast<int>(bounds.x + 7.0f), static_cast<int>(bounds.y + 5.0f), 13, INK);
	const int valueWidth = MeasureText(value.c_str(), 13);
	DrawText(value.c_str(), static_cast<int>(bounds.x + bounds.width - valueWidth - 7.0f),
		static_cast<int>(bounds.y + 5.0f), 13, INK);
}

void drawCoatStock(const Player& player) {
	const int screenWidth = GetScreenWidth();
	const Color cyan {134, 232, 255, 255};
	const ReagentInventory& inventory = player.getInventory();
	const Rectangle satchel {static_cast<float>(screenWidth - 243), 26.0f, 219.0f, 176.0f};
	DrawRectangleRec(satchel, Fade(Color {10, 18, 30, 255}, 0.84f));
	DrawRectangleLinesEx(satchel, 1.5f, cyan);
	DrawText("COAT STOCK", screenWidth - 231, 38, 14, cyan);
	for (std::size_t index = 0; index < REAGENT_TYPE_COUNT; ++index) {
		const ReagentDefinition& definition = getReagentDefinitions()[index];
		const int row = static_cast<int>(index / 2U);
		const int column = static_cast<int>(index % 2U);
		const int x = screenWidth - 231 + column * 105;
		const int y = 63 + row * 42;
		const bool hasAny = inventory[index] > 0;
		DrawRectangleLines(x, y, 94, 31, Fade(definition.color, hasAny ? 0.85f : 0.35f));
		DrawText(definition.shortName, x + 6, y + 8, 12, definition.color);
		const std::string quantity = "x" + std::to_string(inventory[index]);
		DrawText(quantity.c_str(), x + 61, y + 7, 14, hasAny ? RAYWHITE : GRAY);
	}
}

void drawScreenCorners(Color color, float length, float thickness) {
	const float width = static_cast<float>(GetScreenWidth());
	const float height = static_cast<float>(GetScreenHeight());
	DrawLineEx(Vector2 {5.0f, 5.0f}, Vector2 {5.0f + length, 5.0f}, thickness, color);
	DrawLineEx(Vector2 {5.0f, 5.0f}, Vector2 {5.0f, 5.0f + length}, thickness, color);
	DrawLineEx(Vector2 {width - 5.0f, 5.0f}, Vector2 {width - 5.0f - length, 5.0f}, thickness, color);
	DrawLineEx(Vector2 {width - 5.0f, 5.0f}, Vector2 {width - 5.0f, 5.0f + length}, thickness, color);
	DrawLineEx(Vector2 {5.0f, height - 5.0f}, Vector2 {5.0f + length, height - 5.0f}, thickness, color);
	DrawLineEx(Vector2 {5.0f, height - 5.0f}, Vector2 {5.0f, height - 5.0f - length}, thickness, color);
	DrawLineEx(Vector2 {width - 5.0f, height - 5.0f},
		Vector2 {width - 5.0f - length, height - 5.0f}, thickness, color);
	DrawLineEx(Vector2 {width - 5.0f, height - 5.0f},
		Vector2 {width - 5.0f, height - 5.0f - length}, thickness, color);
}

void drawFallbackHands(bool coatOpen, Color color) {
	const float width = static_cast<float>(GetScreenWidth());
	const float height = static_cast<float>(GetScreenHeight());
	const float spread = coatOpen ? 94.0f : 54.0f;
	for (int side : {-1, 1}) {
		const Vector2 wrist {width * 0.5f + side * spread, height + 8.0f};
		const Vector2 palm {width * 0.5f + side * (spread + 13.0f), height - 94.0f};
		DrawLineEx(wrist, palm, 4.0f, color);
		DrawCircleLines(static_cast<int>(palm.x), static_cast<int>(palm.y), 18.0f, color);
		for (int finger = -2; finger <= 2; ++finger) {
			const float fingerSpread = finger * 7.0f;
			const Vector2 start {palm.x + side * 5.0f, palm.y - 13.0f};
			const Vector2 end {palm.x + side * (14.0f + std::abs(finger) * 2.0f) + fingerSpread,
				palm.y - 53.0f + std::abs(finger) * 5.0f};
			DrawLineEx(start, end, 2.0f, color);
		}
	}
}

void drawCoatHands(Color color) {
	const float width = static_cast<float>(GetScreenWidth());
	const float height = static_cast<float>(GetScreenHeight());
	for (int side : {-1, 1}) {
		const float edge = side < 0 ? 22.0f : width - 22.0f;
		const Vector2 wrist {edge, height + 5.0f};
		const Vector2 palm {edge - static_cast<float>(side) * 46.0f, height - 58.0f};
		DrawLineEx(wrist, palm, 4.0f, color);
		DrawCircleLines(static_cast<int>(palm.x), static_cast<int>(palm.y), 14.0f, color);
		for (int finger = -2; finger <= 2; ++finger) {
			const float offset = static_cast<float>(finger) * 5.0f;
			const Vector2 start {palm.x - static_cast<float>(side) * 4.0f, palm.y - 9.0f};
			const Vector2 end {
				palm.x - static_cast<float>(side) * (23.0f + std::abs(finger) * 2.0f) + offset,
				palm.y - 28.0f + std::abs(finger) * 4.0f
			};
			DrawLineEx(start, end, 2.0f, color);
		}
	}
}
}

void HUD::reset() {
	notice.clear();
	noticeColor = RAYWHITE;
	noticeTime = 0.0f;
	castTime = 0.0f;
	hitTime = 0.0f;
}

void HUD::update(float deltaTime) {
	noticeTime = std::max(0.0f, noticeTime - deltaTime);
	castTime = std::max(0.0f, castTime - deltaTime);
	hitTime = std::max(0.0f, hitTime - deltaTime);
	if (noticeTime <= 0.0f) notice.clear();
}

void HUD::notify(const std::string& text, Color color) {
	notice = text;
	noticeColor = color;
	noticeTime = 2.4f;
}

void HUD::triggerCast() {
	castTime = 0.22f;
}

void HUD::triggerHit() {
	hitTime = 0.16f;
}

void HUD::draw(const Player& player, const GameAssets& assets, int wave, int enemiesRemaining,
	bool coatOpen, bool mouseCaptured) const {
	const int screenWidth = GetScreenWidth();
	const int screenHeight = GetScreenHeight();
	const float centreX = screenWidth * 0.5f;
	const float centreY = screenHeight * 0.5f;

	drawHands(player, assets, coatOpen);

	if (player.getDamageFlash() > 0.0f) {
		const unsigned char alpha = static_cast<unsigned char>(80.0f *
			std::clamp(player.getDamageFlash() / 0.25f, 0.0f, 1.0f));
		drawScreenCorners(Color {245, 55, 48, alpha}, 92.0f, 5.0f);
	}
	if (player.isWardActive() || player.getWardFlash() > 0.0f) {
		const float pulse = 2.0f + 2.0f * std::sin(static_cast<float>(GetTime()) * 9.0f);
		drawScreenCorners(Color {WARD_COLOR.r, WARD_COLOR.g, WARD_COLOR.b, 205}, 70.0f, pulse);
		if (!coatOpen) {
			DrawCircleLines(static_cast<int>(centreX), static_cast<int>(centreY), 34.0f + pulse, WARD_COLOR);
		}
	}

	if (!coatOpen) {
		constexpr int crosshairGap = 5;
		constexpr int crosshairLength = 7;
		DrawLine(static_cast<int>(centreX - crosshairGap - crosshairLength), static_cast<int>(centreY),
			static_cast<int>(centreX - crosshairGap), static_cast<int>(centreY), INK);
		DrawLine(static_cast<int>(centreX + crosshairGap), static_cast<int>(centreY),
			static_cast<int>(centreX + crosshairGap + crosshairLength), static_cast<int>(centreY), INK);
		DrawLine(static_cast<int>(centreX), static_cast<int>(centreY - crosshairGap - crosshairLength),
			static_cast<int>(centreX), static_cast<int>(centreY - crosshairGap), INK);
		DrawLine(static_cast<int>(centreX), static_cast<int>(centreY + crosshairGap),
			static_cast<int>(centreX), static_cast<int>(centreY + crosshairGap + crosshairLength), INK);
		if (hitTime > 0.0f) {
			for (int side : {-1, 1}) {
				DrawLineEx(Vector2 {centreX + side * 7.0f, centreY - 7.0f},
					Vector2 {centreX + side * 14.0f, centreY - 14.0f}, 3.0f, CURSE_COLOR);
				DrawLineEx(Vector2 {centreX + side * 7.0f, centreY + 7.0f},
					Vector2 {centreX + side * 14.0f, centreY + 14.0f}, 3.0f, CURSE_COLOR);
			}
		}
	}

	const std::string healthValue = std::to_string(static_cast<int>(std::ceil(player.getHealth()))) + " / "
		+ std::to_string(static_cast<int>(std::ceil(player.getMaxHealth())));
	drawMeter(Rectangle {18.0f, static_cast<float>(screenHeight - 78), 218.0f, 25.0f},
		player.getHealthRatio(), HEALTH_COLOR, "BLOOD", healthValue);
	const std::string wardValue = std::to_string(static_cast<int>(std::ceil(player.getWard()))) + " / "
		+ std::to_string(static_cast<int>(std::ceil(player.getMaxWard())));
	drawMeter(Rectangle {18.0f, static_cast<float>(screenHeight - 47), 218.0f, 25.0f},
		player.getWardRatio(), WARD_COLOR, player.isWardActive() ? "WARD ACTIVE" : "WARD", wardValue);
	if (player.getWardLockout() > 0.0f || player.isWardBroken()) {
		const std::string lockout = player.getWardLockout() > 0.0f
			? "WARD SEALED " + std::to_string(static_cast<int>(std::ceil(player.getWardLockout()))) + "s"
			: "RELEASE RMB TO REFORM WARD";
		DrawText(lockout.c_str(), 24, screenHeight - 97, 13, HEALTH_COLOR);
	}

	if (!coatOpen) {
		drawCoatStock(player);
	}

	if (!coatOpen) {
		const CurseVector& curse = player.getPreparedSpell();
		const bool affordable = curse.canAfford(player.getInventory());
		const Color spellColor = affordable ? CURSE_COLOR : HEALTH_COLOR;
		const Rectangle spellPanel {static_cast<float>(screenWidth - 286),
			static_cast<float>(screenHeight - 121), 268.0f, 99.0f};
		DrawRectangleRec(spellPanel, PANEL_COLOR);
		DrawRectangleLinesEx(spellPanel, 2.0f, spellColor);
		DrawText("READIED CURSE", static_cast<int>(spellPanel.x + 9.0f),
			static_cast<int>(spellPanel.y + 7.0f), 11, DIM_INK);
		const std::string curseName = curse.getCurseName();
		DrawText(curseName.c_str(), static_cast<int>(spellPanel.x + 9.0f),
			static_cast<int>(spellPanel.y + 23.0f), 18, spellColor);
		if (!affordable) {
			DrawText("POPPET FALLBACK ACTIVE", static_cast<int>(spellPanel.x + 9.0f),
				static_cast<int>(spellPanel.y + 50.0f), 12, HEALTH_COLOR);
		} else if (curse.getReagentCount() == 0) {
			DrawText(curse.getType() == CurseVectorType::Poppet
				? "NO COST // ALWAYS READY" : "NO REAGENT DOSES",
				static_cast<int>(spellPanel.x + 9.0f), static_cast<int>(spellPanel.y + 49.0f),
				12, DIM_INK);
		} else {
			for (int i = 0; i < curse.getReagentCount(); ++i) {
				const ReagentType type = curse.getReagent(i);
				const ReagentDefinition& definition = getReagentDefinition(type);
				const float slotX = spellPanel.x + 10.0f + i * 81.0f;
				DrawCircle(static_cast<int>(slotX + 6.0f),
					static_cast<int>(spellPanel.y + 56.0f), 5.0f, definition.color);
				DrawText(definition.shortName, static_cast<int>(slotX + 15.0f),
					static_cast<int>(spellPanel.y + 50.0f), 11, INK);
			}
		}
		DrawText("LMB CAST  //  TAB COAT", static_cast<int>(spellPanel.x + 9.0f),
			static_cast<int>(spellPanel.y + 76.0f), 12, INK);
	}

	const std::string waveText = "WAVE " + std::to_string(wave) + "  //  HOSTILES "
		+ std::to_string(std::max(0, enemiesRemaining));
	const int waveWidth = MeasureText(waveText.c_str(), 15);
	DrawRectangle(screenWidth / 2 - waveWidth / 2 - 10, 12, waveWidth + 20, 27, PANEL_COLOR);
	DrawRectangleLines(screenWidth / 2 - waveWidth / 2 - 10, 12, waveWidth + 20, 27, DIM_INK);
	DrawText(waveText.c_str(), screenWidth / 2 - waveWidth / 2, 18, 15, INK);

	if (!coatOpen && noticeTime > 0.0f && !notice.empty()) {
		const int noticeWidth = MeasureText(notice.c_str(), 18);
		const int noticeX = screenWidth / 2 - noticeWidth / 2;
		DrawRectangle(noticeX - 12, 54, noticeWidth + 24, 31, PANEL_COLOR);
		DrawRectangleLines(noticeX - 12, 54, noticeWidth + 24, 31, noticeColor);
		DrawText(notice.c_str(), noticeX, 61, 18, noticeColor);
	}

	if (!mouseCaptured && !coatOpen) {
		const char* message = "MOUSE RELEASED // ESC TO CAPTURE";
		const int messageWidth = MeasureText(message, 16);
		DrawRectangle(screenWidth / 2 - messageWidth / 2 - 10, screenHeight / 2 + 42,
			messageWidth + 20, 30, PANEL_COLOR);
		DrawRectangleLines(screenWidth / 2 - messageWidth / 2 - 10, screenHeight / 2 + 42,
			messageWidth + 20, 30, HEALTH_COLOR);
		DrawText(message, screenWidth / 2 - messageWidth / 2, screenHeight / 2 + 49, 16, INK);
	}
}

void HUD::drawHands(const Player& player, const GameAssets& assets, bool coatOpen) const {
	const Color fallbackColor = player.isWardActive() ? WARD_COLOR : INK;
	if (coatOpen) {
		drawCoatHands(fallbackColor);
		return;
	}
	if (!textureReady(assets.handsSheet)) {
		drawFallbackHands(coatOpen, fallbackColor);
		return;
	}

	constexpr int columns = 8;
	constexpr int rows = 3;
	const float frameWidth = assets.handsSheet.width / static_cast<float>(columns);
	const float frameHeight = assets.handsSheet.height / static_cast<float>(rows);
	int row = 0;
	int frame = 0;
	if (player.isWardActive() || player.getWardFlash() > 0.0f) {
		row = 2;
		frame = static_cast<int>(GetTime() * 10.0) % columns;
	} else if (castTime > 0.0f) {
		row = 1;
		const float castProgress = 1.0f - std::clamp(castTime / 0.22f, 0.0f, 1.0f);
		frame = std::clamp(static_cast<int>(castProgress * columns), 0, columns - 1);
	} else if (coatOpen) {
		frame = 1 + static_cast<int>(GetTime() * 5.0) % 2;
	}

	const Rectangle source {frame * frameWidth, row * frameHeight, frameWidth, frameHeight};
	const float destinationWidth = std::min(GetScreenWidth() * (coatOpen ? 0.4f : 0.5f), 410.0f);
	const float destinationHeight = destinationWidth * frameHeight / frameWidth;
	const Rectangle destination {
		(GetScreenWidth() - destinationWidth) * 0.5f,
		GetScreenHeight() - destinationHeight + (coatOpen ? 88.0f : 56.0f),
		destinationWidth,
		destinationHeight
	};
	DrawTexturePro(assets.handsSheet, source, destination, Vector2 {}, 0.0f, WHITE);
}
