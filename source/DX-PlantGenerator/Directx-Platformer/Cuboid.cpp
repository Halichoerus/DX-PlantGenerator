#include "Cuboid.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Texture.h"
#include "Sampler.h"
#include "InputLayout.h"
#include "WorldConstantBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Vertex.h"
#include "Mesh.h"


void Cuboid::Load(const wchar_t* path, float width, float height, float depth, bool useCornerOffsets, float offsets[4] )
{
	SetStaticBindID("Cuboid");
	uniqueIndex = useCornerOffsets;


	if (useCornerOffsets)
	{
		Mesh mesh;
		mesh.CreateCuboidMesh(width, height, depth, useCornerOffsets, offsets);
		mesh.GenerateNormals();
		AddBind(new VertexBuffer(mesh.vertices));
	}

	// If currently not static initialised
	if (!GetBindable<IndexBuffer>(DXBindable::BindableType::INDEX_BUFFER))
	{
		Mesh mesh;
		mesh.CreateCuboidMesh(width, height, depth);
		mesh.GenerateNormals();

		VertexShader* vs = new VertexShader(L"PhongVS.cso");
		std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Texcoord",0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		AddStaticBind(new InputLayout(ied, vs->GetByteBlob()));
		AddStaticBind(vs);

		AddStaticBind(new IndexBuffer(mesh.indices));
		AddStaticBind(new VertexBuffer(mesh.vertices));

		AddStaticBind(new PixelShader(L"PhongPS.cso"));
		AddStaticBind(new Texture(path));
		AddStaticBind(new Sampler());
		AddStaticBind((new WorldConstantBuffer(transform)));
	}


	wcb = GetBindable<WorldConstantBuffer>(DXBindable::BindableType::WORLD_CONSTANT_BUFFER);
	indexCount = GetBindable<IndexBuffer>(DXBindable::BindableType::INDEX_BUFFER)->GetIndexCount();
	//AddBind(new WorldConstantBuffer(transform));
}


void Cuboid::Update()
{	
	if(wcb)
	wcb->Update(transform);
}

void Cuboid::Draw()
{
	staticBindLock->lock();
	if (uniqueIndex)
		usingStaticIndex = false;
	if (staticBindID != lastBindId || (!uniqueIndex && !usingStaticIndex))
	{
		usingStaticIndex = true;
		lastBindId = staticBindID;

		for (auto& [key, value] : staticBindables[staticBindID])
		{
			value->Bind();
		}
	}
	staticBindLock->unlock();
	for (auto& [key, value] : bindables)
	{
		value->Bind();
	}

	// If we havn't got the index count yet, find it from the IndexBuffer
	if (indexCount == -1)
	{
		indexCount = GetBindable<IndexBuffer>(DXBindable::BindableType::INDEX_BUFFER)->GetIndexCount();
	}

	context->DrawIndexed(indexCount, 0u, 0u);
}

bool Cuboid::usingStaticIndex = false;