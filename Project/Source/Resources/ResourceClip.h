#pragma once
#include "Application.h"

#include "ResourceAnimation.h"
#include "Resources/ResourceType.h"
#include "Resources/Resource.h"
#include "Modules/ModuleResources.h"

#include "Utils/UID.h"
#include <string>

class ResourceClip : public Resource {
public:
	REGISTER_RESOURCE(ResourceClip, ResourceType::CLIP);

	void Load() override;
	void GetInfoJson();
	void Unload() override;
	void OnEditorUpdate() override;

	bool SaveToFile(const char* filePath);

	void Init(std::string& mName, UID mAnimationUID = 0, int mBeginIndex = 0, int mEndIndex = 0, bool mLoop = false, float mSpeed = 1.0f, UID mid = 0);

	void SetBeginIndex(int index);
	void SetEndIndex(int index);

	void SetSpeed(float mSpeed) {
		speed = mSpeed;
	}

	ResourceAnimation* GetResourceAnimation() const;

public:
	std::string name = "";
	UID animationUID = 0;
	bool loop = false;
	int beginIndex = 0;
	int endIndex = 0;
	float speed = 1.0f;
	float duration = 0;
	unsigned int keyFramesSize = 0;
};
