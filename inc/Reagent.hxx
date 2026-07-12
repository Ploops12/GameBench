#pragma once

enum class ReagentType { GraveSalt, WidowNettle, MothDust, PlagueHoney, SaintAsh, BellBronze, Count };

const char* reagentName(ReagentType type);
const char* reagentShortEffect(ReagentType type);
