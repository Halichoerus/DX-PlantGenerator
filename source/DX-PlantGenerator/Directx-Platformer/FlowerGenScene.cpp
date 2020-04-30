#include "FlowerGenScene.h"
#include "ServiceLocator.h"
#include "Input.h"
#include "Camera.h"

void FlowerGenScene::OnStart()
{
	ServiceLocator::GetInstance()->GetCamera()->SetPosition({ 0,0,-4 });
	//testCube = std::make_unique<Cuboid>(1,1,1);

	testCuboid = std::make_unique<Cuboid>();
	testCuboid->Load(L"Textures/Stems/Grass.jpg", 0.2, 0.2, 0.2);

	flower = std::make_unique<Flower>();

	testLight = std::make_unique<PointLight>();
	testLight->GetPos().x = 2;
	testLight->GetPos().y = 2;
	testLight->GetPos().z = -1;


}

void FlowerGenScene::OnUpdate()
{

	// Test keyboard input
	//if (window.input.IsKeyDown('W'))
	//{
	//	testCube->transform.yPosition += 0.1f;
	//}
	//if (window.input.IsKeyDown('S'))
	//{
	//	testCube->transform.yPosition -= 0.1f;
	//}
	//if (window.input.IsKeyDown('D'))
	//{
	//	testCube->transform.xPosition += 0.1f;
	//}
	//if (window.input.IsKeyDown('A'))
	//{
	//	testCube->transform.xPosition -= 0.1f;
	//}
	//if (window.input.IsKeyDown('Q'))
	//{
	//	testCube->transform.zPosition += 0.1f;
	//}
	//if (window.input.IsKeyDown('E'))
	//{
	//	testCube->transform.zPosition -= 0.1f;
	//}

	if (ServiceLocator::GetInstance()->GetInput()->IsKeyDown('I'))
	{
		testLight->GetPos().y += 0.1f;
	}
	if (ServiceLocator::GetInstance()->GetInput()->IsKeyDown('K'))
	{
		testLight->GetPos().y -= 0.1f;
	}
	if (ServiceLocator::GetInstance()->GetInput()->IsKeyDown('L'))
	{
		testLight->GetPos().x += 0.1f;
	}
	if (ServiceLocator::GetInstance()->GetInput()->IsKeyDown('J'))
	{
		testLight->GetPos().x -= 0.1f;
	}
	if (ServiceLocator::GetInstance()->GetInput()->IsKeyDown('U'))
	{
		testLight->GetPos().z += 0.1f;
	}
	if (ServiceLocator::GetInstance()->GetInput()->IsKeyDown('O'))
	{
		testLight->GetPos().z -= 0.1f;
	}

	testAngle += 0.01f;
	//testCube->transform.xRotation = testAngle;
	//testCube->transform.yRotation = testAngle;
	//testCube->transform.zRotation = testAngle;

	testCuboid->transform.Position() = testLight->GetPos();

	testLight->Bind();

	//testCube->Update();
	testCuboid->Update();
	flower->Update();

	//testCube->Draw();
	testCuboid->Draw();
	flower->Draw();
}