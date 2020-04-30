#pragma once
#include <memory>
class Application;
class SceneManager;
class Input;
class Camera;
class Time;

class ServiceLocator
{
	friend class Application;
public:
	static ServiceLocator* GetInstance() { return instance; };
	SceneManager* GetSceneManager() { return sceneManager; };
	Input* GetInput() { return input; };
	Camera* GetCamera() { return camera; };
	Time* GetTime() { return time; };
private:
	static ServiceLocator* instance;
	SceneManager* sceneManager = nullptr;
	Input* input = nullptr;
	Camera* camera = nullptr;
	Time* time = nullptr;
};

