#include "World.h"
#include <algorithm>
#include <chrono>
#include <random>
#include <math.h>

void World::UpdatePlayerPos(DirectX::XMFLOAT3 pos)
{
	playerPos = pos;
	posChanged = true;
}

void World::Load()
{
	radiusSqrd = loadRadius * loadRadius;
	data.LoadWorld("TestWorld1");
	CalculteVisibleChunks();
	std::vector<int> test;
}

void World::SetLoadRadius(float radius)
{
	loadRadius = radius;
	radiusSqrd = radius * radius;
}

void World::Update()
{
	// Update which chunks should be loaded and rendered
	CalculteVisibleChunks();
}

void World::Draw()
{
	for (auto& pair : loadedChunks)
	{
		pair.second.Draw();
	}
}

void World::CalculteVisibleChunks()
{
	// Create a vector of indices for all chunks that should be loaded
	float chunkWidth = CHUNK_WIDTH * BLOCK_SIZE;
	int maxZChunk = ceil((playerPos.z + loadRadius) / chunkWidth);
	int minZChunk = ceil((playerPos.z - loadRadius) / chunkWidth);
	int maxXChunk = ceil((playerPos.x + loadRadius) / chunkWidth);
	int minXChunk = ceil((playerPos.x - loadRadius) / chunkWidth);

	std::vector<std::pair<int, int>> chunksToLoad;
	float xPos = playerPos.x - loadRadius;
	float zPos = playerPos.z - loadRadius;
	for (int x = minXChunk; x <= maxXChunk; x++)
	{
		xPos += chunkWidth;
		zPos = playerPos.z - loadRadius;
		for (int z = minZChunk; z <= maxZChunk; z++)
		{
			zPos += chunkWidth;
			auto pair = std::pair<int, int>(x, z);
			if (ShouldLoadChunk(xPos, zPos))
			{
				if (loadedChunks.count(pair) == 0)
				{
					chunksToLoad.push_back(std::pair<int, int>(pair));
				}
				else
				{
					loadedChunks[pair].SetUnloadFlag(false);
				}
			}
		}
	}

	std::vector <std::pair<int, int>> removeList;

	// Remove any chunks from the list that are already loaded, and also flag any loaded chunks that should be unloaded
	for (auto& pair : loadedChunks)
	{
		auto it = std::find(chunksToLoad.begin(), chunksToLoad.end(), pair.first);
		if (it == chunksToLoad.end())
		{
			// Skip chunks that havn't been loaded yet to avoid thread problems
			if (pair.second.IsLoaded() && pair.second.ShouldUnload())
			{
				removeList.push_back(pair.first);
			}
			pair.second.SetUnloadFlag(true);
		}
		else
		{
			pair.second.SetUnloadFlag(true);
		}
	}

	for (auto& key : removeList)
	{
		loadedChunks.erase(key);
	}

	// Load in the new chunks
	for (auto pair : chunksToLoad)
	{
		//loadedChunks[pair].GenerateFromHeightmap(GetHeightmap(pair), DirectX::XMFLOAT3(pair.first * CHUNK_WIDTH * BLOCK_SIZE, 0, pair.second * CHUNK_WIDTH * BLOCK_SIZE), pair, &data);
		futures.push(std::async(std::launch::async, &Chunk::GenerateFromHeightmap, &loadedChunks[pair], GetHeightmap(pair), DirectX::XMFLOAT3(pair.first * CHUNK_WIDTH * BLOCK_SIZE, 0, pair.second * CHUNK_WIDTH * BLOCK_SIZE), pair, &data));
	}

	while (!futures.empty() && futures.front()._Is_ready())
	{
		Chunk* nb[4];
		std::pair<int, int> chunk = futures.front().get();
		chunk.second++;
		nb[0] = GetChunkIfExsists(chunk);
		chunk.second -= 2;
		nb[2] = GetChunkIfExsists(chunk);
		chunk.second++;
		chunk.first++;
		nb[1] = GetChunkIfExsists(chunk);
		chunk.first -= 2;
		nb[3] = GetChunkIfExsists(chunk);
		chunk.first++;

		loadedChunks[chunk].CalculateVisibleBlocksOnSides(nb, true);
		futures.pop();
	}
}

