#pragma once

#include "Module.h"

#include "Math/float3.h"
#include "Math/float4x4.h"

class GameObject;

class ModuleRender : public Module
{
public:
	bool Init() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

	void ViewportResized(int width, int height);

	void SetVSync(bool vsync);

public:
	void* context = nullptr;
	unsigned render_texture = 0;
	unsigned depth_renderbuffer = 0;
	unsigned framebuffer = 0;

	unsigned viewport_width = 0;
	unsigned viewport_height = 0;

	float3 clear_color = {0.1f, 0.1f, 0.1f};
	float3 ambient_color = {0.0f, 0.0f, 0.0f};

private:
	void DrawGameObject(GameObject* game_object);
};
