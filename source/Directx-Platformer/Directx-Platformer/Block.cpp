#include "Block.h"
#include "Constants.h"

Mesh Block::sMesh;
bool Block::meshMade = false;

void Block::Load(BlockType blockType, bool useCornerOffsets, float offsets[4])
{
	type = blockType;

	switch (type)
	{
		case BlockType::air:
		{
			blockSight = false;
			break;
		}
		case BlockType::grass:
		{
			//model.Load(L"Textures/Stems/Grass.jpg", BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, useCornerOffsets, offsets);
			blockSight = true;
			break;
		}
	}

	if (useCornerOffsets)
	{
		uniqueIndex = true;
		mesh.CreateCuboidMesh(BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, useCornerOffsets, offsets);
		mesh.GenerateNormals();
	}
	else if(!meshMade)
	{
		sMesh.CreateCuboidMesh(BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
		sMesh.GenerateNormals();
		meshMade = true;
	}
}

void Block::Draw()
{
	//model.Update();
	//model.Draw();
}

void Block::Bind()
{
	//model.BindAll();
}

void Block::Update()
{
	//model.Update();
}

//void Block::SetPosition(DirectX::XMFLOAT3 position)
//{
//	model.transform.Position() = position;
//}

bool Block::HasUniqueVerts()
{
	if (type == BlockType::air)
		return false;
	else
		return uniqueIndex;
}

Mesh Block::GetMesh()
{
	if (uniqueIndex)
		return mesh;
	else
		return sMesh;
}