Chunk* World::GetChunkIfExsists(std::pair<int, int> chunk)
{
	if (loadedChunks.count(chunk) > 0 && loadedChunks[chunk].IsLoaded())
		return &loadedChunks[chunk];
	else
		return nullptr;
}

bool World::ShouldLoadChunk(float xPos, float zPos)
{
	float distSqr = powf(xPos - playerPos.x, 2) + powf(zPos - playerPos.z, 2);
	if (distSqr <= radiusSqrd)
		return true;
	else
		return false;
}

byte* World::GetHeightmap(std::pair<int, int> chunk)
{
	byte* hm = new byte[CHUNK_HM_SIZE];

	std::pair<int, int> hm_index = { floor(chunk.first / (float)HEIGHTMAP_CHUNK_WIDTH),floor(chunk.second / (float)HEIGHTMAP_CHUNK_WIDTH) };

	if (loadedHeightmaps.count(hm_index) == 0)
		GenerateHeightmap(hm_index);

	if (!loadedHeightmaps[hm_index].hasBeenJoined)
		JoinHeightmapEdges(hm_index);

	auto& heightMap = loadedHeightmaps[hm_index].heightMap;


	int xOffset = 0;
	int yOffset = 0;
	if (chunk.first < 0)
		xOffset = (HEIGHTMAP_CHUNK_WIDTH-1) - (abs(chunk.first+1) % HEIGHTMAP_CHUNK_WIDTH);
	else
		xOffset = chunk.first % HEIGHTMAP_CHUNK_WIDTH;
	if (chunk.second < 0)
		yOffset = (HEIGHTMAP_CHUNK_WIDTH-1) - (abs(chunk.second+1) % HEIGHTMAP_CHUNK_WIDTH);
	else
		yOffset = chunk.second % HEIGHTMAP_CHUNK_WIDTH;


	//if (chunk.first < 0)
	//{
	//	if (xOffset == 0)
	//		xOffset = 8;
	//	xOffset = (HEIGHTMAP_CHUNK_WIDTH)-xOffset;
	//}
	//if (chunk.second < 0)
	//{
	//	if (yOffset == 0)
	//		yOffset = 8;
	//	yOffset = (HEIGHTMAP_CHUNK_WIDTH)-yOffset;
	//}


	int startIndex = (xOffset * CHUNK_WIDTH) + (yOffset * HEIGHTMAP_BLOCK_WIDTH * CHUNK_WIDTH);


	for (int i = 0; i < CHUNK_HM_WIDTH; i++)
	{
		int offset = startIndex + (i * HEIGHTMAP_BLOCK_WIDTH);
		int index = i * CHUNK_HM_WIDTH;
		for (int j = 0; j < CHUNK_HM_WIDTH; j++)
		{
			hm[index + j] = heightMap[offset + j];
		}
	}

	OutputDebugString("\n");

	//for (int i = 0; i < CHUNK_HM_WIDTH; i++)
	//{
	//	std::string line = "";
	//	for (int j = 0; j < CHUNK_HM_WIDTH; j++)
	//	{
	//		line += std::to_string(hm[j + (i * (CHUNK_WIDTH + 1))]);
	//		line += '/';
	//	}
	//	line += '\n';
	//	OutputDebugString(line.c_str());
	//}

	return hm;

}

