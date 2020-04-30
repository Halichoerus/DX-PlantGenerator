#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

#include "Flower.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Sampler.h"
#include "InputLayout.h"
#include "WorldConstantBuffer.h"
#include "BlendState.h"
#include "imgui.h"

Flower::Flower()
{
	//testModel.LoadFromFile("Models/Tree", "Tree");
	//testModel.LoadFromFile("C:/Users/tobyj/OneDrive/Desktop", "TestFlower");

	SetStaticBindID("Flower");

	BuildStem(false);
	BuildFlower(false);
	BuildLeaves(false);
	RandomizeLeaves();



	std::wstring texPath = stemFolder + stemTexName;

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

		AddStaticBind(new VertexBuffer(stem.vertices, D3D11_USAGE_DYNAMIC));
		AddStaticBind(new IndexBuffer(stem.indices, D3D11_USAGE_DYNAMIC));
		AddStaticBind(vs);
		AddStaticBind(new PixelShader(L"PhongPS.cso"));

		AddStaticBind(new Texture(texPath.c_str()));
		AddStaticBind(new Sampler());
		AddStaticBind(new BlendState());
	}



	transform.Position().y = -2;
	AddBind(new WorldConstantBuffer(transform));

	// Add additional buffers and textures for other plant parts
	flowerVB = std::make_unique<VertexBuffer>(flower.vertices, D3D11_USAGE_DYNAMIC);
	flowerIB = std::make_unique<IndexBuffer>(flower.indices, D3D11_USAGE_DYNAMIC);
	texPath = flowerFolder + flowerTexName;
	flowerTex = std::make_unique<Texture>(texPath.c_str());

	leavesVB = std::make_unique<VertexBuffer>(leaves.vertices, D3D11_USAGE_DYNAMIC);
	leavesIB = std::make_unique<IndexBuffer>(leaves.indices, D3D11_USAGE_DYNAMIC);
	texPath = leafFolder + leafTexName;
	leavesTex = std::make_unique<Texture>(texPath.c_str());
}

