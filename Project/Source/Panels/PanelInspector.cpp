#include "PanelInspector.h"

#include "Application.h"
#include "GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentType.h"
#include "Components/UI/ComponentText.h"

#include "Modules/ModuleEditor.h"
#include "Modules/ModuleUserInterface.h"

#include "Math/float3.h"
#include "Math/float3x3.h"
#include "Math/float4x4.h"

#include "GL/glew.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"

#include "Utils/Leaks.h"

PanelInspector::PanelInspector()
	: Panel("Inspector", true) {}

void PanelInspector::Update() {
	ImGui::SetNextWindowDockID(App->editor->dockRightId, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FA_CUBE " ") + name;
	std::string optionsSymbol = std::string(ICON_FK_COG);
	if (ImGui::Begin(windowName.c_str(), &enabled)) {
		GameObject* selected = App->editor->selectedGameObject;
		if (selected != nullptr) {
			ImGui::TextUnformatted("Id:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%llu", selected->GetID());

			bool active = selected->IsActive();
			if (ImGui::Checkbox("##game_object", &active)) {
				// TODO: EventSystem would generate an event here
				if (active) {
					selected->Enable();
				} else {
					selected->Disable();
				}
			}
			ImGui::SameLine();
			char name[100];
			sprintf_s(name, 100, "%s", selected->name.c_str());
			if (ImGui::InputText("", name, 100)) {
				selected->name = name;
			}

			// TODO: Fix Me
			ImGui::SameLine();
			HelpMarker("To Fix it.");

			ImGui::Separator();

			// Show Component info
			std::string cName = "";
			for (Component* component : selected->GetComponents()) {
				switch (component->GetType()) {
				case ComponentType::TRANSFORM:
					cName = "Transformation";
					break;
				case ComponentType::MESH_RENDERER:
					cName = "Mesh Renderer";
					break;
				case ComponentType::CAMERA:
					cName = "Camera";
					break;
				case ComponentType::LIGHT:
					cName = "Light";
					break;
				case ComponentType::BOUNDING_BOX:
					cName = "Bounding Box";
					break;
				case ComponentType::TRANSFORM2D:
					cName = "Rect Transform";
					break;
				case ComponentType::BOUNDING_BOX_2D:
					cName = "Bounding Box 2D";
					break;
				case ComponentType::EVENT_SYSTEM:
					cName = "Event System";
					break;
				case ComponentType::IMAGE:
					cName = "Image";
					break;
				case ComponentType::CANVAS:
					cName = "Canvas";
					break;
				case ComponentType::CANVASRENDERER:
					cName = "Canvas Renderer";
					break;
				case ComponentType::BUTTON:
					cName = "Button";
					break;
				case ComponentType::TOGGLE:
					cName = "Toggle";
					break;
				case ComponentType::TEXT:
					cName = "Text";
					break;
				case ComponentType::SELECTABLE:
					cName = "Selectable";
					break;
				case ComponentType::SKYBOX:
					cName = "Skybox";
					break;
				case ComponentType::SCRIPT:
					cName = "Script";
					break;
				case ComponentType::ANIMATION:
					cName = "Animation";
					break;
				default:
					cName = "";
					break;
				}

				ImGui::PushID(component);

				// TODO: Place TransformComponent always th the top of the Inspector
				bool headerOpen = ImGui::CollapsingHeader(cName.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);

				// Options BUTTON (in the same line and at the end of the line)
				ImGui::SameLine();
				if (ImGui::GetWindowWidth() > 170) ImGui::Indent(ImGui::GetWindowWidth() - 43);
				if (ImGui::Button(optionsSymbol.c_str())) ImGui::OpenPopup("Component Options");
				// More Component buttons (edit the Indention)...
				if (ImGui::GetWindowWidth() > 170) ImGui::Unindent(ImGui::GetWindowWidth() - 43);

				// Options POPUP
				if (ImGui::BeginPopup("Component Options")) {
					if (component->GetType() != ComponentType::TRANSFORM) {
						if (ImGui::MenuItem("Remove Component")) componentToDelete = component;
						// TODO: force remove other components that this one requires for functioning
					}
					// More Options items ...
					ImGui::EndPopup();
				}

				// Show Component info
				if (headerOpen) component->OnEditorUpdate();

				ImGui::PopID();
			}

			// Add New Components
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();
			if (ImGui::Button("Add New Component", ImVec2(ImGui::GetContentRegionAvail().x, 25))) { ImGui::OpenPopup("AddComponentPopup"); }
			if (ImGui::BeginPopup("AddComponentPopup")) {
				// Add a Component of type X. If a Component of the same type exists, it wont be created and the modal COMPONENT_EXISTS will show up.
				if (ImGui::MenuItem("Mesh Renderer")) {
					ComponentMeshRenderer* meshRenderer = selected->CreateComponent<ComponentMeshRenderer>();
					if (meshRenderer != nullptr) {
						meshRenderer->Init();
					} else {
						App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
					}
				}
				if (ImGui::MenuItem("Camera")) {
					ComponentCamera* camera = selected->CreateComponent<ComponentCamera>();
					if (camera != nullptr) {
						camera->Init();
					} else {
						App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
					}
				}
				if (ImGui::MenuItem("Light")) {
					ComponentLight* light = selected->CreateComponent<ComponentLight>();
					if (light != nullptr) {
						light->Init();
					} else {
						App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
					}
				}
				if (ImGui::MenuItem("Skybox")) {
					ComponentSkyBox* skybox = selected->CreateComponent<ComponentSkyBox>();
					if (skybox != nullptr) {
						skybox->Init();
					} else {
						App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
					}
				}
				if (ImGui::MenuItem("Script")) {
					ComponentScript* script = selected->CreateComponent<ComponentScript>();
					if (script != nullptr) {
						script->Init();
					} else {
						App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
					}
				}
				if (ImGui::MenuItem("Animation")) {
					ComponentAnimation* animation = selected->CreateComponent<ComponentAnimation>();
					if (animation != nullptr) {
						animation->Init();
					} else {
						App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
					}
				}
				// TRANSFORM is always there, cannot add a new one.

				AddUIComponentsOptions(selected);

				ImGui::EndPopup();
			}
		}
	}
	ImGui::End();
}