void World::GenerateHeightmap(std::pair<int, int> location, bool keepEdges)
{
	float k1 = static_cast<float>(location.first);
	float k2 = static_cast<float>(location.second);
	float id = (0.5f * (k1 + k2) * (k1 + k2 + 1.f)) + k2;

	auto& heightMap = loadedHeightmaps[location].heightMap;
	int maxDiff = 30;

	std::default_random_engine generator;
	generator.seed(worldSeed+id);
	std::uniform_int_distribution<int> distribution(-maxDiff, maxDiff);

	if (!keepEdges)
	{
		heightMap[0] = 50 + distribution(generator);
		heightMap[HEIGHTMAP_BLOCK_WIDTH - 1] = 50 + distribution(generator);
		heightMap[(HEIGHTMAP_BLOCK_WIDTH * HEIGHTMAP_BLOCK_WIDTH) - HEIGHTMAP_BLOCK_WIDTH] = 50 + distribution(generator);
		heightMap[(HEIGHTMAP_BLOCK_WIDTH * HEIGHTMAP_BLOCK_WIDTH) - 1] = 50 + distribution(generator);
	}

	// Using the diamond-square algorithm

	int subSquareWidth = HEIGHTMAP_BLOCK_WIDTH - 1;
	int subSquareHeight = HEIGHTMAP_SIZE - HEIGHTMAP_BLOCK_WIDTH;
	int halfWidth = subSquareWidth / 2;
	int halfHeight = halfWidth * HEIGHTMAP_BLOCK_WIDTH;

	while (halfWidth != 0)
	{
		// Square steps
		for (int i = 0; i < (HEIGHTMAP_BLOCK_WIDTH - 1) / subSquareWidth; i++)
		{
			for (int j = 0; j < (HEIGHTMAP_BLOCK_WIDTH - 1) / subSquareWidth; j++)
			{
				int startIndex = (subSquareWidth * i) + (subSquareHeight * j);
				int index = startIndex + halfWidth + halfHeight;

				if (keepEdges && heightMap[index] != 0)
					continue;

				int total = heightMap[startIndex];
				total += heightMap[startIndex + subSquareWidth];
				total += heightMap[startIndex + subSquareHeight];
				total += heightMap[startIndex + subSquareWidth + subSquareHeight];

				heightMap[index] = (total / 4) + distribution(generator);
				if (heightMap[index] < MIN_TERRAIN_GEN_HEIGHT)
					heightMap[index] = MIN_TERRAIN_GEN_HEIGHT;
				else if (heightMap[index] > MAX_TERRAIN_GEN_HEIGHT)
					heightMap[index] = MAX_TERRAIN_GEN_HEIGHT;
			}
		}

		// Triangle steps
		for (int i = 0; i < ((HEIGHTMAP_BLOCK_WIDTH - 1) / halfWidth) + 1; i++) // 3/4
		{
			int xOdd = i % 2; // 1
			int yOdd = 1 - xOdd; // 0
			for (int j = 0; j < ((HEIGHTMAP_BLOCK_WIDTH - 1) / subSquareWidth) + xOdd; j++) // 0/2
			{
				int midIndex = (halfWidth * i) + ((subSquareHeight * j) + (halfHeight * yOdd));

				if (keepEdges && heightMap[midIndex] != 0)
					continue;

				int total = 0;
				int count = 0;
				//int index = midIndex - halfWidth;
				int rowRemain = midIndex % HEIGHTMAP_BLOCK_WIDTH;

				if (halfWidth <= rowRemain)
				{
					total += heightMap[midIndex - halfWidth];
					count++;
				}
				//index = midIndex + halfWidth;
				if (halfWidth <= HEIGHTMAP_BLOCK_WIDTH - rowRemain)
				{
					total += heightMap[midIndex + halfWidth];
					count++;
				}
				int index = midIndex + halfHeight;
				if (index < HEIGHTMAP_SIZE)
				{
					total += heightMap[index];
					count++;
				}
				index = midIndex - halfHeight;
				if (index >= 0)
				{
					total += heightMap[index];
					count++;
				}

				heightMap[midIndex] = (total / count) + distribution(generator); 
				if (heightMap[midIndex] < MIN_TERRAIN_GEN_HEIGHT)
					heightMap[midIndex] = MIN_TERRAIN_GEN_HEIGHT;
				else if (heightMap[midIndex] > MAX_TERRAIN_GEN_HEIGHT)
					heightMap[midIndex] = MAX_TERRAIN_GEN_HEIGHT;
			}
		}

		subSquareWidth /= 2;
		subSquareHeight /= 2;
		halfWidth /= 2;
		halfHeight /= 2;
		maxDiff /= 2;
		distribution = std::uniform_int_distribution<int>(-maxDiff, maxDiff);

	}

	if (keepEdges)
		return;

	int topRowStart = (HEIGHTMAP_BLOCK_WIDTH * HEIGHTMAP_BLOCK_WIDTH) - (HEIGHTMAP_BLOCK_WIDTH);

	// Record the edges for use in HM joining
	for (int i = 0; i < HEIGHTMAP_BLOCK_WIDTH; i++)
	{
		loadedHeightmaps[location].originalBottom[i] = heightMap[i];
		loadedHeightmaps[location].originalTop[i] = heightMap[topRowStart+i];
		loadedHeightmaps[location].originalLeft[i] = heightMap[HEIGHTMAP_BLOCK_WIDTH * i];
		loadedHeightmaps[location].originalRight[i] = heightMap[(HEIGHTMAP_BLOCK_WIDTH * i) + (HEIGHTMAP_BLOCK_WIDTH-1)];
	}
}

