#pragma once

#include "Component.h"

#include "Modules/ModuleRender.h"

#include "Math/float2.h"
#include "Math/float4x4.h"
#include <unordered_map>

struct aiMesh;

class ComponentMeshRenderer : public Component {
public:
	REGISTER_COMPONENT(ComponentMeshRenderer, ComponentType::MESH_RENDERER, true);
	~ComponentMeshRenderer();

	void OnEditorUpdate() override;
	void Init() override;
	void Update() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void Start() override;

	void Draw(const float4x4& modelMatrix);
	void DrawDepthPrepass(const float4x4& modelMatrix) const;
	void DrawShadow(const float4x4& modelMatrix, unsigned int i, ShadowCasterType lightFrustumType) const;

	void UpdateMasks();
	void AddShadowCaster();
	void AddRenderingModeMask();
	void DeleteRenderingModeMask();

	void SetGameObjectBones(const std::unordered_map<std::string, GameObject*>& goBones);

	void SetMeshInternal(UID meshId);
	void SetMaterialInternal(UID materialId);

	TESSERACT_ENGINE_API UID GetMesh() const;
	TESSERACT_ENGINE_API void SetMesh(UID meshId);
	TESSERACT_ENGINE_API UID GetMaterial() const;
	TESSERACT_ENGINE_API void SetMaterial(UID materialId);

	// Dissolve
	TESSERACT_ENGINE_API void PlayDissolveAnimation(bool reverse = false);	// Plays Dissolve animation. If reverse is true, it will go from transparent to material's color, else it will go from color to transparent
	TESSERACT_ENGINE_API void ResetDissolveValues();				// Resets current timer, dissolve threshold and reverse to false
	
	TESSERACT_ENGINE_API float GetDissolveDuration() const;				// Returns material's dissolve duration
	TESSERACT_ENGINE_API float GetDissolveThreshold() const;			// Returns current dissolve threshold. From 0 to 1.
	TESSERACT_ENGINE_API bool HasDissolveAnimationFinished() const;		// Returns true if the dissolve animation has finished
	TESSERACT_ENGINE_API bool IsDissolvePlayingOnReverse() const;		// Returns true if the dissolve animation is playing on reverse

	// Tilling
	TESSERACT_ENGINE_API float2 GetTextureTiling() const;
	TESSERACT_ENGINE_API float2 GetTextureOffset() const;
	TESSERACT_ENGINE_API void SetTextureTiling(float2 _tiling);
	TESSERACT_ENGINE_API void SetTextureOffset(float2 _offset);

private:
	void UpdateDissolveAnimation();
	float GetDissolveValue() const;

	float2 ChooseTextureTiling(float2 value) const;
	float2 ChooseTextureOffset(float2 value) const;

private:
	bool bbActive = false;

	UID meshId = 0;
	UID materialId = 0;
	std::vector<float4x4> palette;

	std::unordered_map<std::string, GameObject*> goBones;

	// Dissolve variables
	float currentTime = 0.0f;
	float dissolveThreshold = 0.0f;
	bool dissolveAnimationFinished = true;
	bool dissolveAnimationReverse = false;

	// Tiling variable
	float2 textureTiling = {1.0f, 1.0f};
	float2 textureOffset = {0.0f, 0.0f};

	std::vector<float4x4> viewOrtoLightsStatic;
	std::vector<float4x4> viewOrtoLightsDynamic;
	std::vector<float4x4> viewOrtoLightsMainEntitites;

	std::vector<float4x4> projOrtoLightsStatic;
	std::vector<float4x4> projOrtoLightsDynamic;
	std::vector<float4x4> projOrtoLightsMainEntities;
	
	std::vector<float> farPlaneDistancesStatic;
	std::vector<float> farPlaneDistancesDynamic;
	std::vector<float> farPlaneDistancesMainEntities;
};
