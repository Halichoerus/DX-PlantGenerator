#pragma once
#include "DXDrawable.h"
#include "Transform.h"
#include "Mesh.h"

class Cuboid : public DXDrawable
{
public:
	Cuboid() = default;
	void Load(const wchar_t* path, float width, float height, float depth, bool useCornerOffsets = false, float offsets[4] = { 0 });
	void Draw() override;

	~Cuboid() = default;
	
	void Update();
	
	Transform transform;
	bool uniqueIndex = false;
	Mesh mesh;
private:
	static bool usingStaticIndex;

	WorldConstantBuffer* wcb = nullptr;
};

