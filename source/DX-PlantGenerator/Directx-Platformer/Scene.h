#pragma once
class Scene
{
public:
	virtual ~Scene() {};
	Scene() = default;

	virtual void OnStart() = 0;
	virtual void OnUpdate() = 0;

private:
};