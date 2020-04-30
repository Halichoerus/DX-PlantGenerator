#pragma once
#include <vector>
#include <atomic>
#include <mutex>

#include "Transform.h"
#include "DXDrawable.h"
#include "WorldData.h"
#include "Constants.h"

class Chunk : public DXDrawable
{
public:
	Chunk() = default;
	~Chunk();

	std::pair<int, int> GenerateFromHeightmap(byte* heightMap, DirectX::XMFLOAT3 chunkPos, std::pair<int, int> id, WorldData* data);
	void CalculateVisibleBlocks();
	void CalculateVisibleBlocksOnSides(Chunk* neighbours[4], bool callOnNeighbours);

	const int* GetHeightmap() { return heightMap; };
	void Update();
	void Draw();

	const DirectX::XMFLOAT3& GetPos() { return pos; };

	const bool& ShouldUnload() const { return shouldUnload; };
	void SetUnloadFlag(bool flag) { shouldUnload = flag; };

	bool IsLoaded() { return loaded; };

	Block* GetBlockAtLocation(DirectX::XMFLOAT3 location);
	void SetBlockAtLocation(DirectX::XMFLOAT3 location, Block::BlockType type);
	Block* GetBlockAtIndex(int index);
	void SetBlockAtIndex(int index, Block::BlockType type);

	bool DoesLineIntersectBlock(const DirectX::XMFLOAT3 startPos, const DirectX::XMFLOAT3& endPos, int index, float& distOut);

private:
	ChunkSaveData* GetSaveData();
	void LoadIndexIntoMesh(int index);

	ChunkSaveData saveData;
	WorldData* worldData = nullptr;

	Mesh mesh;
	Transform transform;

	std::atomic_bool loaded = false;

	DirectX::XMFLOAT3 pos = { 0,0,0 };
	std::pair<int, int> chunkID;
	bool shouldUnload = true;

	Block blocks[CHUNK_SIZE];
	int heightMap[CHUNK_HM_SIZE];
	std::vector<int> visibleBlocks;

	std::mutex deleteLock;

	bool blocksBound = false;
	std::atomic_bool meshDirty = false;
};