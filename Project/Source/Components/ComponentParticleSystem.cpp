#include "ComponentParticleSystem.h"

#include "Application.h"
#include "GameObject.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModulePhysics.h"
#include "Modules/ModuleScene.h"
#include "Panels/PanelScene.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourceShader.h"
#include "FileSystem/JsonValue.h"
#include "Utils/ImGuiUtils.h"
#include "Utils/ParticleMotionState.h"
#include "Scene.h"

#include "Math/float3x3.h"
#include "Math/TransformOps.h"
#include "Geometry/Plane.h"
#include "Geometry/Line.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_color_gradient.h"
#include "GL/glew.h"
#include "debugdraw.h"
#include <random>

#include "Utils/Leaks.h"

// Gizmo
#define JSON_TAG_DRAW_GIZMO "DrawGizmo"

// Particle System
#define JSON_TAG_DURATION "Duration"
#define JSON_TAG_LOOPING "Looping"
#define JSON_TAG_START_DELAY_RM "StartDelayRM"
#define JSON_TAG_START_DELAY "StartDelay"
#define JSON_TAG_LIFE_RM "LifeRM"
#define JSON_TAG_LIFE "Life"
#define JSON_TAG_SPEED_RM "SpeedRM"
#define JSON_TAG_SPEED "Speed"
#define JSON_TAG_ROTATION_RM "RotationRM"
#define JSON_TAG_ROTATION "Rotation"
#define JSON_TAG_SCALE_RM "ScaleRM"
#define JSON_TAG_SCALE "Scale"
#define JSON_TAG_REVERSE_EFFECT "ReverseEffect"
#define JSON_TAG_REVERSE_DISTANCE_RM "ReverseDistanceRM"
#define JSON_TAG_REVERSE_DISTANCE "ReverseDistance"
#define JSON_TAG_MAX_PARTICLE "MaxParticle"
#define JSON_TAG_PLAY_ON_AWAKE "PlayOnAwake"

// Emision
#define JSON_TAG_ATTACH_EMITTER "AttachEmitter"
#define JSON_TAG_PARTICLES_SECOND_RM "ParticlesPerSecondRM"
#define JSON_TAG_PARTICLES_SECOND "ParticlesPerSecond"

// Gravity
#define JSON_TAG_GRAVITY_EFFECT "GravityEffect"
#define JSON_TAG_GRAVITY_FACTOR_RM "GravityFactorRM"
#define JSON_TAG_GRAVITY_FACTOR "GravityFactor"

// Shape
#define JSON_TAG_EMITTER_TYPE "ParticleEmitterType"
#define JSON_TAG_CONE_RADIUS_UP "ConeRadiusUp"
#define JSON_TAG_CONE_RADIUS_DOWN "ConeRadiusDown"
#define JSON_TAG_RANDOM_CONE_RADIUS_UP "RandomConeRadiusUp"
#define JSON_TAG_RANDOM_CONE_RADIUS_DOWN "RandomConeRadiusDown"

// Rotation over Lifetime
#define JSON_TAG_ROTATION_OVER_LIFETIME "RotationOverLifetime"
#define JSON_TAG_ROTATION_FACTOR_RM "RotationFactorRM"
#define JSON_TAG_ROTATION_FACTOR "RotationFactor"

// Size over Lifetime
#define JSON_TAG_SIZE_OVER_LIFETIME "SizeOverLifetime"
#define JSON_TAG_SCALE_FACTOR_RM "ScaleFactorRM"
#define JSON_TAG_SCALE_FACTOR "ScaleFactor"

// Color over Lifetime
#define JSON_TAG_COLOR_OVER_LIFETIME "ColorOverLifetime"
#define JSON_TAG_NUMBER_COLORS "NumberColors"
#define JSON_TAG_GRADIENT_COLORS "GradientColors"

// Texture Sheet Animation
#define JSON_TAG_YTILES "Ytiles"
#define JSON_TAG_XTILES "Xtiles"
#define JSON_TAG_ANIMATION_SPEED "AnimationSpeed"
#define JSON_TAG_IS_RANDOM_FRAME "IsRandomFrame"
#define JSON_TAG_lOOP_ANIMATION "LoopAnimation"
#define JSON_TAG_N_CYCLES "NCycles"

// Render
#define JSON_TAG_TEXTURE_TEXTURE_ID "TextureId"
#define JSON_TAG_BILLBOARD_TYPE "BillboardType"
#define JSON_TAG_PARTICLE_RENDER_MODE "ParticleRenderMode"
#define JSON_TAG_PARTICLE_RENDER_ALIGNMENT "ParticleRenderAlignment"
#define JSON_TAG_FLIP_TEXTURE "FlipTexture"
#define JSON_TAG_IS_SOFT "IsSoft"
#define JSON_TAG_SOFT_RANGE "SoftRange"

// Collision
#define JSON_TAG_HAS_COLLISION "HasCollision"
#define JSON_TAG_COLLISION_RADIUS "CollRadius"
#define JSON_TAG_LAYER_INDEX "LayerIndex"

// Trail
#define JSON_TAG_HASTRAIL "HasTrail"
#define JSON_TAG_TRAIL_RATIO "TrailRatio"
#define JSON_TAG_TRAIL_WIDTH_RM "WidthRM"
#define JSON_TAG_TRAIL_WIDTH "Width"
#define JSON_TAG_TRAIL_QUADS_RM "TrailQuadsRM"
#define JSON_TAG_TRAIL_QUADS "TrailQuads"
#define JSON_TAG_TRAIL_QUAD_LIFE_RM "QuadLifeRM"
#define JSON_TAG_TRAIL_QUAD_LIFE "QuadLife"
#define JSON_TAG_TRAIL_TEXTURE_TEXTUREID "TextureTrailID"
#define JSON_TAG_TRAIL_FLIP_TEXTURE "FlipTrailTexture"
#define JSON_TAG_TRAIL_TEXTURE_REPEATS "TextureRepeats"
#define JSON_TAG_HAS_COLOR_OVER_TRAIL "HasColorOverTrail"
#define JSON_TAG_NUMBER_COLORS_TRAIL "NumberColorsTrail"
#define JSON_TAG_GRADIENT_COLORS_TRAIL "GradientColorsTrail"

// Sub Emitter
#define JSON_TAG_SUB_EMITTERS "SubEmitters"
#define JSON_TAG_SUB_EMITTERS_NUMBER "SubEmittersNumber"
#define JSON_TAG_SUB_EMITTERS_GAMEOBJECT "GameObjectUID"
#define JSON_TAG_SUB_EMMITERS_EMITTER_TYPE "EmitterType"
#define JSON_TAG_SUB_EMITTERS_EMIT_PROB "EmitProbability"

// Lights
#define JSON_TAG_HAS_LIGHTS "HasLights"
#define JSON_TAG_LIGHT_GAMEOBJECT "LightGameObjectUID"
#define JSON_TAG_LIGHTS_RATIO "LightsRatio"
#define JSON_TAG_LIGHTS_OFFSET "LightsOffset"
#define JSON_TAG_LIGHTS_INTENSITY_MULTIPLIER_RM "IntensityMultiplierRM"
#define JSON_TAG_LIGHTS_INTENSITY_MULTIPLIER "IntensityMultiplier"
#define JSON_TAG_LIGHTS_RANGE_MULTIPLIER_RM "RangeMultiplierRM"
#define JSON_TAG_LIGHTS_RANGE_MULTIPLIER "RangeMultiplier"
#define JSON_TAG_LIGHTS_USE_PARTICLE_COLOR "UseParticleColor"
#define JSON_TAG_LIGHTS_USE_CUSTOM_COLOR "UseCustomColor"
#define JSON_TAG_LIGHTS_NUMBER_COLORS "NumberColorsLights"
#define JSON_TAG_LIGHTS_GRADIENT_COLORS "GradientColorsLights"
#define JSON_TAG_LIGHTS_MAX_LIGHTS "MaxLights"

#define ITEM_SIZE 150