void Flower::Update()
{
	bool flowerDirty = false;
	if (ImGui::Begin("Controls"))
	{
		ImGui::Text("Stem Rotation");
		if (ImGui::SliderAngle("X", &transform.Rotation().x) ||
			ImGui::SliderAngle("Y", &transform.Rotation().y) ||
			ImGui::SliderAngle("Z", &transform.Rotation().z))
		{
			GetBindable<WorldConstantBuffer>(DXBindable::BindableType::WORLD_CONSTANT_BUFFER)->Update(transform);
		}

		if (ImGui::Button("Reset"))
		{
			transform.Rotation() = { 0,0,0 };
			GetBindable<WorldConstantBuffer>(DXBindable::BindableType::WORLD_CONSTANT_BUFFER)->Update(transform);
		}

		if (ImGui::Button("Write To .OBJ"))
		{
			WriteToObj("C:/Users/tobyj/OneDrive/Desktop/", "TestFlower");
		}
	}
	ImGui::End();

	if (ImGui::Begin("Plant"))
	{
		ImGui::Text("Stem Parameters");
		if (
			ImGui::SliderFloat("Stem Radius", &stemRadius, 0.01f, 1.0f) ||
			ImGui::SliderFloat("Length", &stemLength.y, 0.1f, 5.0f)
			)
		{
			flowerBL.y = stemLength.y;
			flowerBR.y = stemLength.y;
			flowerTL.y = stemLength.y;
			flowerTR.y = stemLength.y;
			BuildStem();
			BuildFlower();
		}

		ImGui::Text("Flower Parameters");
		if (ImGui::SliderFloat("Flower Radius", &flowerRadius, 0.5f, 3.0f))
		{
			flowerBL = { -flowerRadius,stemLength.y,-flowerRadius };
			flowerBR = { flowerRadius,stemLength.y,-flowerRadius };
			flowerTL = { -flowerRadius,stemLength.y,flowerRadius };
			flowerTR = { flowerRadius,stemLength.y,flowerRadius };
			flowerDirty = true;
		}
		if (ImGui::SliderFloat("Flower Curve", &flowerCurve, -1, 1))
		{
			flowerDirty = true;
		}
		if (
			ImGui::SliderFloat("x Direction", &stemRotation.x, -1.0f, 1.0f) ||
			ImGui::SliderFloat("z Direction", &stemRotation.z, -1.0f, 1.0f)
			)
		{
			if (!(stemRotation.x == 0 && stemRotation.y == 0 && stemRotation.z == 0))
			{
				flowerDirty = true;
			}
		}

		ImGui::Text("Leaf Parameters");

		int leafCountBefore = leafCount;
		if (ImGui::SliderInt("Number of leaves", &leafCount, 1, 15))
		{
			leafPositions.resize(leafCount);
			leafRotations.resize(leafCount);
			leafScales.resize(leafCount);

			std::default_random_engine generator;
			generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			std::uniform_int_distribution<int> distribution(0, 360);
			std::uniform_int_distribution<int> distribution2(-45, 45);
			std::uniform_int_distribution<int> distribution3(0,  (int)((stemLength.y-0.5f) * 100.0f));
			std::uniform_int_distribution<int> distribution4(40, 150);


			for (int i = leafCountBefore; i < leafCount; i++)
			{
				leafScales[i] = { 1,1,0.5f };
				leafRotations[i] = { (float)distribution(generator) * 0.01745f, (float)distribution2(generator) * 0.01745f, (float)distribution2(generator) * 0.01745f };
				leafPositions[i] = { 0, (float)distribution3(generator) / 100,0 };
				float scale = (float)distribution4(generator) / 100;
				leafScales[i].x = scale;
				leafScales[i].z = scale/2;
			}

			BuildLeaves();
			BuildFlower();
			BuildStem();
		}

		if (ImGui::Button("Randomize Leaves"))
		{
			RandomizeLeaves();
			BuildLeaves();
			BuildFlower();
			BuildStem();
		}

		ImGui::BeginGroup();
		for (int i = 0; i < leafCount; i++)
		{
			ImGui::Text(std::to_string(i+1).c_str());
			if (

				ImGui::SliderAngle(("x Rotation " + std::to_string(i + 1)).c_str(), &leafRotations[i].x) ||
				ImGui::SliderAngle(("y Rotation " + std::to_string(i + 1)).c_str(), &leafRotations[i].y) ||
				ImGui::SliderAngle(("z Rotation " + std::to_string(i + 1)).c_str(), &leafRotations[i].z) ||
								   
				ImGui::SliderFloat(("y Position " + std::to_string(i + 1)).c_str(), &leafPositions[i].y, 0, stemLength.y-0.5f) ||
								   
				ImGui::SliderFloat(("x Scale " + std::to_string(i + 1)).c_str(), &leafScales[i].x, 0.1f, 1.5f) ||
				ImGui::SliderFloat(("z Scale " + std::to_string(i + 1)).c_str(), &leafScales[i].z, 0.1f, 1.5f))
			{
				BuildLeaves();
				BuildFlower();
				BuildStem();
			}

		
		}
		ImGui::EndGroup();
	}

	if (flowerDirty)
	{
		BuildFlower();
	}
	ImGui::End();
}

void Flower::Draw()
{
	DXDrawable::Draw();

	flowerVB->Bind();
	flowerIB->Bind();
	flowerTex->Bind();
	graphics->GetContext()->DrawIndexed(flowerIB->GetIndexCount(), 0u, 0u);

	leavesVB->Bind();
	leavesIB->Bind();
	leavesTex->Bind();
	graphics->GetContext()->DrawIndexed(leavesIB->GetIndexCount(), 0u, 0u);
}