void World::JoinHeightmapEdges(std::pair<int, int> hmIndex)
{
	std::pair<int, int> nbIndex = hmIndex;
	auto& heightMap = loadedHeightmaps[hmIndex].heightMap;

	// NORTH
	nbIndex.second++;
	if (loadedHeightmaps.count(nbIndex) == 0)
		GenerateHeightmap(nbIndex);
	auto& northHm = loadedHeightmaps[nbIndex];

	// EAST
	nbIndex.second--;
	nbIndex.first++;
	if (loadedHeightmaps.count(nbIndex) == 0)
		GenerateHeightmap(nbIndex);
	auto& eastHm = loadedHeightmaps[nbIndex];

	int topRowStart = (HEIGHTMAP_BLOCK_WIDTH * HEIGHTMAP_BLOCK_WIDTH) - (HEIGHTMAP_BLOCK_WIDTH);
	for (int i = 0; i < HEIGHTMAP_BLOCK_WIDTH; i++)
	{
		heightMap[topRowStart + i] = northHm.originalBottom[i]; // Top
		heightMap[(HEIGHTMAP_BLOCK_WIDTH * i) + (HEIGHTMAP_BLOCK_WIDTH - 1)] = eastHm.originalLeft[i]; // Right
	}

	for (int i = 1; i < HEIGHTMAP_BLOCK_WIDTH-1; i++)
	{
		int col = i * HEIGHTMAP_BLOCK_WIDTH;
		for (int j = 1; j < HEIGHTMAP_BLOCK_WIDTH-1 ; j++)
		{
			heightMap[col + j] = 0;
		}
	}
	
	heightMap[HEIGHTMAP_SIZE-1] = (northHm.originalBottom[HEIGHTMAP_BLOCK_WIDTH-1] + eastHm.originalLeft[HEIGHTMAP_BLOCK_WIDTH - 1]) / 2;

	GenerateHeightmap(hmIndex, true);
	loadedHeightmaps[hmIndex].hasBeenJoined = true;

	return;
}

Block* World::GetBlockAtLocation(DirectX::XMFLOAT3 location, Chunk* chunk = nullptr)
{
	float chunkWidth = CHUNK_WIDTH * BLOCK_SIZE;
	std::pair<int, int> id = { ceil(playerPos.x / chunkWidth), ceil(playerPos.z / chunkWidth) };

	if (loadedChunks.count(id) > 0 && loadedChunks[id].IsLoaded())
	{
		chunk = &loadedChunks[id];
		return loadedChunks[id].GetBlockAtLocation(location);
	}
	else
		return nullptr;
}

