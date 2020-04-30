#pragma once

#include "DXDrawable.h"
#include "Vertex.h"
#include "Transform.h"
#include "Mesh.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Model.h"

class Flower : public DXDrawable
{
public:
	Flower();
	~Flower() = default;

	void Update();
	void Draw() override;
private:
	void BuildStem(bool updateBuffers = true);
	void BuildFlower(bool updateBuffers = true);
	void BuildLeaves(bool updateBuffers = true);
	void RandomizeLeaves();

	void WriteToObj(std::string pathToWrite, std::string fileName);

	bool lowPoly = true;

	// File paths
	std::wstring flowerFolder = L"Textures/Flowers/";
	std::wstring flowerTexName = L"lotus_0282.png";
	std::wstring stemFolder = L"Textures/Stems/";
	std::wstring stemTexName = L"Grass.jpg";
	std::wstring leafFolder = L"Textures/Leaves/";
	std::wstring leafTexName = L"leaf2.png";

	// Stem
	float stemRadius = 0.05f;
	DirectX::XMFLOAT3 stemLength = { 0,3,0 };

	// Flower
	std::unique_ptr<VertexBuffer> flowerVB = nullptr;
	std::unique_ptr<IndexBuffer> flowerIB = nullptr;
	std::unique_ptr<Texture> flowerTex = nullptr;

	float flowerRadius = 2;
	float flowerCurve = 0;

	DirectX::XMFLOAT3 stemRotation = { 0,1,0 };

	DirectX::XMFLOAT3 flowerBL = { -flowerRadius,stemLength.y,-flowerRadius };
	DirectX::XMFLOAT3 flowerBR = { flowerRadius,stemLength.y,-flowerRadius };
	DirectX::XMFLOAT3 flowerTL = { -flowerRadius,stemLength.y,flowerRadius };
	DirectX::XMFLOAT3 flowerTR = { flowerRadius,stemLength.y,flowerRadius };

	// Leaves
	std::unique_ptr<VertexBuffer> leavesVB = nullptr;
	std::unique_ptr<IndexBuffer> leavesIB = nullptr;
	std::unique_ptr<Texture> leavesTex = nullptr;

	int leafCount = 1;
	std::vector<DirectX::XMFLOAT3> leafRotations = { { 0,0,0 } };
	std::vector<DirectX::XMFLOAT3> leafPositions = { { 0,0,0 } };
	std::vector<DirectX::XMFLOAT3> leafScales = { {1,1,0.5f} };


	// Transforms
	Transform transform;
	Mesh stem;
	Mesh flower;
	Mesh leaves;

	int test = 3;
};

