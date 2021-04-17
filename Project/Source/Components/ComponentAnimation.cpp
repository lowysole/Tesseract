#include "ComponentAnimation.h"

#include "Application.h"
//#include "Resources/ResourceStateMachine.h"
#include "Resources/ResourceTransition.h"
#include "Resources/AnimationInterpolation.h"
//#include "Resources/ResourceStates.h"
#include "GameObject.h"
#include "AnimationController.h"
#include "Resources/ResourceAnimation.h"
#include "Components/ComponentType.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"

#include "Application.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleInput.h"

#include "Utils/Logging.h"
#include "Utils/Leaks.h"
#include "Utils/UID.h"

#define JSON_TAG_LOOP "Controller"
#define JSON_TAG_ANIMATION_ID "AnimationId"
#define JSON_TAG_STATE_MACHINE_ID "StateMachineId"

void ComponentAnimation::Update() {
	if (App->input->GetKey(SDL_SCANCODE_1)) {
		if (t != 1) {
			SendTrigger("s1Ts2");
			t = 1;
			LOG("Transition1");
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_2)) {
		if (t != 2) {
			SendTrigger("s2Ts3");
			t = 2;
			LOG("Transition2");
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_3)) {
		if (t != 3) {
			SendTrigger("s3Ts1");
			t = 3;
			LOG("Transition3");
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_4)) {
		if (t != 4) {
			//SendTrigger("s1Ts2");
			SendTrigger("s2Ts3");
			SendTrigger("s3Ts1");
			t = 4;
			LOG("Transition4");
		}
	}
	OnUpdate();
}

void ComponentAnimation::OnEditorUpdate() {
	ImGui::TextColored(App->editor->titleColor, "Animation");
}

void ComponentAnimation::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_STATE_MACHINE_ID] = stateMachineResourceUID;
	bool ok = true;
	//jComponent[JSON_TAG_LOOP] = animationController.loop;
}

void ComponentAnimation::Load(JsonValue jComponent) {
	stateMachineResourceUID = jComponent[JSON_TAG_STATE_MACHINE_ID];
	if (stateMachineResourceUID != 0) App->resources->IncreaseReferenceCount(stateMachineResourceUID);
	bool ok = true;

	//animationController.loop = jComponent[JSON_TAG_LOOP];
}

void ComponentAnimation::DuplicateComponent(GameObject& owner) {
	/*ComponentAnimation* component = owner.CreateComponent<ComponentAnimation>();
	component->animationController.animationID = animationController.animationID;
	component->animationController.loop = animationController.loop;
	component->animationController.currentTime = animationController.currentTime;*/
}

void ComponentAnimation::OnUpdate() {
	// Update gameobjects matrix
	GameObject* rootBone = GetOwner().GetRootBone();

	UpdateAnimations(rootBone);

	ResourceStateMachine* resourceStateMachine = (ResourceStateMachine*) App->resources->GetResource(stateMachineResourceUID);
	resourceStateMachine->GetCurrentState()->currentTime += App->time->GetDeltaTime();
}

void ComponentAnimation::SendTrigger(std::string trigger) {
	ResourceStateMachine* resourceStateMachine = (ResourceStateMachine*) App->resources->GetResource(stateMachineResourceUID);

	ResourceTransition* transition = resourceStateMachine->GetValidTransition(trigger);
	if (transition != nullptr) {
		if (animationInterpolations.size() == 0) {
			animationInterpolations.push_front(new AnimationInterpolation(transition->source, resourceStateMachine->GetCurrentState()->currentTime, 0, transition->interpolationDuration));
		}
		animationInterpolations.push_front(new AnimationInterpolation(transition->target, 0, 0, transition->interpolationDuration));
	}
}

struct CheckFinishInterpolation {
	bool operator()(AnimationInterpolation*& animationInterpolation) {
		if (animationInterpolation->fadeTime >= 0.9) {
			ResourceStateMachine* resourceStateMachine = (ResourceStateMachine*) App->resources->GetResource(stateMachineResourceUID);
			resourceStateMachine->SetCurrentState(animationInterpolation->state);
			resourceStateMachine->GetCurrentState()->currentTime = animationInterpolation->TransistionTime;
			LOG("CheckFinishInterpolation");
		}
		return animationInterpolation->fadeTime >= 0.9;
	}
	UID stateMachineResourceUID;
};

void ComponentAnimation::UpdateAnimations(GameObject* gameObject) {
	if (gameObject == nullptr) {
		return;
	}

	//find gameobject in hash
	float3 position, totalPostion = float3::zero;
	Quat rotation, totalRotation = Quat::identity;

	bool result = true;
	std::vector<ResourceTransition*> toDelete;
	ResourceStateMachine* resourceStateMachine = (ResourceStateMachine*) App->resources->GetResource(stateMachineResourceUID);

	if (animationInterpolations.size() > 1) {
		result = AnimationController::InterpolateTransitions(animationInterpolations.begin(), animationInterpolations, GetOwner().GetRootBone(), gameObject, position, rotation);
		CheckFinishInterpolation checkFinishInterpolation;
		checkFinishInterpolation.stateMachineResourceUID = stateMachineResourceUID;
		animationInterpolations.remove_if(checkFinishInterpolation);
		if (animationInterpolations.size() <= 1) {
			animationInterpolations.clear();
		}
	} else {
		result = AnimationController::GetTransform(resourceStateMachine->GetCurrentState()->clip, resourceStateMachine->GetCurrentState()->currentTime, gameObject->name.c_str(), position, rotation);
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
