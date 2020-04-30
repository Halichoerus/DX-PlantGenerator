#include "Mesh.h"

void Mesh::PushTriangle(Vertex vert1, Vertex vert2, Vertex vert3)
{
	indices.push_back(vertices.size());
	vertices.push_back(vert1);
	indices.push_back(vertices.size());
	vertices.push_back(vert2);
	indices.push_back(vertices.size());
	vertices.push_back(vert3);
}

void Mesh::Clear()
{
	vertices.clear();
	indices.clear();
}

void Mesh::CreateCuboidMesh(float width, float height, float depth, bool useCornerOffsets, float offsets[4])
{
	width /= 2;
	height /= 2;
	depth /= 2;

	if (useCornerOffsets)
	{
		for (int i = 0; i < 4; i++)
		{
			if (offsets[i] < -height)
				offsets[i] = -height;
			else if (offsets[i] > height)
				offsets[i] = height;
		}
	}

	vertices = std::vector<Vertex>(24);
	vertices[0].position = { -width,-height,-depth };// 0 near side
	vertices[1].position = { width,-height,-depth };// 1
	vertices[2].position = { -width,height,-depth };// 2
	vertices[3].position = { width,height,-depth };// 3
	vertices[4].position = { -width,-height,depth };// 4 far side
	vertices[5].position = { width,-height,depth };// 5
	vertices[6].position = { -width,height,depth };// 6
	vertices[7].position = { width,height,depth };// 7
	vertices[8].position = { -width,-height,-depth };// 8 left side
	vertices[9].position = { -width,height,-depth };// 9
	vertices[10].position = { -width,-height,depth };// 10
	vertices[11].position = { -width,height,depth };// 11
	vertices[12].position = { width,-height,-depth };// 12 right side
	vertices[13].position = { width,height,-depth };// 13
	vertices[14].position = { width,-height,depth };// 14
	vertices[15].position = { width,height,depth };// 15
	vertices[16].position = { -width,-height,-depth };// 16 bottom side
	vertices[17].position = { width,-height,-depth };// 17
	vertices[18].position = { -width,-height,depth };// 18
	vertices[19].position = { width,-height,depth };// 19
	vertices[20].position = { -width,height,-depth };// 20 top side
	vertices[21].position = { width,height,-depth };// 21
	vertices[22].position = { -width,height,depth };// 22
	vertices[23].position = { width,height,depth };// 23

	if (useCornerOffsets)
	{
		vertices[2].position.y += offsets[3];  //{ -width,height,-depth };// 2
		vertices[3].position.y += offsets[2];  //{ width,height,-depth };// 3
		vertices[6].position.y += offsets[0];  //{ -width,height,depth };// 6
		vertices[7].position.y += offsets[1];  //{ width,height,depth };// 7
		vertices[9].position.y += offsets[3];  //{ -width,height,-depth };// 9
		vertices[11].position.y += offsets[0]; //{ -width,height,depth };// 11
		vertices[13].position.y += offsets[2]; //{ width,height,-depth };// 13
		vertices[15].position.y += offsets[1]; //{ width,height,depth };// 15
		vertices[20].position.y += offsets[3]; //{ -width,height,-depth };// 20 top side
		vertices[21].position.y += offsets[2]; //{ width,height,-depth };// 21
		vertices[22].position.y += offsets[0]; //{ -width,height,depth };// 22
		vertices[23].position.y += offsets[1]; //{ width,height,depth };// 23
	}

	indices = std::vector<unsigned short>
	{
			0, 2, 1, 2, 3, 1,
			4, 5, 7, 4, 7, 6,
			8, 10, 9, 10, 11, 9,
			12, 13, 15, 12, 15, 14,
			16, 17, 18, 18, 17, 19,
			20, 23, 21, 20, 22, 23
	};

	vertices[0].tex = { 0.0f,0.0f };
	vertices[1].tex = { 1.0f,0.0f };
	vertices[2].tex = { 0.0f,1.0f };
	vertices[3].tex = { 1.0f,1.0f };
	vertices[4].tex = { 0.0f,0.0f };
	vertices[5].tex = { 1.0f,0.0f };
	vertices[6].tex = { 0.0f,1.0f };
	vertices[7].tex = { 1.0f,1.0f };
	vertices[8].tex = { 0.0f,0.0f };
	vertices[9].tex = { 1.0f,0.0f };
	vertices[10].tex = { 0.0f,1.0f };
	vertices[11].tex = { 1.0f,1.0f };
	vertices[12].tex = { 0.0f,0.0f };
	vertices[13].tex = { 1.0f,0.0f };
	vertices[14].tex = { 0.0f,1.0f };
	vertices[15].tex = { 1.0f,1.0f };
	vertices[16].tex = { 0.0f,0.0f };
	vertices[17].tex = { 1.0f,0.0f };
	vertices[18].tex = { 0.0f,1.0f };
	vertices[19].tex = { 1.0f,1.0f };
	vertices[20].tex = { 0.0f,0.0f };
	vertices[21].tex = { 1.0f,0.0f };
	vertices[22].tex = { 0.0f,1.0f };
	vertices[23].tex = { 1.0f,1.0f };
}

void Mesh::GenerateNormals()
{
	// asserts face-independent vertices w/ normals cleared to zero
	using namespace DirectX;
	assert(indices.size() % 3 == 0 && indices.size() > 0);
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		auto& v0 = vertices[indices[i]];
		auto& v1 = vertices[indices[i + 1]];
		auto& v2 = vertices[indices[i + 2]];
		const auto p0 = XMLoadFloat3(&v0.position);
		const auto p1 = XMLoadFloat3(&v1.position);
		const auto p2 = XMLoadFloat3(&v2.position);

		const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

		XMStoreFloat3(&v0.normal, n);
		XMStoreFloat3(&v1.normal, n);
		XMStoreFloat3(&v2.normal, n);

	}
}
