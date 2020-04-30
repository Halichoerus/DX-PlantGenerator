#include "Chunk.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Texture.h"
#include "Sampler.h"
#include "InputLayout.h"
#include "WorldConstantBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Vertex.h"
#include <random>
#include <math.h>

Chunk::~Chunk()
{
	deleteLock.lock();
	deleteLock.unlock();
	if(saveData.hasChanges)
		worldData->SaveChunkData(GetSaveData());
}

std::pair<int, int> Chunk::GenerateFromHeightmap(byte* heightMap, DirectX::XMFLOAT3 chunkPos, std::pair<int, int> id, WorldData* data)
{
	deleteLock.lock();
	worldData = data;
	saveData.id = id;

	pos = chunkPos;
	chunkID = id;
	float offsets[4];
	// Create the cubes
	int slice = CHUNK_WIDTH * CHUNK_HEIGHT;
	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		int z = i * slice;
		for (int j = 0; j < CHUNK_WIDTH; j++)
		{
			int xz = (j * CHUNK_HEIGHT) + z;

			offsets[0] = heightMap[((i + 1) * CHUNK_HM_WIDTH) + j];
			offsets[1] = heightMap[((i + 1) * CHUNK_HM_WIDTH) + j + 1];
			offsets[2] = heightMap[(i * CHUNK_HM_WIDTH) + j + 1];
			offsets[3] = heightMap[(i * CHUNK_HM_WIDTH) + j];

			int height = round((offsets[0] + offsets[1] + offsets[2] + offsets[3])/4);

			offsets[0] -= height;
			offsets[1] -= height;
			offsets[2] -= height;
			offsets[3] -= height;

			offsets[0] *= BLOCK_SIZE;
			offsets[1] *= BLOCK_SIZE;
			offsets[2] *= BLOCK_SIZE;
			offsets[3] *= BLOCK_SIZE;

			for (int k = CHUNK_HEIGHT; k > height; k--)
			{
				int index = xz + k;
				//blocks[index].SetPosition({ chunkPos.x + ((float)j * BLOCK_SIZE), chunkPos.y + ((float)k * BLOCK_SIZE), chunkPos.z + ((float)i * BLOCK_SIZE) });
				blocks[index].Load(Block::BlockType::air);
			}

			int k = height;
			blocks[xz + height].Load(Block::BlockType::grass, true, offsets);
			//blocks[xz + height].SetPosition({ chunkPos.x + ((float)j * BLOCK_SIZE), chunkPos.y + ((float)k * BLOCK_SIZE), chunkPos.z + ((float)i * BLOCK_SIZE) });

			k--;
			for (; k >= 0; k--)
			{
				int index = xz + k;
				blocks[index].Load(Block::BlockType::grass);
				//blocks[index].SetPosition({ chunkPos.x + ((float)j * BLOCK_SIZE), chunkPos.y + ((float)k * BLOCK_SIZE), chunkPos.z + ((float)i * BLOCK_SIZE) });

			}
		}
	}


	saveData = data->LoadChunkdata(id);
	
	for (BlockSaveData& bsd : saveData.blockChanges)
	{
		blocks[bsd.index].Load(bsd.blockType);
	}

	CalculateVisibleBlocks();

	transform.Position() = chunkPos;

	// Setup COM bits
	SetStaticBindID("Chunk");

	// If currently not static initialised
	if (!GetBindable<IndexBuffer>(DXBindable::BindableType::INDEX_BUFFER))
	{
		VertexShader* vs = new VertexShader(L"PhongVS.cso");
		std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Texcoord",0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		AddStaticBind(new InputLayout(ied, vs->GetByteBlob()));
		AddStaticBind(vs);

		AddStaticBind(new PixelShader(L"PhongPS.cso"));
		AddStaticBind(new Texture(L"Textures/Stems/Grass.jpg"));
		AddStaticBind(new Sampler());
	}

	AddBind(new VertexBuffer(mesh.vertices, D3D11_USAGE_DYNAMIC));
	AddBind(new IndexBuffer(mesh.indices, D3D11_USAGE_DYNAMIC));
	AddBind((new WorldConstantBuffer(transform)));

	indexCount = GetBindable<IndexBuffer>(DXBindable::BindableType::INDEX_BUFFER)->GetIndexCount();

	loaded = true;
	deleteLock.unlock();
	return id;

}