void Flower::BuildStem(bool updateBuffers)
{
	stem.Clear();



	indexCount = -1;
	if (lowPoly)
	{
		//

		// x-axis Back facing
		stem.PushTriangle(
			Vertex(-stemRadius, 0, 0, 0, 1),
			Vertex(stemRadius, 0, 0, 1, 1),
			Vertex(-stemRadius, stemLength.y, 0, 0, 0)
		);
		stem.PushTriangle(
			Vertex(-stemRadius, stemLength.y, 0, 0, 0),
			Vertex(stemRadius, 0, 0, 1, 1),
			Vertex(stemRadius, stemLength.y, 0, 1, 0)
		);

		// x-axis Front facing
		stem.PushTriangle(
			Vertex(-stemRadius, 0, 0, 0, 1),
			Vertex(-stemRadius, stemLength.y, 0, 0, 0),
			Vertex(stemRadius, 0, 0, 1, 1)
		);
		stem.PushTriangle(
			Vertex(-stemRadius, stemLength.y, 0, 0, 0),
			Vertex(stemRadius, stemLength.y, 0, 1, 0),
			Vertex(stemRadius, 0, 0, 1, 1)
		);

		// z-axis back facing
		stem.PushTriangle(
			Vertex(0, 0, -stemRadius, 0, 1),
			Vertex(0, 0, stemRadius, 1, 1),
			Vertex(0, stemLength.y, -stemRadius, 0, 0)
		);
		stem.PushTriangle(
			Vertex(0, stemLength.y, -stemRadius, 0, 0),
			Vertex(0, 0, stemRadius, 1, 1),
			Vertex(0, stemLength.y, stemRadius, 1, 0)
		);

		// z-axis front facing
		stem.PushTriangle(
			Vertex(0, 0, -stemRadius, 0, 1),
			Vertex(0, stemLength.y, -stemRadius, 0, 0),
			Vertex(0, 0, stemRadius, 1, 1)
		);
		stem.PushTriangle(
			Vertex(0, stemLength.y, -stemRadius, 0, 0),
			Vertex(0, stemLength.y, stemRadius, 1, 0),
			Vertex(0, 0, stemRadius, 1, 1)
		);

	}

	stem.GenerateNormals();
	if (updateBuffers)
	{
		GetBindable<VertexBuffer>(DXBindable::BindableType::VERTEX_BUFFER)->Update(stem.vertices);
		GetBindable<IndexBuffer>(DXBindable::BindableType::INDEX_BUFFER)->Update(stem.indices);
	}
}

