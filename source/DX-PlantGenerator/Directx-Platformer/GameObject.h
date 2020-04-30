#pragma once
#include <vector>
#include "Component.h"


class GameObject
{
public:
	GameObject();
	~GameObject();

private:
	std::vector<Component> components;

};