void Chunk::CalculateVisibleBlocks()
{
	mesh.vertices.clear();
	mesh.indices.clear();

	int slice = CHUNK_WIDTH * CHUNK_HEIGHT;
	int lastSlice = (slice * CHUNK_WIDTH) - slice;
	int lastCol = slice - CHUNK_HEIGHT;


	for (int i = 0; i < slice * CHUNK_WIDTH; i++)
	{
		if (blocks[i].type == Block::BlockType::air)
			continue;
		// -z
		if (i < slice)
		{
			//continue;
		}
		else
		{
			if (!blocks[i - slice].blockSight || blocks[i - slice].HasUniqueVerts())
			{
				if (!blocks[i].sidesVisible[2])
				{
					blocks[i].sidesVisible[2] = true;
					blocks[i].visibleSidesCount++;
				}
			}

		}
		// +z
		if (i >= lastSlice)
		{
			//continue;
		}
		else
		{
			if (!blocks[i + slice].blockSight || blocks[i + slice].HasUniqueVerts())
			{
				if (!blocks[i].sidesVisible[0])
				{
					blocks[i].sidesVisible[0] = true;
					blocks[i].visibleSidesCount++;
				}
			}
		}
		// -y
		int remain = i % CHUNK_HEIGHT;
		if (remain == 0)
		{
			//continue;
		}
		else
		{
			if (!blocks[i - 1].blockSight || blocks[i - 1].HasUniqueVerts())
			{
				if (!blocks[i].sidesVisible[5])
				{
					blocks[i].sidesVisible[5] = true;
					blocks[i].visibleSidesCount++;
				}
			}
		}
		// +y
		if (remain == CHUNK_HEIGHT - 1)
		{
			//continue;
		}
		else
		{
			if (!blocks[i + 1].blockSight)
			{
				if (!blocks[i].sidesVisible[4])
				{
					blocks[i].sidesVisible[4] = true;
					blocks[i].visibleSidesCount++;
				}
			}
		}

		remain = i % slice;
		//-x
		if (remain < CHUNK_HEIGHT)
		{
			//continue;
		}
		else
		{
			if (!blocks[i - CHUNK_HEIGHT].blockSight || blocks[i - CHUNK_HEIGHT].HasUniqueVerts())
			{
				if (!blocks[i].sidesVisible[3])
				{
					blocks[i].sidesVisible[3] = true;
					blocks[i].visibleSidesCount++;
				}
			}
		}
		//+x
		if (remain >= lastCol)
		{
			//continue;
		}
		else
		{
			//OutputDebugString((std::to_string(i)+ '\n').c_str());
			if (!blocks[i + CHUNK_HEIGHT].blockSight || blocks[i + CHUNK_HEIGHT].HasUniqueVerts())
			{
				if (!blocks[i].sidesVisible[1])
				{
					blocks[i].sidesVisible[1] = true;
					blocks[i].visibleSidesCount++;
				}
			}
		}

		if (blocks[i].visibleSidesCount > 0)
		{
			visibleBlocks.push_back(i);
			LoadIndexIntoMesh(i);
		}
	}
}