void Flower::BuildFlower(bool updateBuffers)
{
	flower.Clear();
	if (lowPoly)
	{
		// Generate the plane of the flower
		DirectX::XMVECTOR plane = DirectX::XMPlaneFromPointNormal(DirectX::XMLoadFloat3(&stemLength), DirectX::XMLoadFloat3(&stemRotation));
		
		DirectX::XMFLOAT3 bl;
		DirectX::XMFLOAT3 br;
		DirectX::XMFLOAT3 tl;
		DirectX::XMFLOAT3 tr;

		// Load the 4 corners for the plane into a XMVector
		DirectX::XMVECTOR blv = XMLoadFloat3(&flowerBL);
		DirectX::XMVECTOR brv = XMLoadFloat3(&flowerBR);
		DirectX::XMVECTOR tlv = XMLoadFloat3(&flowerTL);
		DirectX::XMVECTOR trv = XMLoadFloat3(&flowerTR);

		DirectX::XMVECTOR rot = XMLoadFloat3(&stemRotation);

		// Calculate an offset for the corners
		DirectX::XMFLOAT3 offset = stemRotation;

		// Load the flower curve as a XMVector
		DirectX::XMFLOAT3 fc;
		if (flowerCurve < 0)
		{
			fc = { -flowerCurve, flowerCurve, -flowerCurve };
		}
		else
		{
			fc = { flowerCurve, flowerCurve, flowerCurve };
		}

		DirectX::XMVECTOR fcV = XMLoadFloat3(&fc);

		// Offset the corners along the rotation direction
		DirectX::XMVECTOR offsetV = DirectX::XMLoadFloat3(&offset);
		offsetV = DirectX::XMVectorMultiply(offsetV, fcV);

		// Offset the corners towards the middle
		blv = DirectX::XMVectorSubtract(blv, DirectX::XMVectorMultiply(blv, fcV));
		brv = DirectX::XMVectorSubtract(brv, DirectX::XMVectorMultiply(brv, fcV));
		tlv = DirectX::XMVectorSubtract(tlv, DirectX::XMVectorMultiply(tlv, fcV));
		trv = DirectX::XMVectorSubtract(trv, DirectX::XMVectorMultiply(trv, fcV));

		// Find the intersect of the corners with the plane
		DirectX::XMVECTOR bri = DirectX::XMPlaneIntersectLine(plane,brv , DirectX::XMVectorAdd(brv, rot));
		DirectX::XMVECTOR bli = DirectX::XMPlaneIntersectLine(plane,blv , DirectX::XMVectorAdd(blv, rot));
		DirectX::XMVECTOR tli = DirectX::XMPlaneIntersectLine(plane,tlv , DirectX::XMVectorAdd(tlv, rot));
		DirectX::XMVECTOR tri = DirectX::XMPlaneIntersectLine(plane,trv , DirectX::XMVectorAdd(trv, rot));

		// Apply the offset to the intersects
		bri = DirectX::XMVectorAdd(bri, offsetV);
		bli = DirectX::XMVectorAdd(bli, offsetV);
		tli = DirectX::XMVectorAdd(tli, offsetV);
		tri = DirectX::XMVectorAdd(tri, offsetV);

		// Convert back to usable float3's
		DirectX::XMStoreFloat3(&br, bri);
		DirectX::XMStoreFloat3(&bl, bli);
		DirectX::XMStoreFloat3(&tl, tli);
		DirectX::XMStoreFloat3(&tr, tri);

		// Generate the triangles in front/back facing pairs
		flower.PushTriangle(
			Vertex(bl.x, bl.y, bl.z, 0, 1),
			Vertex(br.x, br.y, br.z, 1, 1),
			Vertex(0, stemLength.y, 0, 0.5f, 0.5f)
		);
		flower.PushTriangle(
			Vertex(bl.x, bl.y, bl.z, 0, 1),
			Vertex(0, stemLength.y, 0, 0.5f, 0.5f),
			Vertex(br.x, br.y, br.z, 1, 1)
		);

		flower.PushTriangle(
			Vertex(br.x, br.y, br.z, 1, 1),
			Vertex(tr.x, tr.y, tr.z, 1, 0),
			Vertex(0, stemLength.y, 0, 0.5f, 0.5f)
		);
		flower.PushTriangle(
			Vertex(br.x, br.y, br.z, 1, 1),
			Vertex(0, stemLength.y, 0, 0.5f, 0.5f),
			Vertex(tr.x, tr.y, tr.z, 1, 0)
		);

		flower.PushTriangle(
			Vertex(tr.x, tr.y, tr.z, 1, 0),
			Vertex(tl.x, tl.y, tl.z, 0, 0),
			Vertex(0, stemLength.y, 0, 0.5f, 0.5f)
		);
		flower.PushTriangle(
			Vertex(tr.x, tr.y, tr.z, 1, 0),
			Vertex(0, stemLength.y, 0, 0.5f, 0.5f),
			Vertex(tl.x, tl.y, tl.z, 0, 0)
		);

		flower.PushTriangle(
			Vertex(tl.x, tl.y, tl.z, 0, 0),
			Vertex(bl.x, bl.y, bl.z, 0, 1),
			Vertex(0, stemLength.y, 0, 0.5f, 0.5f)
		);
		flower.PushTriangle(
			Vertex(tl.x, tl.y, tl.z, 0, 0),
			Vertex(0, stemLength.y, 0, 0.5f, 0.5f),
			Vertex(bl.x, bl.y, bl.z, 0, 1)
		);
	}
	else
	{

	}

	flower.GenerateNormals();
	if (updateBuffers)
	{
		flowerVB->Update(flower.vertices);
		flowerIB->Update(flower.indices);
	}
}

