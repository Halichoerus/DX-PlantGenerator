#include <vector>
#include <iostream>
#include <fstream>
#include "WorldData.h"
#include "Constants.h"

WorldData::~WorldData()
{
	deleteLock.lock();
	deleteLock.unlock();
}

unsigned int WorldData::LoadWorld(std::string worldName)
{
	filepath = "Maps/" + worldName + ".world";

	std::ifstream file(filepath, std::ios::binary | std::ios::out | std::ios::in);
	std::vector<char> data;

	if (file.is_open())
	{
		data.resize(sizeof(unsigned int));
		file.read(data.data(), sizeof(unsigned int));
		seed = *reinterpret_cast<unsigned int*>(data.data());
	}
	else
	{
		NewWorld(worldName, 123456);
	}

	file.close();

	return seed;
}

// Creates a new binary .map file in the Maps folder
void WorldData::NewWorld(std::string worldName, unsigned int worldSeed)
{
	filepath = "Maps/" + worldName + ".world";
	seed = worldSeed;
	std::vector<char> data;
	data.resize(sizeof(unsigned int));
	unsigned int size = sizeof(unsigned int);
	unsigned int readPos = 0;

	std::ofstream file(filepath, std::ios::out | std::ios::binary);

	if (file.is_open())
	{
		WriteData(file, data.data(), &worldSeed, readPos, size);
		unsigned int chunkCount = 0;
		WriteData(file, data.data(), &chunkCount, readPos, size);
	}
	file.close();
}

void WorldData::SaveChunkData(ChunkSaveData* chunk)
{
	queueLock.lock();
	chunksToSave.push(chunk);
	queueLock.unlock();

	//SaveChunksInQueue();
	//return;

	if (!writeThreadActive)
	{
		std::thread tr(&WorldData::SaveChunksInQueue, this);
		tr.detach();
	}
}

ChunkSaveData WorldData::LoadChunkdata(std::pair<int, int> chunkID)
{
	fileLock.lock();
	int size = sizeof(unsigned int);
	std::vector<char> data;
	data.resize(size);
	unsigned int chunkCount = 0;
	unsigned int readPos = GetReadPosOfChunk(chunkID, chunkCount);
	readPos += size * 2;
	ChunkSaveData returnData;


	std::ifstream infile(filepath, std::ios::binary | std::ios::out | std::ios::in);

	unsigned int changes = *reinterpret_cast<unsigned int*>(ReadData(infile, data.data(), readPos, size));
	if (changes > 0)
		returnData.hasChanges = true;

	for (unsigned int i = 0; i < changes; i++)
	{
		returnData.blockChanges.emplace_back();
		returnData.blockChanges.back().index = *reinterpret_cast<unsigned int*>(ReadData(infile, data.data(), readPos, size));
		returnData.blockChanges.back().blockType = *reinterpret_cast<Block::BlockType*>(ReadData(infile, data.data(), readPos, size));
		if (returnData.blockChanges.back().index > 100000 || returnData.blockChanges.back().index < 0)
		{
			int itsfucked = 0;
			itsfucked++;
		}
	}

	infile.close();
	fileLock.unlock();
	returnData.id = chunkID;
	return returnData;
}

