#include "MainApplication.h"
#include "ServiceLocator.h"
#include "SceneManager.h"
#include "FlowerGenScene.h"
#include "Input.h"


void MainApplication::OnStart()
{
	ServiceLocator::GetInstance()->GetSceneManager()->LoadScene<FlowerGenScene>();
}

void MainApplication::OnUpdate()
{
	if (ServiceLocator::GetInstance()->GetInput()->IsKeyDown('1'))
	{
		ServiceLocator::GetInstance()->GetSceneManager()->LoadScene<FlowerGenScene>();
	}

	ServiceLocator::GetInstance()->GetSceneManager()->Update();
}