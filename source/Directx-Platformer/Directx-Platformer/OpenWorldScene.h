#pragma once
#include "Scene.h"
#include "Model.h"
#include "Input.h"
#include "Camera.h"
#include "PointLight.h"
#include "World.h"

class OpenWorldScene : public Scene
{
public:
	~OpenWorldScene();

	void OnStart() override;
	void OnUpdate() override;

private:
	World world;

	Camera* camera = nullptr;
	Input* input = nullptr;

	//Model player;
	float playerSpeed = 10.f;
	std::unique_ptr<PointLight> testLight = nullptr;

	//Chunk* testChunk[5] = { nullptr,nullptr,nullptr,nullptr,nullptr };
	bool debugToggle = false;
};