Component* PanelInspector::GetComponentToDelete() const {
	return componentToDelete;
}

void PanelInspector::SetComponentToDelete(Component* comp) {
	componentToDelete = comp;
}

void PanelInspector::AddUIComponentsOptions(GameObject* selected) {
	if (ImGui::BeginMenu("UI")) {
		// ------ CREATING UI HANDLERS -------
		if (ImGui::MenuItem("Event System")) {
			ComponentEventSystem* component = selected->CreateComponent<ComponentEventSystem>();
			if (component != nullptr) {
				component->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("Canvas")) {
			ComponentCanvas* component = selected->CreateComponent<ComponentCanvas>();
			if (component != nullptr) {
				component->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		// ------ CREATING UI ELEMENTS -------
		bool newUIComponentCreated = false;	 // If a UI component is created, we will create a Transform2D and a CanvasRenderer components for it
		bool newUISelectableCreated = false; // In addition, if it is a selectable element, a ComponentBoundingBox2D will also be created
		if (ImGui::MenuItem("Image")) {
			ComponentImage* component = selected->CreateComponent<ComponentImage>();
			if (component != nullptr) {
				component->Init();
				newUIComponentCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("Text")) {
			ComponentText* component = selected->CreateComponent<ComponentText>();
			if (component != nullptr) {
				component->Init();
				newUIComponentCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}
		// MenuItem("TextLabel")
		// MenuItem("ProgressBar")
		// ...

		// Selectables
		ComponentType tmpType = ComponentType::UNKNOWN;

		if (ImGui::MenuItem("Button")) {
			ComponentButton* component = selected->CreateComponent<ComponentButton>();
			if (component != nullptr) {
				component->Init();
				tmpType = component->GetType();
				//ComponentEventSystem::m_Selectables.push_back(component);
				newUIComponentCreated = true;
				newUISelectableCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}
		// MenuItem("Toggle")
		// MenuItem("InputText")
		// MenuItem("ScrollBar")
		// ...

		if (newUIComponentCreated) {
			// Create new Transform2D
			ComponentTransform2D* transform2d = selected->CreateComponent<ComponentTransform2D>();
			if (transform2d != nullptr) transform2d->Init();

			// Create new Canvas Renderer
			ComponentCanvasRenderer* canvasRender = selected->CreateComponent<ComponentCanvasRenderer>();
			if (canvasRender != nullptr) canvasRender->Init();

			if (newUISelectableCreated) {
				ComponentBoundingBox2D* boundingBox2d = selected->CreateComponent<ComponentBoundingBox2D>();
				ComponentSelectable* selectable = selected->CreateComponent<ComponentSelectable>();
				if (boundingBox2d != nullptr) boundingBox2d->Init();
				if (selectable != nullptr) {
					selectable->Init();
					selectable->SetSelectableType(tmpType);
				}
			}
		}
		ImGui::EndMenu();
	}
}