void Chunk::CalculateVisibleBlocksOnSides(Chunk* neighbours[4], bool callOnNeighbours)
{

	meshDirty = false;
	int slice = CHUNK_WIDTH * CHUNK_HEIGHT;
	int lastSlice = (slice * CHUNK_WIDTH) - slice;
	int lastCol = slice - CHUNK_HEIGHT;

	int northIndex = 0;
	int eastIndex = 0;
	int southIndex = 0;
	int westIndex = 0;

	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		for (int j = 0; j < CHUNK_HEIGHT; j++)
		{
			southIndex = (i * CHUNK_HEIGHT) + j;
			northIndex = ((i * CHUNK_HEIGHT) + j) + lastSlice;
			westIndex = (i * slice) + j;
			eastIndex = ((i * slice) + j) + lastCol;

			// NORTH
			// If the block in the neighbouring chunk does not block sight and this differs from this chunks block
			if (neighbours[0] && (!neighbours[0]->blocks[southIndex].blockSight || neighbours[0]->blocks[southIndex].HasUniqueVerts() || blocks[northIndex].HasUniqueVerts()) && blocks[northIndex].type != Block::BlockType::air && !blocks[northIndex].sidesVisible[0])
			{
				// Then set this side to visible
				blocks[northIndex].sidesVisible[0] = true;
				// If the current number of visible sides is 0, then add this block to the visible list
				if (blocks[northIndex].visibleSidesCount == 0)
				{
					visibleBlocks.push_back(northIndex);
					LoadIndexIntoMesh(northIndex);
				}

				blocks[northIndex].visibleSidesCount++;
			}

			// SOUTH
			// If the block in the neighbouring chunk does not block sight and this differs from this chunks block
			if (neighbours[2] && !(neighbours[2]->blocks[northIndex].blockSight || neighbours[2]->blocks[northIndex].HasUniqueVerts() || blocks[southIndex].HasUniqueVerts()) && blocks[southIndex].type != Block::BlockType::air && !blocks[southIndex].sidesVisible[2])
			{
				// Then set this side to visible
				blocks[southIndex].sidesVisible[2] = true;
				// If the current number of visible sides is 0, then add this block to the visible list
				if (blocks[southIndex].visibleSidesCount == 0)
				{
					visibleBlocks.push_back(southIndex);
					LoadIndexIntoMesh(southIndex);

				}

				blocks[southIndex].visibleSidesCount++;
			}

			// EAST
			// If the block in the neighbouring chunk does not block sight and this differs from this chunks block
			if (neighbours[1] && (!neighbours[1]->blocks[westIndex].blockSight || neighbours[1]->blocks[westIndex].HasUniqueVerts() || blocks[eastIndex].HasUniqueVerts()) && blocks[eastIndex].type != Block::BlockType::air && !blocks[eastIndex].sidesVisible[1])
			{
				// Then set this side to visible
				blocks[eastIndex].sidesVisible[1] = true;
				// If the current number of visible sides is 0, then add this block to the visible list
				if (blocks[eastIndex].visibleSidesCount == 0)
				{
					visibleBlocks.push_back(eastIndex);
					LoadIndexIntoMesh(eastIndex);
				}

				blocks[eastIndex].visibleSidesCount++;
			}

			// WEST
			// If the block in the neighbouring chunk does not block sight and this differs from this chunks block
			if (neighbours[3] && (!neighbours[3]->blocks[eastIndex].blockSight || neighbours[3]->blocks[eastIndex].HasUniqueVerts() || blocks[westIndex].HasUniqueVerts()) && blocks[westIndex].type != Block::BlockType::air && !blocks[westIndex].sidesVisible[3])
			{
				// Then set this side to visible
				blocks[westIndex].sidesVisible[3] = true;
				// If the current number of visible sides is 0, then add this block to the visible list
				if (blocks[westIndex].visibleSidesCount == 0)
				{
					visibleBlocks.push_back(westIndex);
					LoadIndexIntoMesh(westIndex);
				}

				blocks[westIndex].visibleSidesCount++;
			}
		}
	}

	if (callOnNeighbours)
	{
		Chunk* nb[4];
		nb[0] = nullptr;
		nb[1] = nullptr;
		nb[2] = this;
		nb[3] = nullptr;
		if (neighbours[0])
			neighbours[0]->CalculateVisibleBlocksOnSides(nb, false);
		nb[2] = nullptr;
		nb[3] = this;
		if (neighbours[1])
			neighbours[1]->CalculateVisibleBlocksOnSides(nb, false);
		nb[3] = nullptr;
		nb[0] = this;
		if (neighbours[2])
			neighbours[2]->CalculateVisibleBlocksOnSides(nb, false);
		nb[0] = nullptr;
		nb[1] = this;
		if (neighbours[3])
			neighbours[3]->CalculateVisibleBlocksOnSides(nb, false);
	}

	meshDirty = true;
}


void Chunk::Update()
{
	if (!loaded)
		return;

	//for (const int& index : visibleBlocks)
	//{
	//	blocks[index].Update();
	//}
}

