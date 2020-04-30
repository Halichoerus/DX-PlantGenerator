#include "OpenWorldScene.h"
#include "ServiceLocator.h"
#include "Time.h"


#include "WorldConstantBuffer.h"

OpenWorldScene::~OpenWorldScene()
{
}

void OpenWorldScene::OnStart()
{
	input = ServiceLocator::GetInstance()->GetInput();
	camera = ServiceLocator::GetInstance()->GetCamera();

	testLight = std::make_unique<PointLight>();
	testLight->GetPos().x = 0;
	testLight->GetPos().y = 0;
	testLight->GetPos().z = 0;

	camera->Translate({ 0,7.5f,0 });

	world.UpdatePlayerPos(ServiceLocator::GetInstance()->GetCamera()->GetPosition());

	world.Load();

}

void OpenWorldScene::OnUpdate()
{
	int fps = ServiceLocator::GetInstance()->GetTime()->FPS();

	if (input->IsKeyDown('W'))
	{
		camera->Translate({ 0,0,0.1 });
	}
	else if (input->IsKeyDown('S'))
	{
		camera->Translate({ 0,0,-0.1 });
	}
	if (input->IsKeyDown('A'))
	{
		camera->Translate({ -0.1,0,0 });
	}
	else if (input->IsKeyDown('D'))
	{
		camera->Translate({ 0.1,0,0 });
	}
	if (input->IsKeyDown('Q'))
	{
		camera->Translate({ 0,-0.1,0 });
	}
	else if (input->IsKeyDown('E'))
	{
		camera->Translate({ 0,0.1,0 });
	}

	if (input->IsKeyDown('L'))
	{
		camera->Rotate({ 0,0.01,0 });
	}
	else if (input->IsKeyDown('J'))
	{
		camera->Rotate({ 0,-0.01,0 });
	}
	if (input->IsKeyDown('I'))
	{
		camera->Rotate({ -0.01,0,0 });
	}
	else if (input->IsKeyDown('K'))
	{
		camera->Rotate({ 0.01,0,0 });
	}
	if (input->IsKeyDown('U'))
	{
		camera->Rotate({ 0,0,-0.01 });
	}
	else if (input->IsKeyDown('O'))
	{
		camera->Rotate({ 0,0,0.01 });
	}


	camera->Rotate({ input->GetRawMouseDelta().y/50.0f, input->GetRawMouseDelta().x/50.0f, 0 });

	if (input->IsRightMouseButtonDown())
	{
		std::pair<int, int> chunkIndex;
		int index = world.GetIndexHitByRay(camera->GetPosition(), camera->GetPositionForwardOfCamera(1), chunkIndex);
		world.SetBlockAtIndex(chunkIndex, index, Block::BlockType::grass);
	}
	else if (input->IsLeftMouseButtonDown())
	{
		std::pair<int, int> chunkIndex;
		int index = world.GetIndexHitByRay(camera->GetPosition(), camera->GetPositionForwardOfCamera(1), chunkIndex);
		if(index != -1)
			world.SetBlockAtIndex(chunkIndex, index, Block::BlockType::air);
	}

	world.UpdatePlayerPos(camera->GetPosition());
	world.Update();

	// Draw
	testLight->Bind();
	world.Draw();

}