void World::SetBlockAtLocation(DirectX::XMFLOAT3 location, Block::BlockType type)
{
	float chunkWidth = CHUNK_WIDTH * BLOCK_SIZE;
	std::pair<int, int> id = { floor(location.x / chunkWidth), floor(location.z / chunkWidth) };

	if (loadedChunks.count(id) > 0 && loadedChunks[id].IsLoaded())
	{
		loadedChunks[id].SetBlockAtLocation(location, type);
		Chunk* nb[4];

		id.second++;
		nb[0] = GetChunkIfExsists(id);
		id.second -= 2;
		nb[2] = GetChunkIfExsists(id);
		id.second++;
		id.first++;
		nb[1] = GetChunkIfExsists(id);
		id.first -= 2;
		nb[3] = GetChunkIfExsists(id);
		id.first++;

		loadedChunks[id].CalculateVisibleBlocksOnSides(nb, true);
	}
}
int World::GetIndexHitByRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, std::pair<int, int>& outChunkIndex)
{
	auto start = rayStart;
	auto end = rayEnd;

	start.x = floor(start.x / BLOCK_SIZE);
	start.y = floor(start.y / BLOCK_SIZE);
	start.z = floor(start.z / BLOCK_SIZE);
	end.x = floor(end.x / BLOCK_SIZE);
	end.y = floor(end.y / BLOCK_SIZE);
	end.z = floor(end.z / BLOCK_SIZE);

	if (start.x > end.x)
	{
		float temp = start.x;
		start.x = end.x;
		end.x = temp;
	}
	if (start.y > end.y)
	{
		float temp = start.y;
		start.y = end.y;
		end.y = temp;
	}
	if (start.z > end.z)
	{
		float temp = start.z;
		start.z = end.z;
		end.z = temp;
	}

	OutputDebugString(("Lower Bound: " + std::to_string(start.x) + ", " + std::to_string(start.y) + ", " + std::to_string(start.z) + "\n").c_str());
	OutputDebugString(("Upper Bound: " + std::to_string(end.x) + ", " + std::to_string(end.y) + ", " + std::to_string(end.z) + "\n").c_str());

	int slice = CHUNK_HEIGHT * CHUNK_WIDTH;
	float distOut = 0;
	float bestDist = FLT_MAX;
	int bestIndex = -1;
	for (int z = start.z; z <= end.z; z++)
	{

		int sliceOffset;
		if (z < 0)
			sliceOffset = ((CHUNK_WIDTH-1) - ((abs(z)-1) % CHUNK_WIDTH))* slice;
		else
			sliceOffset = (z % CHUNK_WIDTH) * slice;

		for (int x = start.x; x <= end.x; x++)
		{
			int colOffset;
			if (x < 0)
				colOffset = ((CHUNK_WIDTH-1) - ((abs(x)-1) % CHUNK_WIDTH)) * CHUNK_HEIGHT;
			else
				colOffset = (x % CHUNK_WIDTH) * CHUNK_HEIGHT;
			for (int y = start.y; y <= end.y; y++)
			{
				std::pair<int, int> id = { floor(x / CHUNK_WIDTH), floor(z / CHUNK_WIDTH) };

				if (loadedChunks.count(id) > 0 && loadedChunks[id].IsLoaded())
				{
					int index = sliceOffset + colOffset + y;
					if (loadedChunks[id].DoesLineIntersectBlock(rayStart, rayEnd, index, distOut))
					{
						if (distOut < bestDist)
						{
							bestDist = distOut;
							bestIndex = index;
							outChunkIndex = id;
						}
					}
				}
			}
		}
	}
	return bestIndex;
}

Block* World::GetBlockAtIndex(std::pair<int, int> chunkIndex, int blockIndex)
{
	if (loadedChunks.count(chunkIndex) > 0)
	{
		return loadedChunks[chunkIndex].GetBlockAtIndex(blockIndex);
	}
	return nullptr;
}
void World:: SetBlockAtIndex(std::pair<int, int> chunkIndex, int blockIndex, Block::BlockType type)
{
	if (loadedChunks.count(chunkIndex) > 0)
	{
		loadedChunks[chunkIndex].SetBlockAtIndex(blockIndex, type);
	}
}