#pragma once
#include <string>
#include <queue>
#include <atomic>
#include <mutex>
#include <thread>
#include "ChunkSaveData.h"

// The .world file format is as following:
// unsigned int - world seed
// unsigned int - number of saved chunks
//		int, int - chunk co-ordinates												}
//		unsigned int - number of changes											}
//				unsigned int - block index }										} Repeat for number of saved chunks
//				int - block type           } repeat for number of blocks in chunk	}

class WorldData
{
public:
	WorldData() = default;
	~WorldData();

	unsigned int LoadWorld(std::string worldName);
	void NewWorld(std::string worldName, unsigned int worldSeed);

	void SaveChunkData(ChunkSaveData* chunk);
	ChunkSaveData LoadChunkdata(std::pair<int,int> chunkID);

private:
	void SaveChunksInQueue();
	unsigned int GetReadPosOfChunk(std::pair<int, int> chunkID, unsigned int& chunkCount);

	void WriteData(std::ofstream& file, char* buffer, const void* source, unsigned int& readPos, const int& size);
	char* ReadData(std::ifstream& file, char* buffer, unsigned int& readPos, const int& size);

	std::queue<ChunkSaveData*> chunksToSave;
	std::atomic_bool writeThreadActive = false;
	std::mutex queueLock;
	std::mutex deleteLock;
	std::mutex fileLock;

	std::string filepath;
	unsigned int seed = 0;

};

