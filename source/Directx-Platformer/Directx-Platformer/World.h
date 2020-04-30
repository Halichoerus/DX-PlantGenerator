#pragma once
#include <map>
#include <future>
#include <queue>
#include "Chunk.h"
#include "HeightMap.h"
#include "WorldData.h"

class World
{
public:
	World() = default;
	
	void Load();
	void Update();
	void Draw();
	void UpdatePlayerPos(DirectX::XMFLOAT3 pos);
	void SetLoadRadius(float radius);

	Block* GetBlockAtLocation(DirectX::XMFLOAT3 location, Chunk* chunk);
	void SetBlockAtLocation(DirectX::XMFLOAT3 location, Block::BlockType type);
	Block* GetBlockAtIndex(std::pair<int, int> chunkIndex, int blockIndex);
	void SetBlockAtIndex(std::pair<int, int> chunkIndex, int blockIndex, Block::BlockType type);
	int GetIndexHitByRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, std::pair<int, int>& outChunkIndex);

private:
	void CalculteVisibleChunks();
	bool ShouldLoadChunk(float xPos, float zPosk);
	void LoadChunkAsync(std::pair<int, int> chunk);
	Chunk* GetChunkIfExsists(std::pair<int, int> chunk);

	byte* GetHeightmap(std::pair<int, int> chunk);
	void GenerateHeightmap(std::pair<int, int> location, bool keepEdges = false);
	void JoinHeightmapEdges(std::pair<int, int> hmIndex);

	std::mutex chunksLock;
	unsigned int worldSeed = 67246;
	int chunkCount = 0;

	DirectX::XMFLOAT3 playerPos = { 0,0,0 };
	bool posChanged = true;
	float loadRadius = 25;
	float radiusSqrd = 0;
	std::map<std::pair<int,int>, Chunk> loadedChunks;

	std::map<std::pair<int, int>, HeightMap> loadedHeightmaps;

	std::queue<std::future<std::pair<int,int>>> futures;

	WorldData data;
};