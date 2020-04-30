#pragma once

// Base class for all components
class Component
{
public:
	Component() = default;
	Component(int poolIndex, int componentID) : poolID(poolIndex), compID(componentID) {};
	virtual ~Component() = 0;

	int GetPoolID() { return poolID; };
	int GetComponentID() { return compID; };

	virtual void Create() = 0;


private:
	bool isActive = true;
	int poolID = 0;
	int compID = 0;
};