void Flower::BuildLeaves(bool updateBuffers)
{
	leaves.Clear();

	for (int i = 0; i < leafCount; i++)
	{
		std::vector<Vertex> newVerts;
		if (lowPoly)
		{
			//Front
			newVerts.push_back(Vertex(0, 0, 0.5f, 0.5f, 1));
			newVerts.push_back(Vertex(1, 0, 0.5f, 0.5f, 0));
			newVerts.push_back(Vertex(1, 0, -0.5, 1, 0));
			newVerts.push_back(Vertex(0, 0, -0.5, 1, 1));

			//Back						 
			newVerts.push_back(Vertex(0, 0, 0.5f, 0, 1));
			newVerts.push_back(Vertex(1, 0, 0.5f, 0, 0));
			newVerts.push_back(Vertex(1, 0, -0.5, 0.5f, 0));
			newVerts.push_back(Vertex(0, 0, -0.5, 0.5f, 1));

			DirectX::XMMATRIX matrix =
				DirectX::XMMatrixScaling(leafScales[i].x, leafScales[i].y, leafScales[i].z) *
				DirectX::XMMatrixRotationRollPitchYaw(leafRotations[i].z, leafRotations[i].x, leafRotations[i].y) *
				DirectX::XMMatrixTranslation(leafPositions[i].x, leafPositions[i].y, leafPositions[i].z);

			for (Vertex& v : newVerts)
			{
				DirectX::XMVECTOR vec;
				vec = DirectX::XMLoadFloat3(&v.position);
				DirectX::XMStoreFloat3(&v.position, DirectX::XMVector3Transform(vec, matrix));
			}
			leaves.PushTriangle(newVerts[0], newVerts[1], newVerts[2]);
			leaves.PushTriangle(newVerts[2], newVerts[3], newVerts[0]);
			leaves.PushTriangle(newVerts[4], newVerts[6], newVerts[5]);
			leaves.PushTriangle(newVerts[6], newVerts[4], newVerts[7]);
		}
	}

	leaves.GenerateNormals();
	if (updateBuffers)
	{
		leavesVB->Update(leaves.vertices);
		leavesIB->Update(leaves.indices);
		GetBindable<WorldConstantBuffer>(DXBindable::BindableType::WORLD_CONSTANT_BUFFER)->Update(transform);
	}
}

void Flower::RandomizeLeaves()
{
	std::default_random_engine generator;
	generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> distribution(0, 360);
	std::uniform_int_distribution<int> distribution2(-45, 45);
	std::uniform_int_distribution<int> distribution3(0, (int)((stemLength.y - 0.5f) * 100.0f));
	std::uniform_int_distribution<int> distribution4(40, 150);


	for (int i = 0; i < leafCount; i++)
	{
		leafScales[i] = { 1,1,0.5f };
		leafRotations[i] = { (float)distribution(generator) * 0.01745f, (float)distribution2(generator) * 0.01745f, (float)distribution2(generator) * 0.01745f };
		leafPositions[i] = { 0, (float)distribution3(generator) / 100,0 };
		float scale = (float)distribution4(generator) / 100;
		leafScales[i].x = scale;
		leafScales[i].z = scale / 2;
	}
}

