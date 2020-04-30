#pragma once
#include "Scene.h"

class SceneManager
{
public:
	void Update();

	template <class T>
	void LoadScene()
	{
		T* newScene = new T();
		delete currentScene;
		currentScene = static_cast<Scene*>(newScene);
		currentScene->OnStart();
	}

private:
	Scene* currentScene = nullptr;
};

