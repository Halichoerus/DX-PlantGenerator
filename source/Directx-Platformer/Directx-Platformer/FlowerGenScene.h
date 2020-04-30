#pragma once
#include <memory>

#include "Scene.h"
#include "Cuboid.h"
#include "PointLight.h"
#include "Flower.h"

class FlowerGenScene :
	public Scene
{
public:
	~FlowerGenScene() {};

	void OnStart() override;
	void OnUpdate() override;


private: 
	float testAngle = 0;
	float testX = 0;
	float testY = 0;
	float testZ = 0;

	//std::unique_ptr<Cuboid> testCube = nullptr;
	std::unique_ptr<Cuboid> testCuboid = nullptr;
	std::unique_ptr<Flower> flower = nullptr;
	std::unique_ptr<PointLight> testLight = nullptr;
};