void Chunk::Draw()
{
	if (!loaded)
		return;

	if (meshDirty)
	{
		GetBindable<VertexBuffer>(DXBindable::BindableType::VERTEX_BUFFER)->Update(mesh.vertices);
		GetBindable<IndexBuffer>(DXBindable::BindableType::INDEX_BUFFER)->Update(mesh.indices);
		indexCount = GetBindable<IndexBuffer>(DXBindable::BindableType::INDEX_BUFFER)->GetIndexCount();
		meshDirty = false;
	}

	GetBindable<WorldConstantBuffer>(DXBindable::BindableType::WORLD_CONSTANT_BUFFER)->Update(transform);
	DXDrawable::Draw();

	//blocks[0].Bind();
	//for (const int& index : visibleBlocks)
	//{
	//	blocks[index].Draw();
	//}

}

Block* Chunk::GetBlockAtLocation(DirectX::XMFLOAT3 location)
{
	location.x -= pos.x;
	location.y -= pos.y;
	location.z -= pos.z;

	location.x = floor(location.x / (float)BLOCK_SIZE);
	location.y = floor(location.y / (float)BLOCK_SIZE);
	location.z = floor(location.z / (float)BLOCK_SIZE);

	int index = location.y + (location.x * CHUNK_HEIGHT) + (location.z * CHUNK_HEIGHT * CHUNK_WIDTH);

	return &blocks[index];
}

void Chunk::SetBlockAtLocation(DirectX::XMFLOAT3 location, Block::BlockType type)
{
	location.x -= pos.x;
	location.y -= pos.y;
	location.z -= pos.z;

	location.x = abs(floor(location.x / (float)BLOCK_SIZE));
	location.y = abs(floor(location.y / (float)BLOCK_SIZE));
	location.z = abs(floor(location.z / (float)BLOCK_SIZE));

	int index = location.y + (location.x * CHUNK_HEIGHT) + (location.z * CHUNK_HEIGHT * CHUNK_WIDTH);
	blocks[index].Load(type);
	//blocks[index].SetPosition({ pos.x + (location.x * BLOCK_SIZE), pos.y + (location.y * BLOCK_SIZE), pos.z + (location.z * BLOCK_SIZE) });
	//OutputDebugString(("Block placed at index: " + std::to_string(index) + "\n").c_str());

	saveData.blockChanges.push_back(BlockSaveData(index, type));
	saveData.hasChanges = true;

	visibleBlocks.clear();
	CalculateVisibleBlocks();
}

Block* Chunk::GetBlockAtIndex(int index)
{
	return &blocks[index];
}
void Chunk::SetBlockAtIndex(int index, Block::BlockType type)
{
	blocks[index].Load(type);

	saveData.blockChanges.push_back(BlockSaveData(index, type));
	saveData.hasChanges = true;

	visibleBlocks.clear();
	CalculateVisibleBlocks();
	meshDirty = true;
}

ChunkSaveData* Chunk::GetSaveData()
{
	if (!saveData.hasChanges)
		return nullptr;
	// A copy of save data is created as this object may be deleted before the file system saves the data
	ChunkSaveData* returnData = new ChunkSaveData(saveData);

	if ((returnData->blockChanges.size() > 0) && (returnData->blockChanges[0].index > 100000 || returnData->blockChanges[0].index < 0))
	{
	}

	return returnData;

}

void Chunk::LoadIndexIntoMesh(int index)
{
	int slice = CHUNK_WIDTH * CHUNK_HEIGHT;
	using namespace DirectX;
	int zOffset = (index / slice);
	int xOffset = (index % slice) / CHUNK_HEIGHT;
	int yOffset = (index % slice) % CHUNK_HEIGHT;
	XMFLOAT3 offset = { (float)xOffset * BLOCK_SIZE, (float)yOffset * BLOCK_SIZE, (float)zOffset * BLOCK_SIZE };

	Mesh blockMesh = blocks[index].GetMesh();
	for (size_t j = 0; j < blockMesh.indices.size(); j++)
	{
		blockMesh.indices[j] += mesh.vertices.size();
	}
	for (size_t j = 0; j < blockMesh.vertices.size(); j++)
	{

		blockMesh.vertices[j].position.x += offset.x;
		blockMesh.vertices[j].position.y += offset.y;
		blockMesh.vertices[j].position.z += offset.z;

		mesh.vertices.push_back(blockMesh.vertices[j]);
	}

	for (unsigned short index : blockMesh.indices)
	{
		mesh.indices.push_back(index);
	}
}


