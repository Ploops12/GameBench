#include "Reagent.hxx"

#include <cmath>

const char* ReagentName(ReagentType type) {
	switch (type) {
		case ReagentType::GraveSalt: return "GRAVE SALT";
		case ReagentType::SaintAsh: return "SAINT ASH";
		case ReagentType::BlackWax: return "BLACK WAX";
		case ReagentType::WidowNettle: return "WIDOW NETTLE";
		case ReagentType::BellBronze: return "BELL BRONZE";
		case ReagentType::MothDust: return "MOTH DUST";
		default: return "UNKNOWN";
	}
}

const char* ReagentKey(ReagentType type) {
	static constexpr const char* keys[REAGENT_COUNT] = {"Q", "E", "G", "H", "J", "K"};
	const int index = static_cast<int>(type);
	return index >= 0 && index < REAGENT_COUNT ? keys[index] : "?";
}

Color ReagentColor(ReagentType type) {
	static constexpr Color colors[REAGENT_COUNT] = {
		{220, 220, 226, 255},
		{255, 214, 107, 255},
		{151, 91, 186, 255},
		{112, 220, 124, 255},
		{246, 145, 72, 255},
		{120, 196, 255, 255}
	};
	const int index = static_cast<int>(type);
	return index >= 0 && index < REAGENT_COUNT ? colors[index] : WHITE;
}

Reagent::Reagent(ReagentType typeValue, int amountValue)
	: type(typeValue), amount(amountValue) {
	radius = 0.35f;
}

void Reagent::DrawWorld() const {
	const Color color = ReagentColor(type);
	DrawSphere(position, 0.23f, Fade(color, 0.85f));
	DrawSphereWires(position, 0.31f, 8, 8, color);
	DrawLine3D({position.x - 0.36f, position.y, position.z}, {position.x + 0.36f, position.y, position.z}, color);
	DrawLine3D({position.x, position.y - 0.36f, position.z}, {position.x, position.y + 0.36f, position.z}, color);
}
