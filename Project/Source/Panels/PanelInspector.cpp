#include "PanelInspector.h"

#include "GameObject.h"
#include "Application.h"
#include "Components/Component.h"
#include "Components/ComponentType.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentSkybox.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentBillboard.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentAgent.h"
#include "Components/ComponentObstacle.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentBoundingBox2D.h"
#include "Components/UI/ComponentEventSystem.h"
#include "Components/UI/ComponentSelectable.h"
#include "Components/UI/ComponentCanvasRenderer.h"
#include "Components/UI/ComponentText.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentButton.h"
#include "Components/UI/ComponentSlider.h"
#include "Components/UI/ComponentProgressBar.h"
#include "Components/Physics/ComponentSphereCollider.h"
#include "Components/Physics/ComponentBoxCollider.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleRender.h"
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

			bool active = selected->IsActiveInternal();
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
			if (ImGui::InputText("##game_object_name", name, 100)) {
				selected->name = name;
			}

			bool isStatic = selected->IsStatic();
			if (ImGui::Checkbox("Static##game_object_static", &isStatic)) {
				selected->SetStatic(isStatic);
			}

			if (ImGui::Button("Mask")) {
				ImGui::OpenPopup("Mask");
			}

			if (ImGui::BeginPopup("Mask")) {
				for (int i = 0; i < ARRAY_LENGTH(selected->GetMask().maskNames); ++i) {
					bool maskActive = selected->GetMask().maskValues[i];
					if (ImGui::Checkbox(selected->GetMask().maskNames[i], &maskActive)) {
						selected->GetMask().maskValues[i] = maskActive;
						if (selected->GetMask().maskValues[i]) {
							selected->AddMask(GetMaskTypeFromName(selected->GetMask().maskNames[i]));
						} else {
							selected->DeleteMask(GetMaskTypeFromName(selected->GetMask().maskNames[i]));
						}
					}
				}
				ImGui::Separator();
				ImGui::EndPopup();
			}

			ImGui::Separator();

			// Don't show Scene PanelInpector information
			if (selected->GetParent() == nullptr) {
				ImGui::End();
				return;
			}

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
				case ComponentType::IMAGE:
					cName = "Image";
					break;
				case ComponentType::VIDEO:
					cName = "Video";
					break;
				case ComponentType::SELECTABLE:
					cName = "Selectable";
					break;
				case ComponentType::SLIDER:
					cName = "Slider";
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
				case ComponentType::PARTICLE:
					cName = "Particle";
					break;
				case ComponentType::TRAIL:
					cName = "Trail";
					break;
				case ComponentType::BILLBOARD:
					cName = "Billboard";
					break;
				case ComponentType::AUDIO_SOURCE:
					cName = "Audio Source";
					break;
				case ComponentType::AUDIO_LISTENER:
					cName = "Audio Listener";
					break;
				case ComponentType::PROGRESS_BAR:
					cName = "Progress Bar";
					break;
				case ComponentType::SPHERE_COLLIDER:
					cName = "Sphere Collider";
					break;
				case ComponentType::BOX_COLLIDER:
					cName = "Box Collider";
					break;
				case ComponentType::CAPSULE_COLLIDER:
					cName = "Capsule Collider";
					break;
				case ComponentType::AGENT:
					cName = "Agent";
					break;
				case ComponentType::OBSTACLE:
					cName = "Obstacle";
					break;
				case ComponentType::FOG:
					cName = "Fog";
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
						if (ImGui::MenuItem("Remove Component")) {
							componentToDelete = component;
						}
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
				if (ImGui::MenuItem("Fog")) {
					ComponentFog* fog = selected->CreateComponent<ComponentFog>();
					if (fog != nullptr) {
						fog->Init();
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
				AddParticleComponentsOptions(selected);
				AddAudioComponentsOptions(selected);
				AddUIComponentsOptions(selected);
				AddColliderComponentsOptions(selected);
				AddNavigationComponentsOptions(selected);

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

void PanelInspector::AddParticleComponentsOptions(GameObject* selected) {
	if (ImGui::BeginMenu("Particles")) {
		if (ImGui::MenuItem("Particle")) {
			ComponentParticleSystem* particle = selected->CreateComponent<ComponentParticleSystem>();
			if (particle != nullptr) {
				particle->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}
		if (ImGui::MenuItem("Billboard")) {
			ComponentBillboard* billboard = selected->CreateComponent<ComponentBillboard>();
			if (billboard != nullptr) {
				billboard->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}
		if (ImGui::MenuItem("Trail")) {
			ComponentTrail* trail = selected->CreateComponent<ComponentTrail>();
			if (trail != nullptr) {
				trail->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}
		ImGui::EndMenu();
	}
}

void PanelInspector::AddAudioComponentsOptions(GameObject* selected) {
	if (ImGui::BeginMenu("Audio")) {
		if (ImGui::MenuItem("Audio Source")) {
			ComponentAudioSource* audioSource = selected->CreateComponent<ComponentAudioSource>();
			if (audioSource != nullptr) {
				audioSource->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}
		if (ImGui::MenuItem("Audio Listener")) {
			ComponentAudioListener* audioListener = selected->CreateComponent<ComponentAudioListener>();
			if (audioListener != nullptr) {
				audioListener->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}
		ImGui::EndMenu();
	}
}

void PanelInspector::AddUIComponentsOptions(GameObject* selected) {
	if (ImGui::BeginMenu("UI")) {
		// ------ CREATING UI HANDLERS -------

		ComponentType typeToCreate = ComponentType::UNKNOWN;

		if (ImGui::MenuItem("Event System")) {
			if (App->scene->scene->eventSystemComponents.Count() == 0) {
				typeToCreate = ComponentType::EVENT_SYSTEM;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("Canvas")) {
			typeToCreate = ComponentType::CANVAS;
		}

		// ------ CREATING UI ELEMENTS -------
		bool newUIComponentCreated = false;	 // If a UI component is created, we will create a Transform2D and a CanvasRenderer components for it
		bool newUISelectableCreated = false; // In addition, if it is a selectable element, a ComponentBoundingBox2D will also be created
		if (ImGui::MenuItem("Image")) {
			ComponentImage* component = selected->GetComponent<ComponentImage>();
			if (component == nullptr) {
				newUIComponentCreated = true;
				typeToCreate = ComponentType::IMAGE;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("Video")) {
			ComponentVideo* component = selected->GetComponent<ComponentVideo>();
			if (component == nullptr) {
				newUIComponentCreated = true;
				typeToCreate = ComponentType::VIDEO;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("Text")) {
			ComponentText* component = selected->GetComponent<ComponentText>();
			if (component == nullptr) {
				newUIComponentCreated = true;
				typeToCreate = ComponentType::TEXT;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		// ----- Selectables

		if (ImGui::MenuItem("Button")) {
			ComponentButton* component = selected->GetComponent<ComponentButton>();
			if (component == nullptr) {
				typeToCreate = ComponentType::BUTTON;
				newUIComponentCreated = true;
				newUISelectableCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("Toggle")) {
			ComponentToggle* component = selected->GetComponent<ComponentToggle>();
			if (component == nullptr) {
				typeToCreate = ComponentType::TOGGLE;
				newUIComponentCreated = true;
				newUISelectableCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("ProgressBar")) {
			ComponentProgressBar* component = selected->GetComponent<ComponentProgressBar>();
			if (component == nullptr) {
				typeToCreate = ComponentType::PROGRESS_BAR;
				newUIComponentCreated = true;
				newUISelectableCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		if (ImGui::MenuItem("Slider")) {
			ComponentSlider* component = selected->GetComponent<ComponentSlider>();
			if (component == nullptr) {
				typeToCreate = ComponentType::SLIDER;
				newUIComponentCreated = true;
				newUISelectableCreated = true;
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

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
					selectable->SetSelectableType(typeToCreate);
				}
			}
		}

		switch (typeToCreate) {
		case ComponentType::EVENT_SYSTEM: {
			ComponentEventSystem* component = selected->CreateComponent<ComponentEventSystem>();
			if (component != nullptr) {
				component->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
			break;
		}
		case ComponentType::CANVAS: {
			ComponentTransform2D* transform = selected->GetComponent<ComponentTransform2D>();
			if (transform == nullptr) {
				transform = selected->CreateComponent<ComponentTransform2D>();
				transform->Init();
			}
			ComponentCanvas* component = selected->CreateComponent<ComponentCanvas>();
			if (component != nullptr) {
				component->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
			break;
		}
		case ComponentType::IMAGE: {
			ComponentImage* component = selected->CreateComponent<ComponentImage>();
			component->Init();
			break;
		}
		case ComponentType::VIDEO: {
			ComponentVideo* component = selected->CreateComponent<ComponentVideo>();
			component->Init();
			break;
		}
		case ComponentType::TEXT: {
			ComponentText* component = selected->CreateComponent<ComponentText>();
			component->Init();
			break;
		}
		case ComponentType::BUTTON: {
			ComponentButton* component = selected->CreateComponent<ComponentButton>();

			component->Init();

			if (!selected->GetComponent<ComponentImage>()) {
				ComponentImage* image = selected->CreateComponent<ComponentImage>();
				image->Init();
			}
			break;
		}
		case ComponentType::TOGGLE: {
			ComponentToggle* component = selected->CreateComponent<ComponentToggle>();
			component->Init();
			if (!selected->GetComponent<ComponentImage>()) {
				ComponentImage* image = selected->CreateComponent<ComponentImage>();
				image->Init();
			}
			break;
		}
		case ComponentType::PROGRESS_BAR: {
			ComponentProgressBar* component = selected->CreateComponent<ComponentProgressBar>();
			component->Init();

			GameObject* background = App->scene->scene->CreateGameObject(selected, GenerateUID(), "Background");
			ComponentTransform2D* transform2D = background->CreateComponent<ComponentTransform2D>();
			ComponentCanvasRenderer* canvasRenderer = background->CreateComponent<ComponentCanvasRenderer>();
			ComponentImage* image = background->CreateComponent<ComponentImage>();
			background->Init();
			transform2D->SetSize(float2(700, 80));

			GameObject* fill = App->scene->scene->CreateGameObject(selected, GenerateUID(), "Fill");
			ComponentTransform2D* transform2DFill = fill->CreateComponent<ComponentTransform2D>();
			ComponentCanvasRenderer* canvasRendererFill = fill->CreateComponent<ComponentCanvasRenderer>();
			ComponentImage* imageFill = fill->CreateComponent<ComponentImage>();
			fill->Init();
			imageFill->SetColor(float4(1.f, 0, 0, 1.f));
			break;
		}
		case ComponentType::SLIDER: {
			ComponentSlider* component = selected->CreateComponent<ComponentSlider>();
			component->Init();
			if (!selected->GetComponent<ComponentImage>()) {
				ComponentImage* image = selected->CreateComponent<ComponentImage>();
				image->Init();
			}

			GameObject* background = App->scene->scene->CreateGameObject(selected, GenerateUID(), "Background");
			ComponentTransform2D* transform2D = background->CreateComponent<ComponentTransform2D>();
			ComponentCanvasRenderer* canvasRenderer = background->CreateComponent<ComponentCanvasRenderer>();
			ComponentImage* image = background->CreateComponent<ComponentImage>();
			background->Init();
			transform2D->SetSize(float2(700, 80));

			GameObject* fill = App->scene->scene->CreateGameObject(selected, GenerateUID(), "Fill");
			ComponentTransform2D* transform2DFill = fill->CreateComponent<ComponentTransform2D>();
			ComponentCanvasRenderer* canvasRendererFill = fill->CreateComponent<ComponentCanvasRenderer>();
			ComponentImage* imageFill = fill->CreateComponent<ComponentImage>();
			fill->Init();

			GameObject* handle = App->scene->scene->CreateGameObject(selected, GenerateUID(), "Handle");
			ComponentTransform2D* transform2DHandle = fill->CreateComponent<ComponentTransform2D>();
			ComponentCanvasRenderer* canvasRendererHandle = fill->CreateComponent<ComponentCanvasRenderer>();
			ComponentImage* imageHandle = fill->CreateComponent<ComponentImage>();
			fill->Init();
			break;
		}
		} // ens switch

		ImGui::EndMenu();
	}
}

void PanelInspector::AddColliderComponentsOptions(GameObject* selected) {
	if (ImGui::BeginMenu("Collider")) {
		if (ImGui::MenuItem("Sphere Collider")) {
			ComponentSphereCollider* sphereCollider = selected->CreateComponent<ComponentSphereCollider>();
			if (sphereCollider != nullptr) {
				sphereCollider->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS; // TODO: Control other colliders exists.
			}
		}

		if (ImGui::MenuItem("Box Collider")) {
			ComponentBoxCollider* boxCollider = selected->CreateComponent<ComponentBoxCollider>();
			if (boxCollider != nullptr) {
				boxCollider->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS; // TODO: Control other colliders exists.
			}
		}

		if (ImGui::MenuItem("Capsule Collider")) {
			ComponentCapsuleCollider* capsuleComponent = selected->CreateComponent<ComponentCapsuleCollider>();
			if (capsuleComponent != nullptr) {
				capsuleComponent->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS; // TODO: Control other colliders exists.
			}
		}

		ImGui::EndMenu();
	}
}

void PanelInspector::AddNavigationComponentsOptions(GameObject* selected) {
	if (ImGui::BeginMenu("Navigation")) {
		if (ImGui::MenuItem("Agent")) {
			ComponentAgent* agent = selected->CreateComponent<ComponentAgent>();
			if (agent != nullptr) {
				agent->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}
		if (ImGui::MenuItem("Obstacle")) {
			ComponentObstacle* obstacle = selected->CreateComponent<ComponentObstacle>();
			if (obstacle != nullptr) {
				obstacle->Init();
			} else {
				App->editor->modalToOpen = Modal::COMPONENT_EXISTS;
			}
		}

		ImGui::EndMenu();
	}
}
