#pragma once

#include "Module.h"

enum class WindowMode;
struct SDL_Window;
struct SDL_Surface;

class ModuleWindow : public Module
{
public:
	bool Init() override;
	bool CleanUp() override;

	void SetWindowMode(WindowMode mode);
	void SetResizable(bool resizable);
	void SetSize(int width, int height);
	void SetBrightness(float brightness);
	void SetTitle(const char* title);

public:
	SDL_Window* window = nullptr;
	SDL_Surface* surface = nullptr;
};