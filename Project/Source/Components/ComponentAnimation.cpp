#include "ComponentAnimation.h"

#include "Application.h"
#include "GameObject.h"
#include "Animation/Transition.h"
#include "Animation/AnimationInterpolation.h"
#include "Animation/AnimationController.h"
#include "Resources/ResourceAnimation.h"
#include "Resources/ResourceClip.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleEvents.h"
#include "Utils/UID.h"
#include "Utils/ImGuiUtils.h"
#include "Animation/StateMachineEnum.h"
#include "Animation/StateMachineManager.h"

#include <algorithm> // std::find

#include "Utils/Leaks.h"

#define JSON_TAG_LOOP "Controller"
#define JSON_TAG_ANIMATION_ID "AnimationId"
#define JSON_TAG_STATE_MACHINE_PRINCIPAL_ID "StateMachinePrincipalId"
#define JSON_TAG_STATE_MACHINE_SECONDARY_ID "StateMachineSecondaryId"
#define JSON_TAG_CLIP "Clip"

ComponentAnimation::~ComponentAnimation() {
	App->resources->DecreaseReferenceCount(stateMachineResourceUIDPrincipal);
	App->resources->DecreaseReferenceCount(stateMachineResourceUIDSecondary);
}

void ComponentAnimation::Init() {
	App->resources->IncreaseReferenceCount(stateMachineResourceUIDPrincipal);
	App->resources->IncreaseReferenceCount(stateMachineResourceUIDSecondary);
}

void ComponentAnimation::Start() {
	LoadStateMachines();
}

void ComponentAnimation::Update() {
	if (!App->time->IsGameRunning() || !IsActive()) {
		return;
	}

	LoadStateMachines();

	// Update gameobjects matrix
	GameObject* rootBone = GetOwner().GetRootBone();

	UpdateAnimations(rootBone);

	if (loadedResourceStateMachine && animationInterpolationsPrincipal.empty()) {
		ResourceClip* currentClip = App->resources->GetResource<ResourceClip>(currentStatePrincipal.clipUid);
		if (!currentClip) {
			return;
		}
		currentTimeStatesPrincipal[currentStatePrincipal.id] += App->time->GetDeltaTime() * currentClip->speed;
	}
	if (loadedResourceStateMachineSecondary && currentStateSecondary.id != 0 && animationInterpolationsSecondary.empty()) {
		ResourceClip* currentClip = App->resources->GetResource<ResourceClip>(currentStateSecondary.clipUid);
		if (!currentClip) {
			return;
		}
		currentTimeStatesSecondary[currentStateSecondary.id] += (App->time->GetDeltaTime() * currentClip->speed);
	}
}

void ComponentAnimation::OnEditorUpdate() {
	LoadStateMachines();

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

	ImGui::TextColored(App->editor->titleColor, "Animation");

	// Principal
	UID oldStateMachinePrincipalUID = stateMachineResourceUIDPrincipal;
	ImGui::ResourceSlot<ResourceStateMachine>("State Machine Principal", &stateMachineResourceUIDPrincipal);
	if (oldStateMachinePrincipalUID != stateMachineResourceUIDPrincipal) {
		currentStatePrincipal = State();
		currentTimeStatesPrincipal.clear();
		loadedResourceStateMachine = false;
	}

	// Secondary
	UID oldStateMachineSecondaryUID = stateMachineResourceUIDSecondary;
	ImGui::ResourceSlot<ResourceStateMachine>("State Machine Secondary", &stateMachineResourceUIDSecondary);
	if (oldStateMachineSecondaryUID != stateMachineResourceUIDSecondary) {
		currentStateSecondary = State();
		currentTimeStatesSecondary.clear();
		loadedResourceStateMachineSecondary = false;
	}
}

void ComponentAnimation::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_STATE_MACHINE_PRINCIPAL_ID] = stateMachineResourceUIDPrincipal;
	jComponent[JSON_TAG_STATE_MACHINE_SECONDARY_ID] = stateMachineResourceUIDSecondary;
}

void ComponentAnimation::Load(JsonValue jComponent) {
	stateMachineResourceUIDPrincipal = jComponent[JSON_TAG_STATE_MACHINE_PRINCIPAL_ID];
	stateMachineResourceUIDSecondary = jComponent[JSON_TAG_STATE_MACHINE_SECONDARY_ID];
}

void ComponentAnimation::SendTrigger(const std::string& trigger) {
	StateMachineManager::SendTrigger(trigger, StateMachineEnum::PRINCIPAL, *this);
}

void ComponentAnimation::SendTriggerSecondary(const std::string& trigger) {
	if (loadedResourceStateMachine && currentStateSecondary.id == 0) {
		//For doing the interpolation between states correctly it is necessary to set the current state of the principal state machine to be the same as the second state machine current state
		currentStateSecondary = currentStatePrincipal;
		//Updating current time to the currentTimeStatesSecondary to sync the times between the current state machine of the first one and the second one
		currentTimeStatesSecondary[currentStateSecondary.id] = currentTimeStatesPrincipal[currentStatePrincipal.id];
	}

	StateMachineManager::SendTrigger(trigger, StateMachineEnum::SECONDARY, *this);
}

void ComponentAnimation::UpdateAnimations(GameObject* gameObject) {
	if (gameObject == nullptr) {
		return;
	}

	//find gameobject in hash
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	bool resetSecondaryStatemachine = false;

	bool result = StateMachineManager::UpdateAnimations(
		gameObject,
		GetOwner(),
		*this,
		position,
		rotation,
		resetSecondaryStatemachine);

	//If finishedTransition and currentStateSecondary equals to currentStatePrincipal we must reset the currentStateSecondary to "empty" state
	//See line 84 of StateMachineManager.cpp
	if (resetSecondaryStatemachine) {
		ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUIDSecondary);
		if (!resourceStateMachine) {
			return;
		}
		std::unordered_map<UID, State>::iterator it = resourceStateMachine->states.find(0); // Get "empty" state
		if (it != resourceStateMachine->states.end()) {
			currentStateSecondary = (*it).second;
		}
	}

	ComponentTransform* componentTransform = gameObject->GetComponent<ComponentTransform>();

	if (componentTransform && result) {
		componentTransform->SetPosition(position);
		componentTransform->SetRotation(rotation);
	}

	for (GameObject* child : gameObject->GetChildren()) {
		UpdateAnimations(child);
	}
}

void ComponentAnimation::LoadStateMachines() {
	if (!loadedResourceStateMachine) { //Checking if there is no state machine
		ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUIDPrincipal);
		if (resourceStateMachine) {
			currentStatePrincipal = State(resourceStateMachine->initialState);
			for (const auto& element : resourceStateMachine->states) {
				currentTimeStatesPrincipal.insert({element.first, 0.0f});
			}
			loadedResourceStateMachine = true;
		}
	}
	if (!loadedResourceStateMachineSecondary) { //Checking if there is no state machine
		ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUIDSecondary);
		if (resourceStateMachine) {
			currentStateSecondary = State(resourceStateMachine->initialState);
			for (const auto& element : resourceStateMachine->states) {
				currentTimeStatesSecondary.insert({element.first, 0.0f});

				//Filling the key events given that the secondary state machine has all of the states
				ResourceClip* clip = App->resources->GetResource<ResourceClip>(element.second.clipUid);
				if (clip) {
					listClipsKeyEvents.insert({element.second.clipUid, clip->keyEventClips});
					listClipsCurrentEventKeyFrames.insert({element.second.clipUid, 0});
				}
			}
			loadedResourceStateMachineSecondary = true;
		}
	}
}