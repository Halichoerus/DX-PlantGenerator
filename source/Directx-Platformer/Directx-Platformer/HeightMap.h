#pragma once
struct HeightMap
{
public:
	byte heightMap[HEIGHTMAP_SIZE];

	byte originalTop[HEIGHTMAP_BLOCK_WIDTH];
	byte originalRight[HEIGHTMAP_BLOCK_WIDTH];
	byte originalBottom[HEIGHTMAP_BLOCK_WIDTH];
	byte originalLeft[HEIGHTMAP_BLOCK_WIDTH];

	bool hasBeenJoined = false;

};