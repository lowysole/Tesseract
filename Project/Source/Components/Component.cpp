#include "Component.h"

#include "FileSystem/JsonValue.h"
#include "Resources/GameObject.h"

#include "Utils/Leaks.h"

Component::Component(ComponentType type_, GameObject* owner_, UID id_, bool active_)
	: type(type_)
	, owner(owner_)
	, id(id_)
	, active(active_) {}

Component::~Component() {}

void Component::Init() {}

void Component::Update() {}

void Component::DrawGizmos() {}

void Component::OnTransformUpdate() {}

void Component::OnEditorUpdate() {}

void Component::Save(JsonValue jComponent) const {}

void Component::Load(JsonValue jComponent) {}

void Component::Enable() {
	active = true;
}

void Component::Disable() {
	active = false;
}

ComponentType Component::GetType() const {
	return type;
}

GameObject& Component::GetOwner() const {
	return *owner;
}

UID Component::GetID() const {
	return id;
}

bool Component::IsActive() const {
	return active;
}

bool Component::IsActiveInHierarchy() const {
	return active && owner->IsActiveInHierarchy();
}
