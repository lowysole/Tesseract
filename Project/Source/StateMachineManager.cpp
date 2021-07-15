#include "StateMachineManager.h"
#include <Application.h>
#include <Modules/ModuleResources.h>
#include "Modules/ModuleTime.h"
#include <Resources/ResourceStateMachine.h>
#include "Transition.h"
#include <Utils/UID.h>
#include <Utils/Logging.h>
#include "GameObject.h"
#include "Components/ComponentAnimation.h"

bool StateMachineManager::Contains(std::list<AnimationInterpolation>& animationInterpolations, const UID& id) {
	for (const auto element : animationInterpolations) {
		if (element.state->id == id) return true;
	}
	return false;
}

void StateMachineManager::SendTrigger(const std::string& trigger, std::unordered_map<UID, float>& currentTimeStates, std::list<AnimationInterpolation>& animationInterpolations, const UID& stateMachineResourceUID, State& currentState, State& currentStateSecondary, std::unordered_map<UID, float>& currentTimeStatesPrincipal, StateMachineEnum stateMachineEnum, std::list<AnimationInterpolation>& animationInterpolationsSecondary) {
	ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUID);
	if (!resourceStateMachine) {
		return;
	}
	Transition* transition = resourceStateMachine->FindTransitionGivenName(trigger);
	if (transition != nullptr) {
		if (transition->source.id == currentState.id) {
			currentTimeStates[transition->target.id] = 0;

			float fade = 0;
			//If animation interpolation has repeated states, then calculate fade and deleteing repeated state. For evade current time isue.
			if (animationInterpolations.size() >0 && Contains(animationInterpolations, transition->target.id)) {
				fade = transition->interpolationDuration - animationInterpolations.front().fadeTime;
				animationInterpolations.pop_back();
			}

			if (animationInterpolations.size() == 0) {
				animationInterpolations.push_front(AnimationInterpolation(&transition->source, currentTimeStates[currentState.id], 0, transition->interpolationDuration));
			}
			//Set the currentTime of AnimationInterpolation equals to currentTimeStatesPrincipal (instead of 0) to avoid the gap between the times between the interpolation of state secondary to state principal.
			// given to this : we are not allowed to have the states in the principal same as the secondary state machine
			animationInterpolations.push_front(AnimationInterpolation(&transition->target, currentTimeStatesPrincipal[transition->target.id], fade, transition->interpolationDuration));
			

			//Check for fixing gap if the principal state machine changes and the secondary is doing the interpolation to another state 
			// different than the previous one
			if (stateMachineEnum == StateMachineEnum::PRINCIPAL && currentStateSecondary.id == currentState.id && animationInterpolationsSecondary.size() > 0) {
				// change the animation interpolation in order to go to the new transition
				currentStateSecondary = transition->target;
				animationInterpolationsSecondary.pop_front();
				animationInterpolationsSecondary.push_front(AnimationInterpolation(&transition->target, currentTimeStatesPrincipal[transition->target.id], 0, transition->interpolationDuration));
			}
			
			currentState = transition->target;
		} else {
			std::string name = StateMachineEnum::PRINCIPAL ? "principal" : "secondary";
			LOG("Warning:%s transition target from %s to %s, and current state is %s ", name.c_str(), transition->source.name.c_str(), transition->target.name.c_str(), currentState.name.c_str());
		}
	}
}

bool StateMachineManager::UpdateAnimations(GameObject* gameObject, const GameObject& owner, std::unordered_map<UID, float>& currentTimeStatesPrincipal, std::list<AnimationInterpolation>& animationInterpolationsPrincipal, const UID& stateMachineResourceUIDPrincipal, State* currentStatePrincipal, std::unordered_map<UID, float>& currentTimeStatesSecondary, std::list<AnimationInterpolation>& animationInterpolationsSecondary, const UID& stateMachineResourceUIDSecondary, State* currentStateSecondary, float3& position, Quat& rotation, bool& resetSecondaryStatemachine, std::unordered_map<UID, std::unordered_map<unsigned int, EventClip>> listClipsKeyEvents, ComponentAnimation &componentAnimation) {
	bool result = true;
	StateMachineEnum stateMachineSelected = StateMachineEnum::PRINCIPAL;
	//The currentStateSecondary could be an empty State object with the id of zero in case for the second state machine
	if (currentStateSecondary && currentStateSecondary->id != 0) {
		ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUIDSecondary);
		//Looking for the bone if is it in the state machine in order to be applied
		if (resourceStateMachine && !resourceStateMachine->bones.empty()) {
			auto nameBone = resourceStateMachine->bones.find(gameObject->name);
			if (nameBone != resourceStateMachine->bones.end()) {
				stateMachineSelected = StateMachineEnum::SECONDARY;
			}
		}
	}

	//Selecting which state machine is going to be used
	switch (stateMachineSelected) {
	case StateMachineEnum::PRINCIPAL:
		result = StateMachineManager::CalculateAnimation(gameObject, owner, currentTimeStatesPrincipal, animationInterpolationsPrincipal, stateMachineResourceUIDPrincipal, currentStatePrincipal, position, rotation, resetSecondaryStatemachine, StateMachineEnum::PRINCIPAL,listClipsKeyEvents,componentAnimation);
		break;
	case StateMachineEnum::SECONDARY:
		ResourceStateMachine* resourceStateMachinePrincipal = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUIDPrincipal);
		bool secondaryToAnyPrincipal = false; 
		if (resourceStateMachinePrincipal){
			secondaryToAnyPrincipal = StateMachineManager::SecondaryEqualsToAnyPrincipal(*currentStateSecondary,resourceStateMachinePrincipal->states);
		}
		result = StateMachineManager::CalculateAnimation(gameObject, owner, currentTimeStatesSecondary, animationInterpolationsSecondary, stateMachineResourceUIDSecondary, currentStateSecondary, position, rotation, resetSecondaryStatemachine, StateMachineEnum::SECONDARY, listClipsKeyEvents, componentAnimation, secondaryToAnyPrincipal );
		break;
	}

	return result;
}

