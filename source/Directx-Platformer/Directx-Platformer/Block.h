#pragma once
#include "Mesh.h"

class Block
{
public:
	friend class Chunk;

	enum class BlockType : char
	{
		air = 0,
		grass
	};

	Block() = default;
	void Load(BlockType blockType, bool useCornerOffsets = false, float offsets[4] = {});

	//void SetPosition(DirectX::XMFLOAT3 position);
	//DirectX::XMFLOAT3 GetPosition() { return model.transform.Position(); };
	void Bind();
	void Draw();
	void Update();

	bool HasUniqueVerts();
	Mesh GetMesh();

	BlockType type;
	bool blockSight = false;
	bool sidesVisible[6] = { false,false,false,false,false,false }; // North, East, South, West, Up, Down
	char visibleSidesCount = 0;
private:
	bool uniqueIndex = false;
	Mesh mesh;
	static Mesh sMesh;
	static bool meshMade;

};