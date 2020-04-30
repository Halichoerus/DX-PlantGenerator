#include "MainApplication.h"
#include "ServiceLocator.h"
#include "SceneManager.h"
#include "FlowerGenScene.h"
#include "OpenWorldScene.h"
#include "Input.h"


void MainApplication::OnStart()
{
	//ServiceLocator::GetInstance()->GetSceneManager()->LoadScene<FlowerGenScene>();
	ServiceLocator::GetInstance()->GetSceneManager()->LoadScene<OpenWorldScene>();
}

void MainApplication::OnUpdate()
{
	if (ServiceLocator::GetInstance()->GetInput()->IsKeyDown('1'))
	{
		ServiceLocator::GetInstance()->GetSceneManager()->LoadScene<FlowerGenScene>();
	}
	else if (ServiceLocator::GetInstance()->GetInput()->IsKeyDown('2'))
	{
		ServiceLocator::GetInstance()->GetSceneManager()->LoadScene<OpenWorldScene>();
	}

	ServiceLocator::GetInstance()->GetSceneManager()->Update();
}