#include "Assets.hxx"

#include <array>
#include <filesystem>
#include <string>
#include <system_error>
#include <vector>

namespace {

namespace fs = std::filesystem;

void addCandidate(std::vector<fs::path>& candidates, const fs::path& candidate) {
	if (candidate.empty()) return;
	const fs::path normalised = candidate.lexically_normal();
	for (const fs::path& existing : candidates) {
		if (existing == normalised) return;
	}
	candidates.push_back(normalised);
}

void addSearchRoots(std::vector<fs::path>& candidates, fs::path root, const fs::path& relativePath) {
	for (int depth = 0; depth < 6 && !root.empty(); ++depth) {
		addCandidate(candidates, root / relativePath);
		const fs::path parent = root.parent_path();
		if (parent == root) break;
		root = parent;
	}
}

std::string findAsset(const char* relativeName) {
	const fs::path relativePath = fs::path("resources") / "generated" / relativeName;
	std::vector<fs::path> candidates;
	addCandidate(candidates, relativePath);

	if (const char* workingDirectory = GetWorkingDirectory()) {
		addSearchRoots(candidates, fs::path(workingDirectory), relativePath);
	}
	if (const char* applicationDirectory = GetApplicationDirectory()) {
		addSearchRoots(candidates, fs::path(applicationDirectory), relativePath);
	}

	fs::path sourceFile(__FILE__);
	if (sourceFile.is_relative()) {
		if (const char* workingDirectory = GetWorkingDirectory()) {
			sourceFile = fs::path(workingDirectory) / sourceFile;
		}
	}
	addCandidate(candidates, sourceFile.parent_path().parent_path() / relativePath);

	for (const fs::path& candidate : candidates) {
		std::error_code error;
		if (fs::is_regular_file(candidate, error) && !error) return candidate.string();
	}
	return {};
}

std::string findEnemyAsset(const char* relativeName) {
	const fs::path relativePath = fs::path("resources") / "generated2" / relativeName;
	std::vector<fs::path> candidates;
	addCandidate(candidates, relativePath);

	if (const char* workingDirectory = GetWorkingDirectory()) {
		addSearchRoots(candidates, fs::path(workingDirectory), relativePath);
	}
	if (const char* applicationDirectory = GetApplicationDirectory()) {
		addSearchRoots(candidates, fs::path(applicationDirectory), relativePath);
	}

	fs::path sourceFile(__FILE__);
	if (sourceFile.is_relative()) {
		if (const char* workingDirectory = GetWorkingDirectory()) {
			sourceFile = fs::path(workingDirectory) / sourceFile;
		}
	}
	addCandidate(candidates, sourceFile.parent_path().parent_path() / relativePath);

	for (const fs::path& candidate : candidates) {
		std::error_code error;
		if (fs::is_regular_file(candidate, error) && !error) return candidate.string();
	}
	return {};
}

bool loadTexture(Texture2D& destination, const std::string& path, const char* label) {
	if (path.empty()) {
		TraceLog(LOG_WARNING, "ASSETS: Could not locate %s", label);
		return false;
	}

	destination = LoadTexture(path.c_str());
	if (!IsTextureValid(destination)) {
		TraceLog(LOG_WARNING, "ASSETS: Could not load %s from '%s'", label, path.c_str());
		destination = {};
		return false;
	}

	SetTextureFilter(destination, TEXTURE_FILTER_BILINEAR);
	return true;
}

void unloadTexture(Texture2D& texture) {
	if (texture.id != 0 && IsWindowReady()) UnloadTexture(texture);
	texture = {};
}

} // namespace

GameAssets::~GameAssets() {
	unload();
}

bool GameAssets::load() {
	unload();
	if (!IsWindowReady()) {
		TraceLog(LOG_WARNING, "ASSETS: A window must be ready before textures are loaded");
		return false;
	}

	static constexpr std::array<const char*, REAGENT_TYPE_COUNT> REAGENT_PATHS {
		"reagents/bone_fragments.png",
		"reagents/herb_bundle.png",
		"reagents/mushrooms.png",
		"reagents/bat_wings.png",
		"reagents/crystals.png",
		"reagents/eyeballs.png"
	};
	static constexpr std::array<const char*, 3> VECTOR_PATHS {
		"reagents/potion_bottle.png",
		"reagents/apple.png",
		"reagents/large_spider.png"
	};

	bool success = true;
	for (std::size_t index = 0; index < reagentIcons.size(); ++index) {
		success = loadTexture(reagentIcons[index], findAsset(REAGENT_PATHS[index]),
			REAGENT_PATHS[index]) && success;
	}
	for (std::size_t index = 0; index < vectorIcons.size(); ++index) {
		success = loadTexture(vectorIcons[index], findAsset(VECTOR_PATHS[index]),
			VECTOR_PATHS[index]) && success;
	}
	success = loadTexture(poppetIcon, findAsset("reagents/poppet.png"), "reagents/poppet.png") && success;
	success = loadTexture(torchBearerSheet,
		findEnemyAsset("enemies/torch_bearer_sheet.png"), "enemies/torch_bearer_sheet.png") && success;
	success = loadTexture(witchHunterSheet,
		findEnemyAsset("enemies/witch_hunter_sheet.png"), "enemies/witch_hunter_sheet.png") && success;
	success = loadTexture(handsSheet,
		findAsset("hands/witch_hands_sheet.png"), "hands/witch_hands_sheet.png") && success;

	if (!success) {
		unload();
		return false;
	}

	loaded = true;
	return true;
}

void GameAssets::unload() {
	for (Texture2D& texture : reagentIcons) unloadTexture(texture);
	for (Texture2D& texture : vectorIcons) unloadTexture(texture);
	unloadTexture(poppetIcon);
	unloadTexture(torchBearerSheet);
	unloadTexture(witchHunterSheet);
	unloadTexture(handsSheet);
	loaded = false;
}