void WorldData::SaveChunksInQueue()
{
	deleteLock.lock();
	fileLock.lock();
	ChunkSaveData* nextChunk = nullptr;
	std::vector<char> data;
	int savedChunkCount = 0;
	int size = sizeof(unsigned int);
	data.resize(size);
	unsigned int readPos = size;

	queueLock.lock();
	bool chunksInQueue = !chunksToSave.empty();
	queueLock.unlock();

	while (chunksInQueue)
	{
		readPos = size;
		// Get next chunk to write
		queueLock.lock();
		nextChunk = chunksToSave.front();
		chunksToSave.pop();
		queueLock.unlock();

		// Find where to write the data
		unsigned int chunkCount = 0;
		readPos = GetReadPosOfChunk(nextChunk->id, chunkCount);

		// Write the data
		std::ofstream outfile(filepath, std::ios::binary | std::ios::out | std::ios::in);

		// If a new chunk, update the chunk count
		unsigned int chunkCountPos = size;
		WriteData(outfile, data.data(), &chunkCount, chunkCountPos, size);


		if (outfile.is_open())
		{
			// Write the chunk ID
			WriteData(outfile, data.data(), &nextChunk->id.first, readPos, size);
			WriteData(outfile, data.data(), &nextChunk->id.second, readPos, size);
			unsigned int changesCount = (unsigned int)nextChunk->blockChanges.size();
			WriteData(outfile, data.data(), &changesCount, readPos, size);

			// Write each block change
			for (BlockSaveData& bsd : nextChunk->blockChanges)
			{
				if (bsd.index > 100000 || bsd.index < 0)
				{
					int isgigafuckedmate = 0;
					isgigafuckedmate++;
				}
				//Write the block index
				WriteData(outfile, data.data(), &bsd.index, readPos, size);

				//Write the block type
				WriteData(outfile, data.data(), &bsd.blockType, readPos, size);
			}
			// Fill the rest out with empty data
			int empty = 1;
			for (int i = 0; i < CHUNK_SIZE - nextChunk->blockChanges.size(); i++)
			{
				WriteData(outfile, data.data(), &empty, readPos, size);
				WriteData(outfile, data.data(), &empty, readPos, size);
			}
		}
		else continue;

		queueLock.lock();
		chunksInQueue = !chunksToSave.empty();
		if(!chunksInQueue)
			writeThreadActive = false;
		//delete nextChunk;
		queueLock.unlock();

		outfile.close();
	}

	writeThreadActive = false;
	deleteLock.unlock();
	fileLock.unlock();
}

unsigned int WorldData::GetReadPosOfChunk(std::pair<int, int> chunkID, unsigned int& chunkCount)
{
	std::vector<char> data;
	int size = sizeof(unsigned int);
	data.resize(size);
	unsigned int readPos = size;
	std::pair<int, int> id;
	std::ifstream infile(filepath, std::ios::binary | std::ios::out | std::ios::in);

	if (infile.is_open())
	{
		// Read the number of chunks that are saved
		unsigned int savedChunkCount = *reinterpret_cast<unsigned int*>(ReadData(infile, data.data(), readPos, size));
		chunkCount = savedChunkCount;

		while (savedChunkCount > 0)
		{
			// Read the chunk id
			id.first = *reinterpret_cast<int*>(ReadData(infile, data.data(), readPos, size));
			id.second = *reinterpret_cast<int*>(ReadData(infile, data.data(), readPos, size));

			if (chunkID == id)
			{
				// Move the read position back to the start of the chunk data
				readPos -= size * 2;
				break;
			}

			// Skip read pos to the end of the chunk data
			readPos += (size * 2 * CHUNK_SIZE) + size;
			//readPos += size;

			savedChunkCount--;
		} 
		if (savedChunkCount == 0)
			chunkCount++;
	}
	infile.close();
	return readPos;
}

// Writes the data 'source' of size 'size' to filesteam 'file' at position 'readPos', using a buffer 'buffer' of size 'size'
void WorldData::WriteData(std::ofstream& file, char* buffer, const void* source, unsigned int& readPos, const int& size)
{
	if ((readPos == 8)|| (readPos == 12) || (size != 4))
	{
		int FUCKFGUCKFDJKLFSDJKO = 1;
		FUCKFGUCKFDJKLFSDJKO++;
	}
	file.seekp(readPos);
	memcpy(buffer, source, size);
	file.write(buffer, size);
	readPos += size;
}

// Reads data of size 'size' in from filesteam 'file' at position 'readPos' into the buffer 'buffer' and returns buffer
char* WorldData::ReadData(std::ifstream& file, char* buffer, unsigned int& readPos, const int& size)
{
	file.seekg(readPos);
	file.read(buffer, size);
	readPos += size;
	return buffer;
}