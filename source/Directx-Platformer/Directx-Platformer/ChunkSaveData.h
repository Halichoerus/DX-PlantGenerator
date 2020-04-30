#pragma once
#include "Block.h"
#include <vector>

// Holds all the changes that have been made for a block
struct BlockSaveData
{
	BlockSaveData()
	{
		index = 0;
		blockType = Block::BlockType::air;
	}

	BlockSaveData(int _index, Block::BlockType _blockType)
	{
		index = _index;
		blockType = _blockType;
	}

	int index = 0;
	Block::BlockType blockType = Block::BlockType::air;
};

// Holds all the changes that have been made in a chunk
struct ChunkSaveData
{
	ChunkSaveData() = default;
	ChunkSaveData(const ChunkSaveData& csd)
	{
		id = csd.id;
		hasChanges = csd.hasChanges;
		blockChanges.resize(csd.blockChanges.size());
		memcpy(blockChanges.data(), csd.blockChanges.data(), csd.blockChanges.size() * sizeof(BlockSaveData));
	}

	std::pair<int, int> id = { 0,0 };
	std::vector<BlockSaveData> blockChanges;
	bool hasChanges = false; // Indicates if there is anything to save, should be set to true if ANY data is added to this struct
};