#pragma once
#include <chrono>
#include "Window.h"
#include "ImguiManager.h"
#include "ServiceLocator.h"
#include "SceneManager.h"
#include "MainApplication.h"
#include "Time.h"

class Application
{
public:
	Application(int width, int height, const char* name);
	~Application();

	int Start();

private:
	int ProcessMessages();
	void Tick();

	ImguiManager imgui;

	Window window;
	int fixedTickRate;

	MainApplication mainApp;
	ServiceLocator serviceLocator;
	SceneManager sceneManager;
	Time time;
};

