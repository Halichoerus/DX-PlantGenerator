#include "SceneManager.h"

void SceneManager::Update()
{
	if(currentScene)
		currentScene->OnUpdate();
}