static bool ImGuiRandomMenu(const char* name, float2& values, RandomMode& mode, float speed = 0.01f, float min = 0, float max = inf, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
	ImGui::PushID(name);
	bool used = false;
	if (mode == RandomMode::CONST) {
		used = ImGui::DragFloat(name, &values[0], App->editor->dragSpeed2f, min, max, format, flags);
	} else if (mode == RandomMode::CONST_MULT) {
		used = ImGui::DragFloat2(name, &values[0], App->editor->dragSpeed2f, min, max, format, flags);
	}
	if (used && values[0] > values[1]) {
		values[1] = values[0];
	}

	const char* randomModes[] = {"Constant", "Random Between Two Constants"};
	const char* randomModesCurrent = randomModes[(int) mode];
	ImGui::SameLine();
	if (ImGui::BeginCombo("##", randomModesCurrent, ImGuiComboFlags_NoPreview)) {
		for (int n = 0; n < IM_ARRAYSIZE(randomModes); ++n) {
			bool isSelected = (randomModesCurrent == randomModes[n]);
			if (ImGui::Selectable(randomModes[n], isSelected)) {
				mode = (RandomMode) n;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopID();
	return used;
};

static float ObtainRandomValueFloat(float2& values, RandomMode& mode) {
	if (mode == RandomMode::CONST_MULT) {
		return rand() / (float) RAND_MAX * (values[1] - values[0]) + values[0];
	} else {
		return values[0];
	}
}

static bool IsProbably(float probablility) {
	return float(rand()) / float(RAND_MAX) <= probablility;
}

ComponentParticleSystem::~ComponentParticleSystem() {
	RELEASE(gradient);
	RELEASE(gradientTrail);
	RELEASE(gradientLight);

	UndertakerParticle(true);
	for (SubEmitter* subEmitter : subEmitters) {
		RELEASE(subEmitter);
	}
	subEmitters.clear();
	subEmittersGO.clear();
}

void ComponentParticleSystem::Init() {
	if (!gradient) gradient = new ImGradient();
	if (!gradientTrail) gradientTrail = new ImGradient();
	if (!gradientLight) gradientLight = new ImGradient();
	layer = WorldLayers(1 << layerIndex);
	AllocateParticlesMemory();
	isStarted = false;
}

void ComponentParticleSystem::Start() {
	if (subEmitters.size() > 0) {
		int pos = 0;
		for (SubEmitter* subEmitter : subEmitters) {
			GameObject* gameObject = App->scene->scene->GetGameObject(subEmitter->gameObjectUID);
			if (gameObject != nullptr) {
				ComponentParticleSystem* particleSystem = gameObject->GetComponent<ComponentParticleSystem>();
				if (particleSystem != nullptr) {
					subEmitter->particleSystem = particleSystem;
				} else {
					subEmitters.erase(subEmitters.begin() + pos);
				}
			} else {
				subEmitters.erase(subEmitters.begin() + pos);
			}
			pos += 1;
		}
	}
	if (lightGameObjectUID != 0) {
		GameObject* gameObject = App->scene->scene->GetGameObject(lightGameObjectUID);
		if (gameObject != nullptr) {
			ComponentLight* light = gameObject->GetComponent<ComponentLight>();
			if (light == nullptr || light->lightType != LightType::POINT) {
				lightGameObjectUID = 0;
				lightComponent = nullptr;
			} else {
				lightComponent = light;
			}
		} else {
			lightGameObjectUID = 0;
			lightComponent = nullptr;
		}
	}
}

void ComponentParticleSystem::OnEditorUpdate() {
	if (ImGui::Checkbox("Active", &active)) {
		if (GetOwner().IsActive()) {
			if (active) {
				Enable();
			} else {
				Disable();
			}
		}
	}
	ImGui::Separator();
	ImGui::Checkbox("Draw Gizmo", &drawGizmo);
	ImGui::Separator();

	ImGui::Indent();
	ImGui::PushItemWidth(ITEM_SIZE);

	// General Particle System
	if (ImGui::CollapsingHeader("Particle System")) {
		ImGui::DragFloat("Duration", &duration, App->editor->dragSpeed2f, 0, inf);
		ImGui::Checkbox("Loop", &looping);

		ImGuiRandomMenu("Start Delay", startDelay, startDelayRM, App->editor->dragSpeed2f, 0, inf);
		if (isPlaying && restDelayTime > 0) {
			ImGui::Indent();
			ImGui::DragFloat("Rest Time", &restDelayTime, App->editor->dragSpeed2f, 0, inf, "%.3f", ImGuiSliderFlags_NoInput);
			ImGui::Unindent();
		}

		ImGuiRandomMenu("Start Life", life, lifeRM);
		if (ImGuiRandomMenu("Start Speed", speed, speedRM)) {
			AllocateParticlesMemory();
		}
		float2 rotDegree = -rotation * RADTODEG;
		if (ImGuiRandomMenu("Start Rotation", rotDegree, rotationRM, App->editor->dragSpeed1f, -inf, inf)) {
			rotation = -rotDegree * DEGTORAD;
		}
		ImGuiRandomMenu("Start Size", scale, scaleRM, App->editor->dragSpeed3f);
		ImGui::Checkbox("Reverse Effect", &reverseEffect);
		if (reverseEffect) {
			ImGui::Indent();
			ImGuiRandomMenu("Distance", reverseDistance, reverseDistanceRM);
			ImGui::Unindent();
		}
		if (ImGui::DragScalar("Max Particles", ImGuiDataType_U32, &maxParticles)) {
			AllocateParticlesMemory();
		}
		ImGui::Checkbox("Play On Awake", &playOnAwake);
	}

	// Emission
	if (ImGui::CollapsingHeader("Emission")) {
		ImGui::Checkbox("Attach to Emitter", &attachEmitter);
		if (ImGuiRandomMenu("Rate over Time", particlesPerSecond, particlesPerSecondRM, 1, 0, inf)) {
			InitStartRate();
		}
	}

	// Gravity
	if (ImGui::CollapsingHeader("Gravity")) {
		ImGui::Checkbox("##gravity_effect", &gravityEffect);
		if (gravityEffect) {
			ImGui::SameLine();
			ImGuiRandomMenu("Gravity##gravity_factor", gravityFactor, gravityFactorRM, App->editor->dragSpeed2f, 0, inf);
		}
	}

	// Shape
	if (ImGui::CollapsingHeader("Shape")) {
		const char* emitterTypeCombo[] = {"Cone", "Sphere", "Hemisphere", "Donut", "Circle", "Rectangle"};
		const char* emitterTypeComboCurrent = emitterTypeCombo[(int) emitterType];
		ImGui::TextColored(App->editor->textColor, "Shape");
		if (ImGui::BeginCombo("##Shape", emitterTypeComboCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(emitterTypeCombo); ++n) {
				bool isSelected = (emitterTypeComboCurrent == emitterTypeCombo[n]);
				if (ImGui::Selectable(emitterTypeCombo[n], isSelected)) {
					emitterType = (ParticleEmitterType) n;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (emitterType == ParticleEmitterType::CONE) {
			ImGui::Checkbox("Random##random_cone_radius_up", &randomConeRadiusUp);
			ImGui::SameLine();
			ImGui::DragFloat("Radius Up", &coneRadiusUp, App->editor->dragSpeed2f, 0, inf);

			ImGui::Checkbox("Random##random_cone_radius_down", &randomConeRadiusDown);
			ImGui::SameLine();
			ImGui::DragFloat("Radius Down", &coneRadiusDown, App->editor->dragSpeed2f, 0, inf);
		}
	}

	// Rotation over Lifetime
	if (ImGui::CollapsingHeader("Rotation over Lifetime")) {
		ImGui::Checkbox("##rot_over_lifetime", &rotationOverLifetime);
		if (rotationOverLifetime) {
			ImGui::SameLine();
			float2 rotDegree = -rotationFactor * RADTODEG;
			if (ImGuiRandomMenu("Rotation Factor", rotDegree, rotationFactorRM, App->editor->dragSpeed1f, -inf, inf)) {
				rotationFactor = -rotDegree * DEGTORAD;
			}
		}
	}

	// Size over Lifetime
	if (ImGui::CollapsingHeader("Size over Lifetime")) {
		ImGui::Checkbox("##size_over_lifetime", &sizeOverLifetime);
		if (sizeOverLifetime) {
			ImGui::SameLine();
			ImGuiRandomMenu("Scale Factor", scaleFactor, scaleFactorRM, App->editor->dragSpeed3f, -inf, inf);
		}
	}

	// Color over Lifetime
	if (ImGui::CollapsingHeader("Color over Lifetime")) {
		ImGui::Checkbox("##color_over_lifetime", &colorOverLifetime);
		if (colorOverLifetime) {
			ImGui::SameLine();
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(200);
			ImGui::GradientEditor(gradient, draggingGradient, selectedGradient);
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(ITEM_SIZE);
		}
	}

	// Texture Sheet Animation
	if (ImGui::CollapsingHeader("Texture Sheet Animation")) {
		ImGui::DragScalar("X Tiles", ImGuiDataType_U32, &Xtiles);
		ImGui::DragScalar("Y Tiles", ImGuiDataType_U32, &Ytiles);
		ImGui::Checkbox("Loop Animation", &loopAnimation);
		if (loopAnimation) {
			ImGui::DragFloat("Animation Speed", &animationSpeed, App->editor->dragSpeed2f, -inf, inf);
		} else {
			ImGui::DragFloat("Cycles", &nCycles, App->editor->dragSpeed2f, 1, inf);
		}
		ImGui::Checkbox("Random Frame", &isRandomFrame);
	}

	// Render
	if (ImGui::CollapsingHeader("Render")) {
		ImGui::SetNextItemWidth(200);
		const char* billboardTypeCombo[] = {"Billboard", "Stretched Billboard", "Horitzontal Billboard", "Vertical Billboard"};
		const char* billboardTypeComboCurrent = billboardTypeCombo[(int) billboardType];
		if (ImGui::BeginCombo("Bilboard Mode##", billboardTypeComboCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(billboardTypeCombo); ++n) {
				bool isSelected = (billboardTypeComboCurrent == billboardTypeCombo[n]);
				if (ImGui::Selectable(billboardTypeCombo[n], isSelected)) {
					billboardType = (BillboardType) n;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (billboardType == BillboardType::NORMAL) {
			ImGui::Indent();
			const char* renderAlignmentCombo[] = {"View", "World", "Local", "Facing", "Velocity"};
			const char* renderAlignmentComboCurrent = renderAlignmentCombo[(int) renderAlignment];
			if (ImGui::BeginCombo("Render Alignment##", renderAlignmentComboCurrent)) {
				for (int n = 0; n < IM_ARRAYSIZE(renderAlignmentCombo); ++n) {
					bool isSelected = (renderAlignmentComboCurrent == renderAlignmentCombo[n]);
					if (ImGui::Selectable(renderAlignmentCombo[n], isSelected)) {
						renderAlignment = (ParticleRenderAlignment) n;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Unindent();
		}
		const char* renderModeCombo[] = {"Additive", "Transparent"};
		const char* renderModeComboCurrent = renderModeCombo[(int) renderMode];
		if (ImGui::BeginCombo("Render Mode##", renderModeComboCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(renderModeCombo); ++n) {
				bool isSelected = (renderModeComboCurrent == renderModeCombo[n]);
				if (ImGui::Selectable(renderModeCombo[n], isSelected)) {
					renderMode = (ParticleRenderMode) n;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::ResourceSlot<ResourceTexture>("texture", &textureID);

		ImGui::NewLine();
		ImGui::Text("Flip: ");
		ImGui::SameLine();
		ImGui::Checkbox("X", &flipTexture[0]);
		ImGui::SameLine();
		ImGui::Checkbox("Y", &flipTexture[1]);

		ImGui::NewLine();
		ImGui::Text("Soft: ");
		ImGui::SameLine();
		ImGui::Checkbox("##soft", &isSoft);
		ImGui::DragFloat("Softness Range", &softRange, App->editor->dragSpeed2f, 0.0f, inf);
	}

	// Collision
	if (ImGui::CollapsingHeader("Collision")) {
		ImGui::Checkbox("##collision", &collision);
		if (collision) {
			ImGui::Indent();

			// World Layers combo box
			const char* layerTypeItems[] = {"No Collision", "Event Triggers", "World Elements", "Player", "Enemy", "Bullet", "Bullet Enemy", "Skills", "Everything"};
			const char* layerCurrent = layerTypeItems[layerIndex];
			if (ImGui::BeginCombo("Layer", layerCurrent)) {
				for (int n = 0; n < IM_ARRAYSIZE(layerTypeItems); ++n) {
					if (ImGui::Selectable(layerTypeItems[n])) {
						layerIndex = n;
						if (n == 7) {
							layer = WorldLayers::EVERYTHING;
						} else {
							layer = WorldLayers(1 << layerIndex);
						}
					}
				}
				ImGui::EndCombo();
			}
			ImGui::DragFloat("Radius", &radius, App->editor->dragSpeed2f, 0, inf);
			ImGui::Unindent();
		}
	}

	// Trail
	if (ImGui::CollapsingHeader("Trail")) {
		if (ImGui::Checkbox("##Trail", &hasTrail)) {
			if (isPlaying) {
				Stop();
				Play();
			}
		}
		if (hasTrail) {
			ImGui::Indent();
			ImGui::DragFloat("Ratio", &trailRatio, App->editor->dragSpeed2f, 0, 1, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			if (ImGuiRandomMenu("Width", width, widthRM, App->editor->dragSpeed2f, 0, inf)) {
				for (Particle& particle : particles) {
					if (particle.trail != nullptr) {
						particle.trail->width = ObtainRandomValueFloat(width, widthRM);
					}
				}
			}
			if (ImGui::DragFloat("Vertex Distance", &distanceVertex, App->editor->dragSpeed2f, 0, 10, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
				for (Particle& particle : particles) {
					if (particle.trail != nullptr) {
						particle.trail->vertexDistance = distanceVertex;
					}
				}
			}
			ImGuiRandomMenu("Num Quads (Length)", trailQuads, trailQuadsRM, 1.0f, 1, 50, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			ImGuiRandomMenu("Quad Life", quadLife, quadLifeRM, App->editor->dragSpeed2f, 1, inf);

			ImGui::Checkbox("Color Over Trail", &colorOverTrail);
			if (colorOverTrail) {
				ImGui::PopItemWidth();
				ImGui::PushItemWidth(200);
				ImGui::GradientEditor(gradientTrail, draggingGradientTrail, selectedGradientTrail);
				ImGui::PopItemWidth();
				ImGui::PushItemWidth(ITEM_SIZE);
			}

			ImGui::NewLine();
			ImGui::ResourceSlot<ResourceTexture>("texture", &textureTrailID);
			ImGui::Text("Flip: ");
			ImGui::SameLine();
			if (ImGui::Checkbox("X##flip_trail", &flipTrailTexture[0])) {
				for (Particle& particle : particles) {
					if (particle.trail != nullptr) {
						particle.trail->flipTexture[0] = flipTrailTexture[0];
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Y##flip_trail", &flipTrailTexture[1])) {
				for (Particle& particle : particles) {
					if (particle.trail != nullptr) {
						particle.trail->flipTexture[1] = flipTrailTexture[1];
					}
				}
			}
			ImGui::DragInt("Texture Repeats", &nTextures, 1.0f, 1, inf, "%d", ImGuiSliderFlags_AlwaysClamp);

			ImGui::Unindent();
		}
	}

	// Sub Emitter
	if (ImGui::CollapsingHeader("Sub Emitter")) {
		if (subEmitters.size() <= 0) {
			ImGui::NewLine();
			std::string addSubEmmiter = std::string(ICON_FA_PLUS);
			if (ImGui::Button(addSubEmmiter.c_str())) {
				SubEmitter* subEmitter = new SubEmitter();
				subEmitters.push_back(subEmitter);
			}
		}

		int position = 0;
		for (SubEmitter* subEmitter : subEmitters) {
			ImGui::PushID(subEmitter);
			UID oldUI = subEmitter->gameObjectUID;
			ImGui::BeginColumns("", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			{
				ImGui::GameObjectSlot("", &subEmitter->gameObjectUID);
				if (oldUI != subEmitter->gameObjectUID) {
					GameObject* gameObject = App->scene->scene->GetGameObject(subEmitter->gameObjectUID);
					if (gameObject != nullptr) {
						ComponentParticleSystem* particleSystem = gameObject->GetComponent<ComponentParticleSystem>();
						if (particleSystem == nullptr) {
							subEmitter->gameObjectUID = 0;
						} else {
							subEmitter->particleSystem = particleSystem;
							subEmitter->particleSystem->active = false;
						}
					}
				}
			}
			ImGui::NextColumn();
			{
				ImGui::NewLine();
				const char* subEmitterTypes[] = {"Birth", "Collision", "Death"};
				const char* subEmitterTypesCurrent = subEmitterTypes[(int) subEmitter->subEmitterType];
				if (ImGui::BeginCombo("", subEmitterTypesCurrent)) {
					for (int n = 0; n < IM_ARRAYSIZE(subEmitterTypes); ++n) {
						bool isSelected = (subEmitterTypesCurrent == subEmitterTypes[n]);
						if (ImGui::Selectable(subEmitterTypes[n], isSelected)) {
							subEmitter->subEmitterType = (SubEmitterType) n;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::SetNextItemWidth(75);
				ImGui::DragFloat("Emit Probability", &subEmitter->emitProbability, App->editor->dragSpeed2f, 0, 1, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			}
			ImGui::EndColumns();

			std::string addSubEmmiter = std::string(ICON_FA_PLUS);
			if (ImGui::Button(addSubEmmiter.c_str())) {
				SubEmitter* subEmitter = new SubEmitter();
				subEmitters.push_back(subEmitter);
			}
			ImGui::SameLine();
			std::string removeSubEmmiter = std::string(ICON_FA_MINUS);
			if (ImGui::Button(removeSubEmmiter.c_str())) {
				subEmitters.erase(subEmitters.begin() + position);
				position -= 1;
			}
			position += 1;
			ImGui::PopID();
			ImGui::NewLine();
		}
	}

	// Lights
	if (ImGui::CollapsingHeader("Lights")) {
		ImGui::Checkbox("##lights", &hasLights);
		if (hasLights) {
			ImGui::Indent();
			UID oldUID = lightGameObjectUID;
			ImGui::GameObjectSlot("Point Light", &lightGameObjectUID);
			if (oldUID != lightGameObjectUID) {
				GameObject* gameObject = App->scene->scene->GetGameObject(lightGameObjectUID);
				if (gameObject != nullptr) {
					ComponentLight* light = gameObject->GetComponent<ComponentLight>();
					if (light == nullptr || light->lightType != LightType::POINT) {
						lightGameObjectUID = 0;
						lightComponent = nullptr;
					} else {
						lightComponent = light;
					}
				} else {
					lightGameObjectUID = 0;
					lightComponent = nullptr;
				}
			}
			ImGui::NewLine();
			ImGui::DragFloat("Ratio##lights_ratio", &lightsRatio, App->editor->dragSpeed2f, 0, 1, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGuiRandomMenu("Intensity Multiplier", intensityMultiplier, intensityMultiplierRM, App->editor->dragSpeed2f, 0, 10, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGuiRandomMenu("Range Multiplier", rangeMultiplier, rangeMultiplierRM, App->editor->dragSpeed2f, 0, 10, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			if (ImGui::Checkbox("Use Particle Color", &useParticleColor)) {
				if (useParticleColor && useCustomColor) {
					useCustomColor = false;
				}
			}
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			if (ImGui::Checkbox("Use Custom Color", &useCustomColor)) {
				if (useCustomColor && useParticleColor) {
					useParticleColor = false;
				}
			}
			if (useCustomColor) {
				ImGui::PopItemWidth();
				ImGui::PushItemWidth(200);
				ImGui::GradientEditor(gradientLight, draggingGradientLight, selectedGradientLight);
				ImGui::PopItemWidth();
				ImGui::PushItemWidth(ITEM_SIZE);
				ImGui::NewLine();
			}
			ImGui::DragInt("Max Lights", &maxLights, 1.0f, 0, 10, "%d", ImGuiSliderFlags_AlwaysClamp);

			ImGui::NewLine();
			ImGui::SetNextItemWidth(200);
			ImGui::DragFloat3("Local Offset (X, Y, Z)", lightOffset.ptr(), App->editor->dragSpeed2f, inf, inf, "%.2f");

			ImGui::Unindent();
		}
	}

	ImGui::NewLine();

	// Texture Preview
	if (ImGui::CollapsingHeader("Texture Preview")) {
		ResourceTexture* textureResource = App->resources->GetResource<ResourceTexture>(textureID);
		if (textureResource != nullptr) {
			int width;
			int height;
			glGetTextureLevelParameteriv(textureResource->glTexture, 0, GL_TEXTURE_WIDTH, &width);
			glGetTextureLevelParameteriv(textureResource->glTexture, 0, GL_TEXTURE_HEIGHT, &height);

			ImGui::TextColored(App->editor->titleColor, "Particle Texture");
			ImGui::TextWrapped("Size:");
			ImGui::SameLine();
			ImGui::TextWrapped("%i x %i", width, height);
			ImGui::Image((void*) textureResource->glTexture, ImVec2(200, 200));
			ImGui::NewLine();
		}

		ResourceTexture* trailTextureResource = App->resources->GetResource<ResourceTexture>(textureTrailID);
		if (trailTextureResource != nullptr) {
			int width;
			int height;
			glGetTextureLevelParameteriv(trailTextureResource->glTexture, 0, GL_TEXTURE_WIDTH, &width);
			glGetTextureLevelParameteriv(trailTextureResource->glTexture, 0, GL_TEXTURE_HEIGHT, &height);

			ImGui::TextColored(App->editor->titleColor, "Trail Texture");
			ImGui::TextWrapped("Size:");
			ImGui::SameLine();
			ImGui::TextWrapped("%d x %d", width, height);
			ImGui::Image((void*) trailTextureResource->glTexture, ImVec2(200, 200));
			ImGui::NewLine();
		}
	}
	ImGui::Unindent();
	ImGui::PopItemWidth();
}

void ComponentParticleSystem::Load(JsonValue jComponent) {
	// Gizmo
	drawGizmo = jComponent[JSON_TAG_DRAW_GIZMO];

	// Particle System
	duration = jComponent[JSON_TAG_DURATION];
	looping = jComponent[JSON_TAG_LOOPING];
	startDelayRM = (RandomMode)(int) jComponent[JSON_TAG_START_DELAY];
	JsonValue jStartDelay = jComponent[JSON_TAG_START_DELAY];
	startDelay[0] = jStartDelay[0];
	startDelay[1] = jStartDelay[1];
	lifeRM = (RandomMode)(int) jComponent[JSON_TAG_LIFE_RM];
	JsonValue jLife = jComponent[JSON_TAG_LIFE];
	life[0] = jLife[0];
	life[1] = jLife[1];
	speedRM = (RandomMode)(int) jComponent[JSON_TAG_SPEED_RM];
	JsonValue jSpeed = jComponent[JSON_TAG_SPEED];
	speed[0] = jSpeed[0];
	speed[1] = jSpeed[1];
	rotationRM = (RandomMode)(int) jComponent[JSON_TAG_ROTATION_RM];
	JsonValue jRotation = jComponent[JSON_TAG_ROTATION];
	rotation[0] = jRotation[0];
	rotation[1] = jRotation[1];
	scaleRM = (RandomMode)(int) jComponent[JSON_TAG_SCALE_RM];
	JsonValue jScale = jComponent[JSON_TAG_SCALE];
	scale[0] = jScale[0];
	scale[1] = jScale[1];
	reverseEffect = jComponent[JSON_TAG_REVERSE_EFFECT];
	reverseDistanceRM = (RandomMode)(int) jComponent[JSON_TAG_REVERSE_DISTANCE_RM];
	JsonValue jReverseDistance = jComponent[JSON_TAG_REVERSE_DISTANCE];
	reverseDistance[0] = jReverseDistance[0];
	reverseDistance[1] = jReverseDistance[1];
	maxParticles = jComponent[JSON_TAG_MAX_PARTICLE];
	playOnAwake = jComponent[JSON_TAG_PLAY_ON_AWAKE];

	// Emision
	attachEmitter = jComponent[JSON_TAG_ATTACH_EMITTER];
	particlesPerSecondRM = (RandomMode)(int) jComponent[JSON_TAG_PARTICLES_SECOND_RM];
	JsonValue jParticlesPerSecond = jComponent[JSON_TAG_PARTICLES_SECOND];
	particlesPerSecond[0] = jParticlesPerSecond[0];
	particlesPerSecond[1] = jParticlesPerSecond[1];

	// Gravity
	gravityEffect = jComponent[JSON_TAG_GRAVITY_EFFECT];
	gravityFactorRM = (RandomMode)(int) jComponent[JSON_TAG_GRAVITY_FACTOR_RM];
	JsonValue jGravityFactor = jComponent[JSON_TAG_GRAVITY_FACTOR];
	gravityFactor[0] = jGravityFactor[0];
	gravityFactor[1] = jGravityFactor[1];

	// Shape
	emitterType = (ParticleEmitterType)(int) jComponent[JSON_TAG_EMITTER_TYPE];
	coneRadiusUp = jComponent[JSON_TAG_CONE_RADIUS_UP];
	coneRadiusDown = jComponent[JSON_TAG_CONE_RADIUS_DOWN];
	randomConeRadiusUp = jComponent[JSON_TAG_RANDOM_CONE_RADIUS_UP];
	randomConeRadiusDown = jComponent[JSON_TAG_RANDOM_CONE_RADIUS_DOWN];

	// Rotation over Lifetime
	rotationOverLifetime = jComponent[JSON_TAG_ROTATION_OVER_LIFETIME];
	rotationFactorRM = (RandomMode)(int) jComponent[JSON_TAG_ROTATION_FACTOR_RM];
	JsonValue jRotationFactor = jComponent[JSON_TAG_ROTATION_FACTOR];
	rotationFactor[0] = jRotationFactor[0];
	rotationFactor[1] = jRotationFactor[1];

	// Size over Lifetime
	sizeOverLifetime = jComponent[JSON_TAG_SIZE_OVER_LIFETIME];
	scaleFactorRM = (RandomMode)(int) jComponent[JSON_TAG_SCALE_FACTOR_RM];
	JsonValue jScaleFactor = jComponent[JSON_TAG_SCALE_FACTOR];
	scaleFactor[0] = jScaleFactor[0];
	scaleFactor[1] = jScaleFactor[1];

	// Color over Lifetime
	colorOverLifetime = jComponent[JSON_TAG_COLOR_OVER_LIFETIME];
	int numberColors = jComponent[JSON_TAG_NUMBER_COLORS];
	if (!gradient) gradient = new ImGradient();
	gradient->clearList();
	JsonValue jColor = jComponent[JSON_TAG_GRADIENT_COLORS];
	for (int i = 0; i < numberColors; ++i) {
		JsonValue jMark = jColor[i];
		gradient->addMark(jMark[4], ImColor((float) jMark[0], (float) jMark[1], (float) jMark[2], (float) jMark[3]));
	}

	// Texture Sheet Animation
	Ytiles = jComponent[JSON_TAG_YTILES];
	Xtiles = jComponent[JSON_TAG_XTILES];
	animationSpeed = jComponent[JSON_TAG_ANIMATION_SPEED];
	isRandomFrame = jComponent[JSON_TAG_IS_RANDOM_FRAME];
	loopAnimation = jComponent[JSON_TAG_lOOP_ANIMATION];
	nCycles = jComponent[JSON_TAG_N_CYCLES];

	// Render
	textureID = jComponent[JSON_TAG_TEXTURE_TEXTURE_ID];
	if (textureID != 0) {
		App->resources->IncreaseReferenceCount(textureID);
	}
	billboardType = (BillboardType)(int) jComponent[JSON_TAG_BILLBOARD_TYPE];
	renderMode = (ParticleRenderMode)(int) jComponent[JSON_TAG_PARTICLE_RENDER_MODE];
	renderAlignment = (ParticleRenderAlignment)(int) jComponent[JSON_TAG_PARTICLE_RENDER_ALIGNMENT];
	JsonValue jFlip = jComponent[JSON_TAG_FLIP_TEXTURE];
	flipTexture[0] = jFlip[0];
	flipTexture[1] = jFlip[1];
	isSoft = jComponent[JSON_TAG_IS_SOFT];
	softRange = jComponent[JSON_TAG_SOFT_RANGE];

	// Collision
	collision = jComponent[JSON_TAG_HAS_COLLISION];
	radius = jComponent[JSON_TAG_COLLISION_RADIUS];
	layerIndex = jComponent[JSON_TAG_LAYER_INDEX];
	layer = WorldLayers(1 << layerIndex);

	//Trail
	hasTrail = jComponent[JSON_TAG_HASTRAIL];

	trailRatio = jComponent[JSON_TAG_TRAIL_RATIO];
	widthRM = (RandomMode)(int) jComponent[JSON_TAG_TRAIL_WIDTH_RM];
	JsonValue jWidth = jComponent[JSON_TAG_TRAIL_WIDTH];
	width[0] = jWidth[0];
	width[1] = jWidth[1];
	trailQuadsRM = (RandomMode)(int) jComponent[JSON_TAG_TRAIL_QUADS_RM];
	JsonValue jtrailQuads = jComponent[JSON_TAG_TRAIL_QUADS];
	trailQuads[0] = jtrailQuads[0];
	trailQuads[1] = jtrailQuads[1];
	quadLifeRM = (RandomMode)(int) jComponent[JSON_TAG_TRAIL_QUAD_LIFE_RM];
	JsonValue jQuadLife = jComponent[JSON_TAG_TRAIL_QUAD_LIFE];
	quadLife[0] = jQuadLife[0];
	quadLife[1] = jQuadLife[1];

	textureTrailID = jComponent[JSON_TAG_TRAIL_TEXTURE_TEXTUREID];
	if (textureTrailID != 0) {
		App->resources->IncreaseReferenceCount(textureTrailID);
	}
	JsonValue jTrailFlip = jComponent[JSON_TAG_TRAIL_FLIP_TEXTURE];
	flipTrailTexture[0] = jTrailFlip[0];
	flipTrailTexture[1] = jTrailFlip[1];
	nTextures = jComponent[JSON_TAG_TRAIL_TEXTURE_REPEATS];

	colorOverTrail = jComponent[JSON_TAG_HAS_COLOR_OVER_TRAIL];
	int trailNumberColors = jComponent[JSON_TAG_NUMBER_COLORS_TRAIL];
	if (!gradientTrail) gradientTrail = new ImGradient();
	gradientTrail->clearList();
	JsonValue jTrailColor = jComponent[JSON_TAG_GRADIENT_COLORS_TRAIL];
	for (int i = 0; i < trailNumberColors; ++i) {
		JsonValue jMark = jTrailColor[i];
		gradientTrail->addMark(jMark[4], ImColor((float) jMark[0], (float) jMark[1], (float) jMark[2], (float) jMark[3]));
	}

	// Sub Emmiters
	int numSubEmitters = jComponent[JSON_TAG_SUB_EMITTERS_NUMBER];
	subEmitters.clear();
	JsonValue jSubEmitters = jComponent[JSON_TAG_SUB_EMITTERS];
	for (int i = 0; i < numSubEmitters; ++i) {
		JsonValue jSubEmitter = jSubEmitters[i];
		SubEmitter* subEmitter = new SubEmitter();
		subEmitter->gameObjectUID = jSubEmitter[JSON_TAG_SUB_EMITTERS_GAMEOBJECT];
		subEmitter->subEmitterType = (SubEmitterType)(int) jSubEmitter[JSON_TAG_SUB_EMMITERS_EMITTER_TYPE];
		subEmitter->emitProbability = jSubEmitter[JSON_TAG_SUB_EMITTERS_EMIT_PROB];
		subEmitters.push_back(subEmitter);
	}

	// Lights
	hasLights = jComponent[JSON_TAG_HAS_LIGHTS];
	lightGameObjectUID = jComponent[JSON_TAG_LIGHT_GAMEOBJECT];

	lightsRatio = jComponent[JSON_TAG_LIGHTS_RATIO];
	JsonValue jLightOffset = jComponent[JSON_TAG_LIGHTS_OFFSET];
	lightOffset.Set(jLightOffset[0], jLightOffset[1], jLightOffset[2]);
	intensityMultiplierRM = (RandomMode)(int) jComponent[JSON_TAG_LIGHTS_INTENSITY_MULTIPLIER_RM];
	JsonValue jIntensityMultiplier = jComponent[JSON_TAG_LIGHTS_INTENSITY_MULTIPLIER];
	intensityMultiplier.Set(jIntensityMultiplier[0], jIntensityMultiplier[1]);
	rangeMultiplierRM = (RandomMode)(int) jComponent[JSON_TAG_LIGHTS_RANGE_MULTIPLIER_RM];
	JsonValue jRangeMultiplier = jComponent[JSON_TAG_LIGHTS_RANGE_MULTIPLIER];
	rangeMultiplier.Set(jRangeMultiplier[0], jRangeMultiplier[1]);

	useParticleColor = jComponent[JSON_TAG_LIGHTS_USE_PARTICLE_COLOR];
	useCustomColor = jComponent[JSON_TAG_LIGHTS_USE_CUSTOM_COLOR];
	int lightsNumberColors = jComponent[JSON_TAG_LIGHTS_NUMBER_COLORS];
	if (!gradientLight) gradientLight = new ImGradient();
	gradientLight->clearList();
	JsonValue jLightColor = jComponent[JSON_TAG_LIGHTS_GRADIENT_COLORS];
	for (int i = 0; i < lightsNumberColors; ++i) {
		JsonValue jMark = jLightColor[i];
		gradientLight->addMark(jMark[4], ImColor((float) jMark[0], (float) jMark[1], (float) jMark[2], (float) jMark[3]));
	}

	maxLights = jComponent[JSON_TAG_LIGHTS_MAX_LIGHTS];

	AllocateParticlesMemory();
}

void ComponentParticleSystem::Save(JsonValue jComponent) const {
	// Gizmo
	jComponent[JSON_TAG_DRAW_GIZMO] = drawGizmo;

	// Particle System
	jComponent[JSON_TAG_DURATION] = duration;
	jComponent[JSON_TAG_LOOPING] = looping;
	jComponent[JSON_TAG_START_DELAY] = (int) startDelayRM;
	JsonValue jStartDelay = jComponent[JSON_TAG_START_DELAY];
	jStartDelay[0] = startDelay[0];
	jStartDelay[1] = startDelay[1];
	jComponent[JSON_TAG_LIFE_RM] = (int) lifeRM;
	JsonValue jLife = jComponent[JSON_TAG_LIFE];
	jLife[0] = life[0];
	jLife[1] = life[1];
	jComponent[JSON_TAG_SPEED_RM] = (int) speedRM;
	JsonValue jSpeed = jComponent[JSON_TAG_SPEED];
	jSpeed[0] = speed[0];
	jSpeed[1] = speed[1];
	jComponent[JSON_TAG_ROTATION_RM] = (int) rotationRM;
	JsonValue jRotation = jComponent[JSON_TAG_ROTATION];
	jRotation[0] = rotation[0];
	jRotation[1] = rotation[1];
	jComponent[JSON_TAG_SCALE_RM] = (int) scaleRM;
	JsonValue jScale = jComponent[JSON_TAG_SCALE];
	jScale[0] = scale[0];
	jScale[1] = scale[1];
	jComponent[JSON_TAG_REVERSE_EFFECT] = reverseEffect;
	jComponent[JSON_TAG_REVERSE_DISTANCE_RM] = (int) reverseDistanceRM;
	JsonValue jReverseDistance = jComponent[JSON_TAG_REVERSE_DISTANCE];
	jReverseDistance[0] = reverseDistance[0];
	jReverseDistance[1] = reverseDistance[1];
	jComponent[JSON_TAG_MAX_PARTICLE] = maxParticles;
	jComponent[JSON_TAG_PLAY_ON_AWAKE] = playOnAwake;

	// Emision
	jComponent[JSON_TAG_ATTACH_EMITTER] = attachEmitter;
	jComponent[JSON_TAG_PARTICLES_SECOND_RM] = (int) particlesPerSecondRM;
	JsonValue jParticlesPerSecond = jComponent[JSON_TAG_PARTICLES_SECOND];
	jParticlesPerSecond[0] = particlesPerSecond[0];
	jParticlesPerSecond[1] = particlesPerSecond[1];

	// Gravity
	jComponent[JSON_TAG_GRAVITY_EFFECT] = gravityEffect;
	jComponent[JSON_TAG_GRAVITY_FACTOR_RM] = (int) gravityFactorRM;
	JsonValue jGravityFactor = jComponent[JSON_TAG_GRAVITY_FACTOR];
	jGravityFactor[0] = gravityFactor[0];
	jGravityFactor[1] = gravityFactor[1];

	// Shape
	jComponent[JSON_TAG_EMITTER_TYPE] = (int) emitterType;
	jComponent[JSON_TAG_CONE_RADIUS_UP] = coneRadiusUp;
	jComponent[JSON_TAG_CONE_RADIUS_DOWN] = coneRadiusDown;
	jComponent[JSON_TAG_RANDOM_CONE_RADIUS_UP] = randomConeRadiusUp;
	jComponent[JSON_TAG_RANDOM_CONE_RADIUS_DOWN] = randomConeRadiusDown;

	// Rotation over Lifetime
	jComponent[JSON_TAG_ROTATION_OVER_LIFETIME] = rotationOverLifetime;
	jComponent[JSON_TAG_ROTATION_FACTOR_RM] = (int) rotationFactorRM;
	JsonValue jRotationFactor = jComponent[JSON_TAG_ROTATION_FACTOR];
	jRotationFactor[0] = rotationFactor[0];
	jRotationFactor[1] = rotationFactor[1];

	// Size over Lifetime
	jComponent[JSON_TAG_SIZE_OVER_LIFETIME] = sizeOverLifetime;
	jComponent[JSON_TAG_SCALE_FACTOR_RM] = (int) scaleFactorRM;
	JsonValue jScaleFactor = jComponent[JSON_TAG_SCALE_FACTOR];
	jScaleFactor[0] = scaleFactor[0];
	jScaleFactor[1] = scaleFactor[1];

	// Color over Lifetime
	jComponent[JSON_TAG_COLOR_OVER_LIFETIME] = colorOverLifetime;
	int color = 0;
	JsonValue jColor = jComponent[JSON_TAG_GRADIENT_COLORS];
	for (ImGradientMark* mark : gradient->getMarks()) {
		JsonValue jMask = jColor[color];
		jMask[0] = mark->color[0];
		jMask[1] = mark->color[1];
		jMask[2] = mark->color[2];
		jMask[3] = mark->color[3];
		jMask[4] = mark->position;

		color++;
	}
	jComponent[JSON_TAG_NUMBER_COLORS] = gradient->getMarks().size();

	// Texture Sheet Animation
	jComponent[JSON_TAG_YTILES] = Ytiles;
	jComponent[JSON_TAG_XTILES] = Xtiles;
	jComponent[JSON_TAG_ANIMATION_SPEED] = animationSpeed;
	jComponent[JSON_TAG_IS_RANDOM_FRAME] = isRandomFrame;
	jComponent[JSON_TAG_lOOP_ANIMATION] = loopAnimation;
	jComponent[JSON_TAG_N_CYCLES] = nCycles;

	// Render
	jComponent[JSON_TAG_TEXTURE_TEXTURE_ID] = textureID;
	jComponent[JSON_TAG_BILLBOARD_TYPE] = (int) billboardType;
	jComponent[JSON_TAG_PARTICLE_RENDER_MODE] = (int) renderMode;
	jComponent[JSON_TAG_PARTICLE_RENDER_ALIGNMENT] = (int) renderAlignment;
	JsonValue jFlip = jComponent[JSON_TAG_FLIP_TEXTURE];
	jFlip[0] = flipTexture[0];
	jFlip[1] = flipTexture[1];
	jComponent[JSON_TAG_IS_SOFT] = isSoft;
	jComponent[JSON_TAG_SOFT_RANGE] = softRange;

	// Collision
	jComponent[JSON_TAG_HAS_COLLISION] = collision;
	jComponent[JSON_TAG_LAYER_INDEX] = layerIndex;
	jComponent[JSON_TAG_COLLISION_RADIUS] = radius;

	// Trail
	jComponent[JSON_TAG_HASTRAIL] = hasTrail;

	jComponent[JSON_TAG_TRAIL_RATIO] = trailRatio;
	jComponent[JSON_TAG_TRAIL_WIDTH_RM] = (int) widthRM;
	JsonValue jWidth = jComponent[JSON_TAG_TRAIL_WIDTH];
	jWidth[0] = width[0];
	jWidth[1] = width[1];
	jComponent[JSON_TAG_TRAIL_QUADS_RM] = (int) trailQuadsRM;
	JsonValue jTrailQuads = jComponent[JSON_TAG_TRAIL_QUADS];
	jTrailQuads[0] = trailQuads[0];
	jTrailQuads[1] = trailQuads[1];
	jComponent[JSON_TAG_TRAIL_QUAD_LIFE_RM] = (int) quadLifeRM;
	JsonValue jQuadLife = jComponent[JSON_TAG_TRAIL_QUAD_LIFE];
	jQuadLife[0] = quadLife[0];
	jQuadLife[1] = quadLife[1];

	jComponent[JSON_TAG_TRAIL_TEXTURE_TEXTUREID] = textureTrailID;
	JsonValue jTrailFlip = jComponent[JSON_TAG_TRAIL_FLIP_TEXTURE];
	jTrailFlip[0] = flipTrailTexture[0];
	jTrailFlip[1] = flipTrailTexture[1];
	jComponent[JSON_TAG_TRAIL_TEXTURE_REPEATS] = nTextures;

	jComponent[JSON_TAG_HAS_COLOR_OVER_TRAIL] = colorOverTrail;
	int trailColor = 0;
	JsonValue jTrailColor = jComponent[JSON_TAG_GRADIENT_COLORS_TRAIL];
	for (ImGradientMark* mark : gradientTrail->getMarks()) {
		JsonValue jMask = jTrailColor[trailColor];
		jMask[0] = mark->color[0];
		jMask[1] = mark->color[1];
		jMask[2] = mark->color[2];
		jMask[3] = mark->color[3];
		jMask[4] = mark->position;

		trailColor++;
	}
	jComponent[JSON_TAG_NUMBER_COLORS_TRAIL] = gradientTrail->getMarks().size();

	// Sub Emitters
	int num = 0;
	JsonValue jSubEmitters = jComponent[JSON_TAG_SUB_EMITTERS];
	for (SubEmitter* subEmitter : subEmitters) {
		if (subEmitter->gameObjectUID == 0) continue;
		JsonValue jSubEmitter = jSubEmitters[num];
		jSubEmitter[JSON_TAG_SUB_EMITTERS_GAMEOBJECT] = subEmitter->gameObjectUID;
		jSubEmitter[JSON_TAG_SUB_EMMITERS_EMITTER_TYPE] = (int) subEmitter->subEmitterType;
		jSubEmitter[JSON_TAG_SUB_EMITTERS_EMIT_PROB] = subEmitter->emitProbability;

		num += 1;
	}
	jComponent[JSON_TAG_SUB_EMITTERS_NUMBER] = num;

	// Lights
	jComponent[JSON_TAG_HAS_LIGHTS] = hasLights;
	jComponent[JSON_TAG_LIGHT_GAMEOBJECT] = lightGameObjectUID;

	jComponent[JSON_TAG_LIGHTS_RATIO] = lightsRatio;
	JsonValue jLightOffset = jComponent[JSON_TAG_LIGHTS_OFFSET];
	jLightOffset[0] = lightOffset[0];
	jLightOffset[1] = lightOffset[1];
	jLightOffset[2] = lightOffset[2];
	jComponent[JSON_TAG_LIGHTS_INTENSITY_MULTIPLIER_RM] = (int) intensityMultiplierRM;
	JsonValue jIntensityMultiplier = jComponent[JSON_TAG_LIGHTS_INTENSITY_MULTIPLIER];
	jIntensityMultiplier[0] = intensityMultiplier[0];
	jIntensityMultiplier[1] = intensityMultiplier[1];
	jComponent[JSON_TAG_LIGHTS_RANGE_MULTIPLIER_RM] = (int) rangeMultiplierRM;
	JsonValue jRangeMultiplier = jComponent[JSON_TAG_LIGHTS_RANGE_MULTIPLIER];
	jRangeMultiplier[0] = rangeMultiplier[0];
	jRangeMultiplier[1] = rangeMultiplier[1];

	jComponent[JSON_TAG_LIGHTS_USE_PARTICLE_COLOR] = useParticleColor;
	jComponent[JSON_TAG_LIGHTS_USE_CUSTOM_COLOR] = useCustomColor;
	int lightsColor = 0;
	JsonValue jLightColor = jComponent[JSON_TAG_LIGHTS_GRADIENT_COLORS];
	for (ImGradientMark* mark : gradientLight->getMarks()) {
		JsonValue jMask = jLightColor[lightsColor];
		jMask[0] = mark->color[0];
		jMask[1] = mark->color[1];
		jMask[2] = mark->color[2];
		jMask[3] = mark->color[3];
		jMask[4] = mark->position;

		lightsColor++;
	}
	jComponent[JSON_TAG_LIGHTS_NUMBER_COLORS] = gradientLight->getMarks().size();

	jComponent[JSON_TAG_LIGHTS_MAX_LIGHTS] = maxLights;
}

void ComponentParticleSystem::AllocateParticlesMemory() {
	if (isPlaying) {
		Restart();
	}
	particles.Allocate(maxParticles);
}

void ComponentParticleSystem::SpawnParticles() {
	if (!IsActive()) return;
	if (isPlaying && ((emitterTime < duration) || looping)) {
		if (restParticlesPerSecond <= 0) {
			for (int i = 0; i < particlesCurrentFrame; i++) {
				if (maxParticles > particles.Count()) SpawnParticleUnit();
			}
			InitStartRate();
		} else {
			restParticlesPerSecond -= App->time->GetDeltaTimeOrRealDeltaTime();
		}
	} else if (particles.Count() == 0) {
		isPlaying = false;
	}
}

void ComponentParticleSystem::SpawnParticleUnit() {
	Particle* currentParticle = particles.Obtain();

	if (currentParticle) {
		InitParticlePosAndDir(currentParticle);
		InitParticleRotation(currentParticle);
		InitParticleScale(currentParticle);
		InitParticleSpeed(currentParticle);
		InitParticleLife(currentParticle);
		InitParticleAnimationTexture(currentParticle);
		if (hasTrail && IsProbably(trailRatio)) {
			InitParticleTrail(currentParticle);
		}
		currentParticle->emitterPosition = GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();
		currentParticle->emitterRotation = GetOwner().GetComponent<ComponentTransform>()->GetGlobalRotation();

		if (App->time->HasGameStarted() && collision) {
			currentParticle->emitter = this;
			currentParticle->radius = radius;
			App->physics->CreateParticleRigidbody(currentParticle);
		}
		InitSubEmitter(currentParticle, SubEmitterType::BIRTH);

		if (hasLights && IsProbably(lightsRatio)) {
			if (lightsSpawned < maxLights) {
				InitLight(currentParticle);
			}
		}
	}
}

void ComponentParticleSystem::InitParticlePosAndDir(Particle* currentParticle) {
	float x0 = 0, y0 = 0, z0 = 0, x1 = 0, y1 = 0, z1 = 0;
	float3 localPos = float3::zero;
	float3 localDir = float3::zero;

	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();

	float reverseDist = ObtainRandomValueFloat(reverseDistance, reverseDistanceRM);

	if (emitterType == ParticleEmitterType::CONE) {
		float theta = 2 * pi * float(rand()) / float(RAND_MAX);
		if (randomConeRadiusDown) {
			x0 = float(rand()) / float(RAND_MAX) * cos(theta);
			z0 = float(rand()) / float(RAND_MAX) * sin(theta);
		} else {
			x0 = cos(theta);
			z0 = sin(theta);
		}

		if (randomConeRadiusUp) {
			float theta = 2 * pi * float(rand()) / float(RAND_MAX);
			x1 = float(rand()) / float(RAND_MAX) * cos(theta);
			z1 = float(rand()) / float(RAND_MAX) * sin(theta);
		} else {
			x1 = x0;
			z1 = z0;
		}

		float3 localPos0 = float3(x0, 0.0f, z0) * coneRadiusDown;
		float3 localPos1 = float3(x1, 0.0f, z1) * coneRadiusUp + float3::unitY;
		localDir = (localPos1 - localPos0).Normalized();

		if (reverseEffect) {
			localPos = localPos0 + localDir * reverseDist;
		} else {
			localPos = localPos0;
		}
	} else if (emitterType == ParticleEmitterType::SPHERE) {
		x1 = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
		y1 = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
		z1 = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;

		localDir = float3(x1, y1, z1);

		if (reverseEffect) {
			localPos = localDir * reverseDist;
		} else {
			localPos = float3::zero;
		}
	}

	currentParticle->initialPosition = transform->GetGlobalPosition() + transform->GetGlobalRotation() * localPos;
	currentParticle->position = currentParticle->initialPosition;
	currentParticle->direction = transform->GetGlobalRotation() * localDir;
}

void ComponentParticleSystem::InitParticleRotation(Particle* currentParticle) {
	float newRotation = ObtainRandomValueFloat(rotation, rotationRM);

	if (billboardType == BillboardType::STRETCH) {
		newRotation += pi / 2;
	}
	currentParticle->rotation = Quat::FromEulerXYZ(0.0f, 0.0f, newRotation);
}

void ComponentParticleSystem::InitParticleScale(Particle* currentParticle) {
	currentParticle->scale = float3(0.1f, 0.1f, 0.1f) * ObtainRandomValueFloat(scale, scaleRM);
}

void ComponentParticleSystem::InitParticleSpeed(Particle* currentParticle) {
	currentParticle->speed = ObtainRandomValueFloat(speed, speedRM);
}

void ComponentParticleSystem::InitParticleLife(Particle* currentParticle) {
	currentParticle->initialLife = ObtainRandomValueFloat(life, lifeRM);
	currentParticle->life = currentParticle->initialLife;
}

void ComponentParticleSystem::InitParticleAnimationTexture(Particle* currentParticle) {
	if (isRandomFrame) {
		currentParticle->currentFrame = static_cast<float>(rand() % ((Xtiles * Ytiles) + 1));
	} else {
		currentParticle->currentFrame = 0;
	}

	if (loopAnimation) {
		currentParticle->animationSpeed = animationSpeed;
	} else {
		float timePerCycle = currentParticle->initialLife / nCycles;
		float timePerFrame = (Ytiles * Xtiles) / timePerCycle;
		currentParticle->animationSpeed = timePerFrame;
	}
}

void ComponentParticleSystem::InitParticleTrail(Particle* currentParticle) {
	currentParticle->trail = new Trail();
	currentParticle->trail->Init();
	currentParticle->trail->width = ObtainRandomValueFloat(width, widthRM);
	currentParticle->trail->trailQuads = (int) ObtainRandomValueFloat(trailQuads, trailQuadsRM);
	currentParticle->trail->quadLife = ObtainRandomValueFloat(quadLife, quadLifeRM);
	currentParticle->trail->vertexDistance = distanceVertex;
	currentParticle->trail->textureID = textureTrailID;
	currentParticle->trail->flipTexture[0] = flipTrailTexture[0];
	currentParticle->trail->flipTexture[1] = flipTrailTexture[1];
	currentParticle->trail->nTextures = (nTextures > currentParticle->trail->trailQuads ? currentParticle->trail->trailQuads : nTextures);

	currentParticle->trail->colorOverTrail = colorOverTrail;
	currentParticle->trail->gradient = gradientTrail;
	currentParticle->trail->draggingGradient = draggingGradientTrail;
	currentParticle->trail->selectedGradient = selectedGradientTrail;
}

void ComponentParticleSystem::InitStartDelay() {
	restDelayTime = ObtainRandomValueFloat(startDelay, startDelayRM);
}

void ComponentParticleSystem::InitStartRate() {
	float newParticlesPerSecond = ObtainRandomValueFloat(particlesPerSecond, particlesPerSecondRM);
	if (newParticlesPerSecond == 0) {
		restParticlesPerSecond = inf;
	} else {
		restParticlesPerSecond = 1 / newParticlesPerSecond;
	}

	particlesCurrentFrame = (App->time->GetDeltaTimeOrRealDeltaTime() / restParticlesPerSecond);
}

void ComponentParticleSystem::InitSubEmitter(Particle* currentParticle, SubEmitterType subEmitterType) {
	for (SubEmitter* subEmitter : subEmitters) {
		if (subEmitter->subEmitterType != subEmitterType) return;
		if (!IsProbably(subEmitter->emitProbability)) return;
		if (subEmitter->particleSystem != nullptr) {
			GameObject& parent = GetOwner();
			Scene* scene = parent.scene;
			UID gameObjectId = GenerateUID();
			GameObject* newGameObject = scene->gameObjects.Obtain(gameObjectId);
			newGameObject->scene = scene;
			newGameObject->id = gameObjectId;
			newGameObject->name = "SubEmitter (Temp)";
			newGameObject->SetParent(&parent);
			ComponentTransform* transform = newGameObject->CreateComponent<ComponentTransform>();
			float3x3 rotationMatrix = float3x3::RotateFromTo(float3::unitY, currentParticle->direction);
			transform->SetGlobalPosition(currentParticle->position);
			transform->SetGlobalRotation(rotationMatrix.ToEulerXYZ());
			transform->SetGlobalScale(float3::one);
			newGameObject->Init();

			ComponentParticleSystem* newParticleSystem = newGameObject->CreateComponent<ComponentParticleSystem>();
			rapidjson::Document resourceMetaDocument;
			JsonValue jResourceMeta(resourceMetaDocument, resourceMetaDocument);
			subEmitter->particleSystem->Save(jResourceMeta);
			newParticleSystem->Load(jResourceMeta);
			newParticleSystem->Start();
			newParticleSystem->SetIsSubEmitter(true);
			newParticleSystem->Play();

			subEmittersGO.push_back(newGameObject);
		}
	}
}

void ComponentParticleSystem::InitLight(Particle* currentParticle) {
	if (lightComponent == nullptr) return;

	GameObject& parent = GetOwner();
	Scene* scene = parent.scene;
	UID gameObjectId = GenerateUID();
	GameObject* newGameObject = scene->gameObjects.Obtain(gameObjectId);
	newGameObject->scene = scene;
	newGameObject->id = gameObjectId;
	newGameObject->name = "Light (Temp)";
	newGameObject->SetParent(&parent);
	ComponentTransform* transform = newGameObject->CreateComponent<ComponentTransform>();
	ComponentTransform* transformPS = GetOwner().GetComponent<ComponentTransform>();
	float3 globalOffset = transformPS->GetGlobalMatrix().RotatePart() * lightOffset;
	transform->SetGlobalPosition(currentParticle->position + globalOffset);
	transform->SetGlobalRotation(float3::zero);
	transform->SetGlobalScale(float3::one);
	newGameObject->Init();

	ComponentLight* newLight = newGameObject->CreateComponent<ComponentLight>();
	rapidjson::Document resourceMetaDocument;
	JsonValue jResourceMeta(resourceMetaDocument, resourceMetaDocument);
	lightComponent->Save(jResourceMeta);
	newLight->Load(jResourceMeta);
	newLight->Enable();
	newLight->lightType = LightType::POINT;
	newLight->intensity *= ObtainRandomValueFloat(intensityMultiplier, intensityMultiplierRM);
	newLight->radius *= ObtainRandomValueFloat(rangeMultiplier, rangeMultiplierRM);

	if (useParticleColor && colorOverLifetime) {
		float4 color = float4::one;
		gradient->getColorAt(0.0f, color.ptr());
		newLight->color = float3(color.x, color.y, color.z);
	} else if (useCustomColor) {
		float4 color = float4::one;
		gradientLight->getColorAt(0.0f, color.ptr());
		newLight->color = float3(color.x, color.y, color.z);
	}
	currentParticle->lightGO = newGameObject;
	lightsSpawned++;
}

void ComponentParticleSystem::Update() {
	if (!isStarted && App->time->HasGameStarted() && playOnAwake) {
		Play();
		isStarted = true;
	}

	if (App->editor->selectedGameObject == &GetOwner()) {
		ImGuiParticlesEffect();
	}

	if (restDelayTime <= 0) {
		if (isPlaying) {
			emitterTime += App->time->GetDeltaTimeOrRealDeltaTime();

			for (Particle& currentParticle : particles) {
				UpdatePosition(&currentParticle);

				UpdateLife(&currentParticle);

				if (rotationOverLifetime) {
					UpdateRotation(&currentParticle);
				}

				if (sizeOverLifetime) {
					UpdateScale(&currentParticle);
				}

				if (!isRandomFrame) {
					currentParticle.currentFrame += currentParticle.animationSpeed * App->time->GetDeltaTimeOrRealDeltaTime();
				}
				if (currentParticle.trail != nullptr) {
					UpdateTrail(&currentParticle);
				}

				if (currentParticle.life < 0) {
					deadParticles.push_back(&currentParticle);
					InitSubEmitter(&currentParticle, SubEmitterType::DEATH);
				}

				if (currentParticle.hasCollided) {
					InitSubEmitter(&currentParticle, SubEmitterType::COLLISION);
				}

				if (currentParticle.lightGO != nullptr) {
					UpdateLight(&currentParticle);
				}
			}
		}

		UndertakerParticle();
		UpdateSubEmitters();
		SpawnParticles();
	} else {
		if (!isPlaying) return;
		restDelayTime -= App->time->GetDeltaTimeOrRealDeltaTime();
	}
}

void ComponentParticleSystem::UpdatePosition(Particle* currentParticle) {
	if (attachEmitter) {
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		float3 position = transform->GetGlobalPosition();
		Quat rotation = transform->GetGlobalRotation();

		if (!currentParticle->emitterRotation.Equals(rotation)) {
			// Update Particle direction
			float3 direction = (rotation * float3::unitY).Normalized();
			float3 oldDirection = (currentParticle->emitterRotation * float3::unitY).Normalized();
			float3 axisToRotate = Cross(oldDirection, direction).Normalized();
			float angleToRotate = acos(Dot(oldDirection, direction));
			float3x3 rotateMatrix = float3x3::RotateAxisAngle(axisToRotate, angleToRotate);
			currentParticle->direction = rotateMatrix * currentParticle->direction;

			// Update initialPoint using intersection between Plane that contains the point and axisToRotate
			float dist = 0;
			Plane planeInitPos = Plane(currentParticle->initialPosition, axisToRotate);
			Line line = Line(currentParticle->emitterPosition, axisToRotate);
			planeInitPos.Intersects(line, &dist);
			float3 intersectPoint = line.GetPoint(dist);
			float3 newInitialPosDirection = rotateMatrix * (currentParticle->initialPosition - intersectPoint).Normalized();
			currentParticle->initialPosition = newInitialPosDirection * Length(currentParticle->initialPosition - intersectPoint) + intersectPoint;

			currentParticle->position = currentParticle->direction * Length(currentParticle->position - currentParticle->initialPosition) + currentParticle->initialPosition;
			currentParticle->emitterRotation = rotation;
		}

		if (!currentParticle->emitterPosition.Equals(position)) {
			float3 directionLength = (position - currentParticle->emitterPosition).Normalized() * Length(position - currentParticle->emitterPosition);
			currentParticle->initialPosition = directionLength + currentParticle->initialPosition;
			currentParticle->position = directionLength + currentParticle->position;
			currentParticle->emitterPosition = position;
		}
	}

	if (reverseEffect) {
		currentParticle->position -= currentParticle->direction * currentParticle->speed * App->time->GetDeltaTimeOrRealDeltaTime();
	} else {
		if (gravityEffect) {
			UpdateGravityDirection(currentParticle);
		}
		currentParticle->position += currentParticle->direction * currentParticle->speed * App->time->GetDeltaTimeOrRealDeltaTime();
	}
}

void ComponentParticleSystem::UpdateRotation(Particle* currentParticle) {
	float newRotation = ObtainRandomValueFloat(rotationFactor, rotationFactorRM);
	float rotation = currentParticle->rotation.ToEulerXYZ().z;
	rotation += newRotation * App->time->GetDeltaTimeOrRealDeltaTime();
	currentParticle->rotation = Quat::FromEulerXYZ(0.0f, 0.0f, rotation);
}

void ComponentParticleSystem::UpdateScale(Particle* currentParticle) {
	float newScale = ObtainRandomValueFloat(scaleFactor, scaleFactorRM);

	currentParticle->radius *= 1 + newScale * App->time->GetDeltaTimeOrRealDeltaTime() / currentParticle->scale.x;
	if (collision) App->physics->UpdateParticleRigidbody(currentParticle);

	currentParticle->scale.x += newScale * App->time->GetDeltaTimeOrRealDeltaTime();
	currentParticle->scale.y += newScale * App->time->GetDeltaTimeOrRealDeltaTime();
	currentParticle->scale.z += newScale * App->time->GetDeltaTimeOrRealDeltaTime();

	if (currentParticle->scale.x < 0) {
		currentParticle->scale.x = 0;
	}
	if (currentParticle->scale.y < 0) {
		currentParticle->scale.y = 0;
	}
	if (currentParticle->scale.z < 0) {
		currentParticle->scale.z = 0;
	}
}

void ComponentParticleSystem::UpdateLife(Particle* currentParticle) {
	currentParticle->life -= App->time->GetDeltaTimeOrRealDeltaTime();
}

void ComponentParticleSystem::UpdateTrail(Particle* currentParticle) {
	currentParticle->trail->Update(currentParticle->position);
}

void ComponentParticleSystem::UpdateGravityDirection(Particle* currentParticle) {
	float newGravityFactor = ObtainRandomValueFloat(gravityFactor, gravityFactorRM);
	float x = currentParticle->direction.x;
	float y = -(1 / newGravityFactor) * Pow(currentParticle->gravityTime, 2) + currentParticle->gravityTime;
	float z = currentParticle->direction.z;
	currentParticle->gravityTime += 10 * App->time->GetDeltaTimeOrRealDeltaTime();
	currentParticle->direction = float3(x, y, z);
}

void ComponentParticleSystem::UpdateSubEmitters() {
	int pos = 0;
	for (GameObject* gameObject : subEmittersGO) {
		ComponentParticleSystem* particleSystem = gameObject->GetComponent<ComponentParticleSystem>();
		if (particleSystem) {
			if (particleSystem->subEmittersGO.size() == 0) {
				if (!particleSystem->isPlaying) {
					subEmittersGO.erase(subEmittersGO.begin() + pos);
					pos -= 1;
					if (App->editor->selectedGameObject == gameObject) App->editor->selectedGameObject = nullptr;
					App->scene->DestroyGameObjectDeferred(gameObject);
				}
			}
		}
		pos += 1;
	}
}

void ComponentParticleSystem::UpdateLight(Particle* currentParticle) {
	if (lightComponent && lightComponent->lightType != LightType::POINT) {
		lightGameObjectUID = 0;
		lightComponent = nullptr;
	}

	if (currentParticle->lightGO && (lightGameObjectUID == 0 || !hasLights)) {
		App->scene->DestroyGameObjectDeferred(currentParticle->lightGO);
		lightsSpawned--;
	}

	if (currentParticle->lightGO == nullptr) return;
	ComponentLight* light = currentParticle->lightGO->GetComponent<ComponentLight>();
	if (light == nullptr) return;

	float3 position = currentParticle->position;
	ComponentTransform* transform = currentParticle->lightGO->GetComponent<ComponentTransform>();
	ComponentTransform* transformPS = GetOwner().GetComponent<ComponentTransform>();
	float3 globalOffset = transformPS->GetGlobalMatrix().RotatePart() * lightOffset;
	transform->SetGlobalPosition(currentParticle->position + globalOffset);

	if (useParticleColor && colorOverLifetime) {
		float4 color = float4::one;
		float factor = 1 - currentParticle->life / currentParticle->initialLife;
		gradient->getColorAt(factor, color.ptr());
		light->color = float3(color.x, color.y, color.z);
	} else if (useCustomColor) {
		float4 color = float4::one;
		float factor = 1 - currentParticle->life / currentParticle->initialLife;
		gradientLight->getColorAt(factor, color.ptr());
		light->color = float3(color.x, color.y, color.z);
	}
}

void ComponentParticleSystem::KillParticle(Particle* currentParticle) {
	currentParticle->life = -1;
}

void ComponentParticleSystem::UndertakerParticle(bool force) {
	if (force) {
		for (Particle& currentParticle : particles) {
			deadParticles.push_back(&currentParticle);
		}
	}
	for (Particle* currentParticle : deadParticles) {
		if (App->time->IsGameRunning()) App->physics->RemoveParticleRigidbody(currentParticle);
		if (currentParticle->motionState) {
			RELEASE(currentParticle->motionState);
		}
		RELEASE(currentParticle->trail);

		if (currentParticle->lightGO != nullptr) {
			if (App->editor->selectedGameObject == currentParticle->lightGO) App->editor->selectedGameObject = nullptr;
			App->scene->DestroyGameObjectDeferred(currentParticle->lightGO);
			lightsSpawned--;
		}
		particles.Release(currentParticle);
	}
	deadParticles.clear();
}

void ComponentParticleSystem::DrawGizmos() {
	if (IsActive() && drawGizmo) {
		if (emitterType == ParticleEmitterType::CONE) {
			ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
			dd::cone(transform->GetGlobalPosition(), transform->GetGlobalRotation() * float3::unitY * 1, dd::colors::White, coneRadiusUp, coneRadiusDown);
		}
		if (emitterType == ParticleEmitterType::SPHERE) {
			ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
			dd::sphere(transform->GetGlobalPosition(), dd::colors::White, 1.0f);
		}
	}
}

void ComponentParticleSystem::Draw() {
	if (isPlaying) {
		for (Particle& currentParticle : particles) {
			ProgramBillboard* program = App->programs->billboard;
			glUseProgram(program->program);

			unsigned glTexture = 0;
			ResourceTexture* texture = App->resources->GetResource<ResourceTexture>(textureID);
			glTexture = texture ? texture->glTexture : 0;
			int hasDiffuseMap = texture ? 1 : 0;

			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			if (renderMode == ParticleRenderMode::ADDITIVE) {
				glBlendFunc(GL_ONE, GL_ONE);
			} else {
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}

			glBindBuffer(GL_ARRAY_BUFFER, App->userInterface->GetQuadVBO());
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * 6 * 3));

			float4x4 newModelMatrix;
			if (billboardType == BillboardType::NORMAL) {
				if (renderAlignment == ParticleRenderAlignment::VIEW) {
					Frustum* frustum = App->camera->GetActiveCamera()->GetFrustum();
					newModelMatrix = float4x4::LookAt(float3::unitZ, -frustum->Front(), float3::unitY, float3::unitY);
				} else if (renderAlignment == ParticleRenderAlignment::WORLD) {
					newModelMatrix = float3x3::identity;
					newModelMatrix[1][1] = -1; // Invert z axis
				} else if (renderAlignment == ParticleRenderAlignment::LOCAL) {
					ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
					newModelMatrix = float4x4::LookAt(float3::unitZ, -(transform->GetGlobalRotation() * float3::unitY), float3::unitY, float3::unitY);
				} else if (renderAlignment == ParticleRenderAlignment::FACING) {
					float3 cameraPos = App->camera->GetActiveCamera()->GetFrustum()->Pos();
					newModelMatrix = float4x4::LookAt(float3::unitZ, cameraPos - currentParticle.position, float3::unitY, float3::unitY);
				} else { // Velocity
					newModelMatrix = float4x4::LookAt(float3::unitZ, -currentParticle.direction, float3::unitY, float3::unitY);
				}
			} else if (billboardType == BillboardType::STRETCH) {
				float3 cameraPos = App->camera->GetActiveCamera()->GetFrustum()->Pos();
				float3 cameraDir = (cameraPos - currentParticle.position).Normalized();
				float3 upDir = Cross(currentParticle.direction, cameraDir);
				float3 newCameraDir = Cross(currentParticle.direction, upDir);

				float3x3 newRotation;
				newRotation.SetCol(0, upDir);
				newRotation.SetCol(1, currentParticle.direction);
				newRotation.SetCol(2, newCameraDir);

				newModelMatrix = float4x4::identity * newRotation;
			} else if (billboardType == BillboardType::HORIZONTAL) {
				newModelMatrix = float4x4::LookAt(float3::unitZ, float3::unitY, float3::unitY, float3::unitY);
			} else if (billboardType == BillboardType::VERTICAL) {
				float3 cameraPos = App->camera->GetActiveCamera()->GetFrustum()->Pos();
				float3 cameraDir = (float3(cameraPos.x, currentParticle.position.y, cameraPos.z) - currentParticle.position).Normalized();
				newModelMatrix = float4x4::LookAt(float3::unitZ, cameraDir, float3::unitY, float3::unitY);
			}

			float4x4 modelMatrix = float4x4::FromTRS(currentParticle.position, newModelMatrix.RotatePart() * float3x3::FromQuat(currentParticle.rotation), currentParticle.scale);
			float4x4* view = &App->camera->GetViewMatrix();
			float4x4* proj = &App->camera->GetProjectionMatrix();

			glUniformMatrix4fv(program->modelLocation, 1, GL_TRUE, modelMatrix.ptr());
			glUniformMatrix4fv(program->viewLocation, 1, GL_TRUE, view->ptr());
			glUniformMatrix4fv(program->projLocation, 1, GL_TRUE, proj->ptr());

			float4 color = float4::one;
			if (colorOverLifetime) {
				float factor = 1 - currentParticle.life / currentParticle.initialLife; // Life decreases from Life to 0
				gradient->getColorAt(factor, color.ptr());
			}

			glUniform1f(program->nearLocation, App->camera->GetNearPlane());
			glUniform1f(program->farLocation, App->camera->GetFarPlane());

			glUniform1i(program->transparentLocation, renderMode == ParticleRenderMode::TRANSPARENT ? 1 : 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, App->renderer->depthsTexture);
			glUniform1i(program->depthsLocation, 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, glTexture);
			glUniform1i(program->diffuseMapLocation, 1);
			glUniform1i(program->hasDiffuseLocation, hasDiffuseMap);
			glUniform4fv(program->inputColorLocation, 1, color.ptr());

			glUniform1f(program->currentFrameLocation, currentParticle.currentFrame);

			glUniform1i(program->xTilesLocation, Xtiles);
			glUniform1i(program->yTilesLocation, Ytiles);

			glUniform1i(program->xFlipLocation, flipTexture[0] ? 1 : 0);
			glUniform1i(program->yFlipLocation, flipTexture[1] ? 1 : 0);

			glUniform1i(program->isSoftLocation, isSoft ? 1 : 0);
			glUniform1f(program->softRangeLocation, softRange);

			//TODO: implement drawarrays
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);

			if (currentParticle.trail != nullptr) {
				currentParticle.trail->Draw();
			}
			if (App->renderer->drawColliders) {
				dd::sphere(currentParticle.position, dd::colors::LawnGreen, currentParticle.radius);
			}
		}
	}
}

void ComponentParticleSystem::ImGuiParticlesEffect() {
	float2 pos = App->editor->panelScene.GetWindowsPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 50));
	ImGui::Begin("Particle Effect##particle_effect", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("GameObject: ");
	ImGui::SameLine();
	ImGui::TextColored(App->editor->textColor, GetOwner().name.c_str());

	float particlesData = ChildParticlesInfo();

	char particlesSpawned[10];
	sprintf_s(particlesSpawned, 10, "%.1f", particlesData);
	ImGui::Text("Particles: ");
	ImGui::SameLine();
	ImGui::TextColored(App->editor->textColor, particlesSpawned);

	ImGui::Separator();

	if (ImGui::Button("Play")) {
		PlayChildParticles();
	}
	ImGui::SameLine();
	if (ImGui::Button("Restart")) {
		RestartChildParticles();
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop")) {
		StopChildParticles();
	}
	ImGui::End();
}

void ComponentParticleSystem::Play() {
	if (!isPlaying) {
		isPlaying = true;
		InitStartDelay();
		emitterTime = 0.0f;
	}
}

void ComponentParticleSystem::Restart() {
	Stop();
	Play();
}

void ComponentParticleSystem::Stop() {
	UndertakerParticle(true);
	isPlaying = false;
	lightsSpawned = 0;
}

void ComponentParticleSystem::PlayChildParticles() {
	Play();
	for (GameObject* currentChild : GetOwner().GetChildren()) {
		ComponentParticleSystem* particleSystem = currentChild->GetComponent<ComponentParticleSystem>();
		if (particleSystem && !particleSystem->GetIsSubEmitter()) {
			particleSystem->PlayChildParticles();
		}
	}
}

void ComponentParticleSystem::RestartChildParticles() {
	Restart();
	for (GameObject* currentChild : GetOwner().GetChildren()) {
		if (currentChild->GetComponent<ComponentParticleSystem>()) {
			currentChild->GetComponent<ComponentParticleSystem>()->RestartChildParticles();
		}
	}
}

void ComponentParticleSystem::StopChildParticles() {
	Stop();
	for (GameObject* currentChild : GetOwner().GetChildren()) {
		if (currentChild->GetComponent<ComponentParticleSystem>()) {
			currentChild->GetComponent<ComponentParticleSystem>()->StopChildParticles();
		}
	}
}

float ComponentParticleSystem::ChildParticlesInfo() {
	float particlesInfo = (float) particles.Count();
	for (GameObject* currentChild : GetOwner().GetChildren()) {
		if (currentChild->GetComponent<ComponentParticleSystem>()) {
			particlesInfo += currentChild->GetComponent<ComponentParticleSystem>()->ChildParticlesInfo();
		}
	}
	return particlesInfo;
}

// ----- GETTERS -----

// Particle System
float ComponentParticleSystem::GetDuration() const {
	return duration;
}
bool ComponentParticleSystem::GetIsLooping() const {
	return looping;
}
float2 ComponentParticleSystem::GetLife() const {
	return life;
}
float2 ComponentParticleSystem::GetSpeed() const {
	return speed;
}
float2 ComponentParticleSystem::GetRotation() const {
	return rotation;
}
float2 ComponentParticleSystem::GetScale() const {
	return scale;
}
bool ComponentParticleSystem::GetIsReverseEffect() const {
	return reverseEffect;
}
float2 ComponentParticleSystem::GetReserseDistance() const {
	return reverseDistance;
}
unsigned ComponentParticleSystem::GetMaxParticles() const {
	return maxParticles;
}
bool ComponentParticleSystem::GetPlayOnAwake() const {
	return playOnAwake;
}

// Emision
bool ComponentParticleSystem::GetIsAttachEmitter() const {
	return attachEmitter;
}
float2 ComponentParticleSystem::GetParticlesPerSecond() const {
	return particlesPerSecond;
}

// Shape
ParticleEmitterType ComponentParticleSystem::GetEmitterType() const {
	return emitterType;
}

// -- Cone
float ComponentParticleSystem::GetConeRadiusUp() const {
	return coneRadiusUp;
}
float ComponentParticleSystem::GetConeRadiusDown() const {
	return coneRadiusDown;
}
bool ComponentParticleSystem::GetRandomConeRadiusDown() const {
	return randomConeRadiusDown;
}
bool ComponentParticleSystem::GetRandomConeRadiusUp() const {
	return randomConeRadiusUp;
}

// Rotation over Lifetime
bool ComponentParticleSystem::GetRotationOverLifetime() const {
	return rotationOverLifetime;
}
float2 ComponentParticleSystem::GetRotationFactor() const {
	return rotationFactor;
}

// Size over Lifetime
bool ComponentParticleSystem::GetSizeOverLifetime() const {
	return sizeOverLifetime;
}
float2 ComponentParticleSystem::GetScaleFactor() const {
	return scaleFactor;
}

// Color over Lifetime
bool ComponentParticleSystem::GetColorOverLifetime() const {
	return colorOverLifetime;
}

// Texture Sheet Animation
unsigned ComponentParticleSystem::GetXtiles() const {
	return Xtiles;
}
unsigned ComponentParticleSystem::GetYtiles() const {
	return Ytiles;
}
float ComponentParticleSystem::GetAnimationSpeed() const {
	return animationSpeed;
}
bool ComponentParticleSystem::GetIsRandomFrame() const {
	return isRandomFrame;
}
bool ComponentParticleSystem::GetIsLoopAnimation() const {
	return loopAnimation;
}
float ComponentParticleSystem::GetNCycles() const {
	return nCycles;
}

// Render
BillboardType ComponentParticleSystem::GetBillboardType() const {
	return billboardType;
}
ParticleRenderMode ComponentParticleSystem::GetRenderMode() const {
	return renderMode;
}
ParticleRenderAlignment ComponentParticleSystem::GetRenderAlignment() const {
	return renderAlignment;
}
bool ComponentParticleSystem::GetFlipXTexture() const {
	return flipTexture[0];
}
bool ComponentParticleSystem::GetFlipYTexture() const {
	return flipTexture[1];
}

// Collision
bool ComponentParticleSystem::GetCollision() const {
	return collision;
}

// Sub Emitter
bool ComponentParticleSystem::GetIsSubEmitter() const {
	return isSubEmitter;
}

// Lights
bool ComponentParticleSystem::GetHasLights() const {
	return hasLights;
}

// ----- SETTERS -----

// Particle System
void ComponentParticleSystem::SetDuration(float _duration) {
	duration = _duration;
}
void ComponentParticleSystem::SetIsLooping(bool _isLooping) {
	looping = _isLooping;
}
void ComponentParticleSystem::SetLife(float2 _life) {
	life = _life;
}
void ComponentParticleSystem::SetSpeed(float2 _speed) {
	speed = _speed;
}
void ComponentParticleSystem::SetRotation(float2 _rotation) {
	rotation = _rotation;
}
void ComponentParticleSystem::SetScale(float2 _scale) {
	scale = _scale;
}
void ComponentParticleSystem::SetIsReverseEffect(bool _isReverse) {
	reverseEffect = _isReverse;
}
void ComponentParticleSystem::SetReserseDistance(float2 _reverseDistance) {
	reverseDistance = _reverseDistance;
}
void ComponentParticleSystem::SetMaxParticles(unsigned _maxParticle) {
	maxParticles = _maxParticle;
	AllocateParticlesMemory();
}
void ComponentParticleSystem::SetPlayOnAwake(bool _playOnAwake) {
	playOnAwake = _playOnAwake;
}

// Emision
void ComponentParticleSystem::SetIsAttachEmmitter(bool _isAttachEmmiter) {
	attachEmitter = _isAttachEmmiter;
}
void ComponentParticleSystem::SetParticlesPerSecond(float2 _particlesPerSecond) {
	particlesPerSecond = _particlesPerSecond;
	InitStartRate();
}
// Shape
void ComponentParticleSystem::SetEmmitterType(ParticleEmitterType _emmitterType) {
	emitterType = _emmitterType;
}

// -- Cone
void ComponentParticleSystem::SetConeRadiusUp(float _coneRadiusUp) {
	coneRadiusUp = _coneRadiusUp;
}
void ComponentParticleSystem::SetConeRadiusDown(float _coneRadiusUp) {
	coneRadiusDown = coneRadiusDown;
}
void ComponentParticleSystem::SetRandomConeRadiusDown(bool _randomConeRadiusDown) {
	randomConeRadiusDown = _randomConeRadiusDown;
}
void ComponentParticleSystem::SetRandomConeRadiusUp(bool _randomConeRadiusUp) {
	randomConeRadiusUp = _randomConeRadiusUp;
}

// Rotation over Lifetime
void ComponentParticleSystem::SetRotationOverLifetime(bool _rotationOverLifeTime) {
	rotationOverLifetime = _rotationOverLifeTime;
}
void ComponentParticleSystem::SetRotationFactor(float2 _rotationFactor) {
	rotationFactor = _rotationFactor;
}

// Size over Lifetime
void ComponentParticleSystem::SetSizeOverLifetime(bool _sizeOverLifeTime) {
	sizeOverLifetime = _sizeOverLifeTime;
}
void ComponentParticleSystem::SetScaleFactor(float2 _scaleFactor) {
	scaleFactor = _scaleFactor;
}

// Color over Lifetime
void ComponentParticleSystem::SetColorOverLifetime(bool _colorOverLifeTime) {
	colorOverLifetime = _colorOverLifeTime;
}

// Texture Sheet Animation
void ComponentParticleSystem::SetXtiles(unsigned _Xtiles) {
	Xtiles = _Xtiles;
}
void ComponentParticleSystem::SetYtiles(unsigned _Ytiles) {
	Ytiles = _Ytiles;
}
void ComponentParticleSystem::SetAnimationSpeed(float _animationSpeed) {
	animationSpeed = _animationSpeed;
}
void ComponentParticleSystem::SetIsRandomFrame(bool _randomFrame) {
	isRandomFrame = _randomFrame;
}
void ComponentParticleSystem::SetIsLoopAnimation(bool _loopAnimation) {
	loopAnimation = _loopAnimation;
}
void ComponentParticleSystem::SetNCycles(float _nCycles) {
	nCycles = _nCycles;
}

// Render
void ComponentParticleSystem::SetBillboardType(BillboardType _bilboardType) {
	billboardType = _bilboardType;
}
void ComponentParticleSystem::SetRenderMode(ParticleRenderMode _renderMode) {
	renderMode = _renderMode;
}
void ComponentParticleSystem::SetRenderAlignment(ParticleRenderAlignment _renderAligment) {
	renderAlignment = _renderAligment;
}
void ComponentParticleSystem::SetFlipXTexture(bool _flipX) {
	flipTexture[0] = _flipX;
}
void ComponentParticleSystem::SetFlipYTexture(bool _flipY) {
	flipTexture[1] = _flipY;
}
void ComponentParticleSystem::SetIsSoft(bool _isSoft) {
	isSoft = _isSoft;
}

// Collision
void ComponentParticleSystem::SetCollision(bool _collision) {
	collision = _collision;
}

// Sub Emitter
void ComponentParticleSystem::SetIsSubEmitter(bool _isSubEmitter) {
	isSubEmitter = _isSubEmitter;
}

// Lights
void ComponentParticleSystem::SetHasLights(bool _hasLights) {
	hasLights = _hasLights;
}