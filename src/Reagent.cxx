#include "Reagent.hxx"

const ReagentDefinition &getReagentDefinition(ReagentType type) {
	static const ReagentDefinition definitions[REAGENT_COUNT] = {
		{"GRAVE SALT", "SALT", "splashes", Color{220, 220, 190, 255}, 9},
		{"SAINT ASH", "ASH", "ward", Color{190, 190, 210, 255}, 9},
		{"BLACK WAX", "WAX", "seeks", Color{150, 80, 190, 255}, 9},
		{"WIDOW NETTLE", "NETTLE", "snares", Color{80, 220, 120, 255}, 9},
		{"MOTH DUST", "MOTH", "panics", Color{240, 190, 70, 255}, 9},
		{"BOG MARROW", "MARROW", "rots", Color{220, 90, 70, 255}, 9}
	};
	return definitions[static_cast<int>(type)];
}