void Flower::WriteToObj(std::string pathToWrite, std::string fileName)
{
	std::ofstream file(pathToWrite + fileName + ".obj");
	std::ofstream file2(pathToWrite + fileName + ".mtl");
	if (file.is_open() && file2.is_open())
	{
		file << "mtllib " + fileName + ".mtl" << std::endl;

		// Write all the vertex positions
		for (Vertex v : stem.vertices)
		{
			file << "v " << std::to_string(v.position.x) << " " << std::to_string(v.position.y) << " " << std::to_string(v.position.z) << std::endl;
		}
		for (Vertex v : flower.vertices)
		{
			file << "v " << std::to_string(v.position.x) << " " << std::to_string(v.position.y) << " " << std::to_string(v.position.z) << std::endl;
		}
		for (Vertex v : leaves.vertices)
		{
			file << "v " << std::to_string(v.position.x) << " " << std::to_string(v.position.y) << " " << std::to_string(v.position.z) << std::endl;
		}

		// Write all the texture co-ordinates
		for (Vertex v : stem.vertices)
		{
			file << "vt " << std::to_string(v.tex.x) << " " << std::to_string(-v.tex.y) << std::endl;
		}
		for (Vertex v : flower.vertices)
		{
			file << "vt " << std::to_string(v.tex.x) << " " << std::to_string(-v.tex.y) << std::endl;
		}
		for (Vertex v : leaves.vertices)
		{
			file << "vt " << std::to_string(v.tex.x) << " " << std::to_string(-v.tex.y) << std::endl;
		}

		// Write all the normals
		for (Vertex v : stem.vertices)
		{
			file << "vn " << std::to_string(v.normal.x) << " " << std::to_string(v.normal.y) << " " << std::to_string(v.normal.z) << std::endl;
		}
		for (Vertex v : flower.vertices)
		{
			file << "vn " << std::to_string(v.normal.x) << " " << std::to_string(v.normal.y) << " " << std::to_string(v.normal.z) << std::endl;
		}
		for (Vertex v : leaves.vertices)
		{
			file << "vn " << std::to_string(v.normal.x) << " " << std::to_string(v.normal.y) << " " << std::to_string(v.normal.z) << std::endl;
		}

		// Get the name of the texture without its extension
		std::string texName = "";

		int i = 0;
		while (stemTexName[i] != '.')
		{
			texName += stemTexName[i];
			i++;
			if (i == stemTexName.length() - 1)
				break;
		}

		std::string ext = "";
		while (i != stemTexName.length())
		{
			ext += stemTexName[i];
			i++;
		}

		// Write to .mtl file
		file2 << "newmtl " << texName << std::endl;
		file2 << "d " << 0.9f << std::endl;
		file2 << "Tr " << 0.1f << std::endl;
		file2 << "map_Kd " << texName << ext << std::endl << std::endl;

		// Copy the material to output folder
		std::ifstream src(stemFolder + stemTexName, std::ios::binary);
		std::ofstream dst(pathToWrite + texName + ext, std::ios::binary);

		dst << src.rdbuf();
		src.close();
		dst.close();

		// Write the faces for the stem
		int startIndex = 1;
		int ifCount = 0;
		file << "usemtl " + texName << std::endl;
		for (unsigned short index : stem.indices)
		{
			if (ifCount == 0)
			{
				file << "f ";
			}
			file << index + startIndex << "/" << index + startIndex << "/" << index + startIndex;
			ifCount++;
			if (ifCount == 3)
			{
				file << std::endl;
				ifCount = 0;
			}
			else
			{
				file << " ";
			}
		}

		// Get the name of the texture without its extension
		texName = "";

		i = 0;
		while (flowerTexName[i] != '.')
		{
			texName += flowerTexName[i];
			i++;
			if (i == flowerTexName.length())
				break;
		}

		ext = "";
		while (i != flowerTexName.length())
		{
			ext += flowerTexName[i];
			i++;
		}

		// Write to .mtl file
		file2 << "newmtl " << texName << std::endl;
		file2 << "d " << 0.9f << std::endl;
		file2 << "Tr " << 0.1f << std::endl;
		file2 << "map_Kd " <<  texName << ext << std::endl << std::endl;

		// Copy the material to output folder
		src = std::ifstream(flowerFolder + flowerTexName, std::ios::binary);
		dst = std::ofstream(pathToWrite + texName + ext, std::ios::binary);

		dst << src.rdbuf();
		src.close();
		dst.close();

		// Write the faces for the flower
		startIndex = 1 + stem.vertices.size();
		ifCount = 0;
		file << "usemtl " + texName << std::endl;
		for (unsigned short index : flower.indices)
		{
			if (ifCount == 0)
			{
				file << "f ";
			}
			file << index + startIndex << "/" << index + startIndex << "/" << index + startIndex;
			ifCount++;
			if (ifCount == 3)
			{
				file << std::endl;
				ifCount = 0;
			}
			else
			{
				file << " ";
			}
		}

		// Get the name of the texture without its extension
		texName = "";

		i = 0;
		while (leafTexName[i] != '.')
		{
			texName += leafTexName[i];
			i++;
			if (i == leafTexName.length() - 1)
				break;
		}

		ext = "";
		while (i != leafTexName.length())
		{
			ext += leafTexName[i];
			i++;
		}

		// Write to .mtl file
		file2 << "newmtl " << texName << std::endl;
		file2 << "d " << 0.9f << std::endl;
		file2 << "Tr " << 0.1f << std::endl;
		file2 << "map_Kd " << texName << ext << std::endl << std::endl;

		// Copy the material to output folder
		src = std::ifstream(leafFolder + leafTexName, std::ios::binary);
		dst = std::ofstream(pathToWrite + texName + ext, std::ios::binary);

		dst << src.rdbuf();
		src.close();
		dst.close();

		// Write the faces for the leaves
		startIndex = 1 + stem.vertices.size() + flower.vertices.size();
		ifCount = 0;
		file << "usemtl " + texName << std::endl;
		for (unsigned short index : leaves.indices)
		{
			if (ifCount == 0)
			{
				file << "f ";
			}
			file << index + startIndex << "/" << index + startIndex << "/" << index + startIndex;
			ifCount++;
			if (ifCount == 3)
			{
				file << std::endl;
				ifCount = 0;
			}
			else
			{
				file << " ";
			}
		}
	}
	file.close();

}