bool StateMachineManager::SecondaryEqualsToAnyPrincipal(const State& currentStateSecondary, const std::unordered_map<UID, State> &states) {
	for (const auto& element : states) {
		if (currentStateSecondary.id == element.second.id) return true;
	}
	return false;
}

bool StateMachineManager::CalculateAnimation(GameObject* gameObject, const GameObject& owner, std::unordered_map<UID, float>& currentTimeStates, std::list<AnimationInterpolation>& animationInterpolations, const UID& stateMachineResourceUID, State* currentState, float3& position, Quat& rotation, bool& resetSecondaryStatemachine,const StateMachineEnum stateMachineEnum, std::unordered_map<UID, std::unordered_map<unsigned int, EventClip>> listClipsKeyEvents, ComponentAnimation &componentAnimation, bool principalEqualSecondary) {
	bool result = false;

	ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUID);
	if (!resourceStateMachine) {
		return result;
	}

	ResourceClip* clip = App->resources->GetResource<ResourceClip>(currentState->clipUid);
	if (!clip) {
		return result;
	}
	int currentSample = AnimationController::GetCurrentSample(*clip, currentTimeStates[currentState->id]);

	//Checking for transition between states
	if (animationInterpolations.size() > 1) {
		result = AnimationController::InterpolateTransitions(animationInterpolations.begin(), animationInterpolations, *owner.GetRootBone(), *gameObject, position, rotation, componentAnimation);

		//Updating times
		if (gameObject->name == (*resourceStateMachine->bones.begin())) { // Only udate currentTime for the rootBone
			bool finishedTransition = AnimationController::UpdateTransitions(animationInterpolations, currentTimeStates, App->time->GetDeltaTime());
			//Comparing the state principal with state secondary & set variable for setting secondary state as "empty" State
			if (finishedTransition && principalEqualSecondary) {
				resetSecondaryStatemachine = true;
			}
		}

	} else {
		if (currentState->id != 0) {
			if (principalEqualSecondary) {
				resetSecondaryStatemachine = true;
			}
			
			result = AnimationController::GetTransform(*clip, currentTimeStates[currentState->id], gameObject->name.c_str(), position, rotation, gameObject->name == (*resourceStateMachine->bones.begin()), componentAnimation);
			if (gameObject->name == (*resourceStateMachine->bones.begin())) {
				
			}

		}
	}


	if (gameObject->name == (*resourceStateMachine->bones.begin())) {
		//Sending Event on Finished
		if (!clip->loop) {
			// Checking if the current sample is the last keyframe in order to send the event
			//if (currentTimeStates[currentState->id] >= clip->duration) {
			if (currentSample == clip->endIndex) {
				for (ComponentScript& script : owner.GetComponents<ComponentScript>()) {
					if (script.IsActive()) {
						Script* scriptInstance = script.GetScriptInstance();
						if (scriptInstance != nullptr) {
							switch (stateMachineEnum) {
							case StateMachineEnum::PRINCIPAL:
								scriptInstance->OnAnimationFinished();
								break;
							case StateMachineEnum::SECONDARY:
								scriptInstance->OnAnimationSecondaryFinished();
								break;
							}
						}
					}
				}
			}
		}

		//Sending event on keyframe
		unsigned int currentEventKeyFrame = componentAnimation.listClipsCurrentEventKeyFrames[currentState->clipUid];
		if (!componentAnimation.listClipsKeyEvents[currentState->clipUid].empty()) { //Only call this once
			// Send key Frame event
			int difference = currentSample - currentEventKeyFrame;
			int i = 0;
			if (currentEventKeyFrame == 178) {
				int a = 0;
			}
			for (int i = 0; i <= difference; i++) {
				if ( componentAnimation.listClipsKeyEvents[currentState->clipUid].find(currentEventKeyFrame + i) !=  componentAnimation.listClipsKeyEvents[currentState->clipUid].end() && !componentAnimation.listClipsKeyEvents[currentState->clipUid][currentEventKeyFrame + i].sent) {
					for (ComponentScript& script : owner.GetComponents<ComponentScript>()) {
						if (script.IsActive()) {
							Script* scriptInstance = script.GetScriptInstance();

							if (scriptInstance != nullptr) {
								//LOG("current: %d, CEF: %d", currentSample, currentEventKeyFrame);
								scriptInstance->OnAnimationEvent(stateMachineEnum, componentAnimation.listClipsKeyEvents[currentState->clipUid][currentEventKeyFrame + i].name.c_str());
								componentAnimation.listClipsKeyEvents[currentState->clipUid][currentEventKeyFrame + i].sent = true;
							}
						}
					}
				}
			}

			componentAnimation.listClipsCurrentEventKeyFrames[currentState->clipUid] = currentSample;
			//LOG("Reset currentEventKeyFrame %s %d, dif: %d, %d", std::to_string(owner.id).c_str(), currentSample, difference, currentEventKeyFrame);
		}
	}
	
	return result;
}
