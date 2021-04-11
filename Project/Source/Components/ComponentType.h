#pragma once

/* Creating a new component type:
*    1. Add a new ComponentType for the new component
*    2. Add REGISTER_COMPONENT to the .h of the new component
*    3. Add the new component to the GetComponentTypeName and GetComponentTypeFromName functions in ComponentType.cpp
*    4. Create a new VectorMap for the new component in Scene.h
*    5. Add the new component to the ByTypeAndId functions in GameObject.cpp
*    6. Implement DuplicateComponent for the new component
*    7. Add the "Add Component" functionality in PanelInspector.cpp (ln. 62)
*/

// REGISTER_COMPONENT builds the data structures common to all Components.
// This includes the Constructor.
#define REGISTER_COMPONENT(componentClass, componentType, allowMultiple) \
	static const ComponentType staticType = componentType;               \
	static const bool allowMultipleComponents = allowMultiple;           \
	componentClass(GameObject* owner, UID id, bool active) : Component(staticType, owner, id, active) {}

enum class ComponentType {
	UNKNOWN,
	TRANSFORM,
	MESH_RENDERER,
	BOUNDING_BOX,
	CAMERA,
	LIGHT,
	CANVAS,
	CANVASRENDERER,
	IMAGE,
	TRANSFORM2D,
	BUTTON,
	EVENT_SYSTEM,
	BOUNDING_BOX_2D,
	TOGGLE,
	SELECTABLE,
	SKYBOX,
	ANIMATION,
	TEXT,
	SCRIPT
};

const char* GetComponentTypeName(ComponentType type);
ComponentType GetComponentTypeFromName(const char* name);