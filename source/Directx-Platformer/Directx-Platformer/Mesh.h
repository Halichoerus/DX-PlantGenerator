#pragma once
#include <vector>
#include <string>
#include "Vertex.h"

class Mesh
{
public:
	Mesh() = default;

	void PushTriangle(Vertex vert1, Vertex vert2, Vertex vert3);
	void Clear();

	void CreateCuboidMesh(float width, float height, float depth, bool useCornerOffsets = false, float offsets[4] = {});

	void GenerateNormals();

	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
private:

};