bool Chunk::DoesLineIntersectBlock(const DirectX::XMFLOAT3 startPos, const DirectX::XMFLOAT3& endPos, int index, float& distOut)
{
	if (index < 0 || index >= CHUNK_SIZE || blocks[index].type == Block::BlockType::air)
		return false;

	distOut = FLT_MAX;
	using namespace DirectX;
	int slice = CHUNK_WIDTH * CHUNK_HEIGHT;
	int zOffset = (index / slice);
	int xOffset = (index % slice) / CHUNK_HEIGHT;
	int yOffset = (index % slice) % CHUNK_HEIGHT;
	XMFLOAT3 offset = { (float)xOffset * BLOCK_SIZE, (float)yOffset * BLOCK_SIZE, (float)zOffset * BLOCK_SIZE };
	XMFLOAT3 copy;

	auto& verts = blocks[index].GetMesh().vertices;
	XMVECTOR start = XMLoadFloat3(&startPos);
	XMVECTOR end = XMLoadFloat3(&endPos);

	XMVECTOR p1;
	XMVECTOR p2;
	XMVECTOR p3;
	XMVECTOR plane;
	XMVECTOR intersect;

	XMVECTOR u;
	XMVECTOR v;
	XMVECTOR n;
	XMVECTOR w;
	XMVECTOR betaV;
	XMVECTOR gammaV;
	float alpha;
	float beta;
	float gamma;
	XMFLOAT3 storeVec;
	for (int i = 0; i < verts.size(); i += 3)
	{
		copy.x = verts[i].position.x + offset.x;
		copy.y = verts[i].position.y + offset.y;
		copy.z = verts[i].position.z + offset.z;
		p1 = XMLoadFloat3(&copy);
		copy.x = verts[i+1].position.x + offset.x;
		copy.y = verts[i+1].position.y + offset.y;
		copy.z = verts[i+1].position.z + offset.z;
		p2 = XMLoadFloat3(&copy);
		copy.x = verts[i+2].position.x + offset.x;
		copy.y = verts[i+2].position.y + offset.y;
		copy.z = verts[i+2].position.z + offset.z;
		p3 = XMLoadFloat3(&copy);
		plane = XMPlaneFromPoints(p1, p2, p3);
		intersect = XMPlaneIntersectLine(plane, start, end);
		if (XMVector3IsNaN(intersect))
			continue;



		/* Information for this found at https://gamedev.stackexchange.com/questions/28781/easy-way-to-project-point-onto-triangle-or-plane/152476#152476
			referencing the work done by:
			Wolfgang Heidrich, 2005, Computing the Barycentric Coordinates of a Projected Point, Journal of Graphics Tools, pp 9-12, 10(3).*/
			// u=P2−P1

		p1 = XMVector3Length(intersect - start);
		XMStoreFloat3(&storeVec, p1);
		if(storeVec.x < distOut)
			distOut = storeVec.x;
		continue;

		u = p2 - p1;
		// v=P3−P1
		v = p3 - p1;
		// n=u×v
		n = XMVector3Cross(u,v);
		// w=P−P1
		w = intersect - p1;
		// Barycentric coordinates of the projection P′of P onto T:
		// γ=[(u×w)⋅n]/n²
		gammaV = XMVector3Dot(XMVector3Cross(u,w), n) / (n*n);
		// β=[(w×v)⋅n]/n²
		betaV = XMVector3Dot(XMVector3Cross(w,v),n) / (n*n);
		XMStoreFloat3(&storeVec, betaV);
		beta = storeVec.x;
		XMStoreFloat3(&storeVec, gammaV);
		gamma = storeVec.x;
		alpha = 1 - gamma - beta;
		// The point P′ lies inside T if:

		if ((0 <= alpha) && (alpha <= 1) &&
			(0 <= beta) && (beta <= 1) &&
			(0 <= gamma) && (gamma <= 1))
		{
			p1 = XMVector3Length(intersect - start);
			XMStoreFloat3(&storeVec, p1);
			distOut = storeVec.x;
			return true;
		}

	}
	return true;
}
