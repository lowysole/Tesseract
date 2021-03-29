#pragma once

#include "FileSystem/JsonValue.h"

#include <unordered_map>

class GameObject;

namespace ModelImporter {
	bool ImportModel(const char* filePath, JsonValue jMeta);
	bool SavePrefab(const char* filePath, JsonValue jMeta, GameObject* root, unsigned& resourceIndex);

	void SaveBones(GameObject* node, std::unordered_map<std::string, GameObject*>& goBones);
	void CacheBones(GameObject* node, std::unordered_map<std::string, GameObject*>& goBones);
} // namespace ModelImporter