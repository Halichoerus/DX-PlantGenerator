#include "Model.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Sampler.h"
#include "InputLayout.h"
#include "WorldConstantBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Vertex.h"
#include "Mesh.h"

#include <fstream>
#include <iostream>
#include <queue>
#include <map>

Model::~Model()
{

}

// Filepath should contain the .mtl and .obj files, these should both have the same name
bool Model::LoadFromFile(std::string filepath, std::string name)
{
	std::queue <std::string> data;
	std::map<std::string, std::string> texPaths;
	std::vector<Vertex> vertPos;
	std::vector<Vertex> vertNorm;
	std::vector<Vertex> vertTex;
	// Add one to each of these since .obj files start at index 1
	vertPos.push_back(Vertex());
	vertNorm.push_back(Vertex());
	vertTex.push_back(Vertex());

	std::string id = "";
	std::string lastMatKey = "";
	int readPos = 0;
	int indexOffset[3] = { 0,0,0 };

	// Ifstream is not thread safe, so lock its use then read all the required data and unlock
	streamLock.lock();


	// Load the texture data
	std::ifstream stream = std::ifstream(filepath + "/" + name + ".mtl");
	if (stream.is_open())
	{
		
		std::string line;

		while (std::getline(stream, line))
		{
			bool cont = true;
			while (cont)
			{
				if (line == "") break;
				if (GetWord(line, readPos, id))
				{
					break;
				}

				if (id == "newmtl")
				{
					lastMatKey = "";
					GetWord(line, readPos, lastMatKey);
				}
				else if (id == "map_Kd")
				{
					std::string newPath = "";
					GetWord(line, readPos, newPath);
					texPaths[lastMatKey] = newPath;
				}
				else if (id == "#")
				{
					break;
				}
				else
				{
					continue;
				}
				cont = false;
			}
			readPos = 0;
		}
	}
	else
	{
		std::string error = "Could not open file: " + name;
		OutputDebugString(error.c_str());
		streamLock.unlock();
		return false;
	}

	stream.close();

	// Load the vert data
	stream = std::ifstream(filepath + "/" + name + ".obj");
	if (stream.is_open())
	{
		std::string line;
		while (std::getline(stream, line))
		{
			if (line == "") continue;
			data.push(line);
		}
	}
	else
	{
		std::cerr << "Could not open file: " << name << std::endl;
	}
	stream.close();
	streamLock.unlock();

	// Create some variables to store vertex value so we are not constantly creating/destroying them
	Mesh mesh;
	float verts[3];
	Vertex faceVerts[4];
	std::string vert;
	std::string index;
	int matIndex = -1;

	// Process the read data
	while (!data.empty())
	{
		if (GetWord(data.front(), readPos, id))
		{
			readPos = 0;
			data.pop();
			continue;
		}

		if (id == "o")
		{
			indexOffset[0] = vertPos.size()-1;
			indexOffset[1] = vertTex.size()-1;
			indexOffset[2] = vertNorm.size()-1;
		}
		else if (id == "v")
		{
			for(int i = 0; i<3; i++)
			{
				GetWord(data.front(), readPos, vert);
				verts[i] = std::atof(vert.c_str());
			}
			vertPos.push_back(Vertex(verts[0], verts[1], verts[2]));
		}
		else if (id == "vn")
		{
			for (int i = 0; i < 3; i++)
			{
				GetWord(data.front(), readPos, vert);
				verts[i] = std::atof(vert.c_str());
			}
			vertNorm.push_back(Vertex());
			vertNorm.back().normal.x = verts[0];
			vertNorm.back().normal.y = verts[1];
			vertNorm.back().normal.z = verts[2];
		}
		else if (id == "vt")
		{
			for (int i = 0; i < 2; i++)
			{
				GetWord(data.front(), readPos, vert);
				verts[i] = std::atof(vert.c_str());
			}
			vertTex.push_back(Vertex(0,0,0,verts[0], verts[1]));
		}
		else if (id == "usemtl")
		{
			GetWord(data.front(), readPos, lastMatKey);
			if (texPaths.count(lastMatKey) == 0)
			{
				std::string error = "Error in reading model: " + name + ", the .mtl does not contain the material " + lastMatKey + "\n";
				OutputDebugString(error.c_str());
			}
			std::string path = filepath + "/" + texPaths[lastMatKey];
			std::wstring asWide;
			const char* cstr = path.c_str();
			std::copy(cstr, cstr + strlen(cstr), back_inserter(asWide));
			const WCHAR* wcstr = asWide.c_str();
			textures.push_back(Texture(wcstr));

			indexCounts.push_back(0u);
			matIndex++;
		}
		else if (id == "f")
		{
			int i = 0;
			bool cont = true;
			while (cont)
			{
				cont = !GetWord(data.front(), readPos, vert);
				if (vert == "")
				{
					break;
				}
				if (i > 3)
				{
					std::string error = "Error in reading model: " + name + " at face number: " + std::to_string(mesh.vertices.size() / 3.) + ", the .obj file has faces with more than 4 indices, this is not currently supported.\n";
					OutputDebugString(error.c_str());
					return false;
				}

				int readPos2 = 0;
				for (int j = 0; j < 3; j++)
				{
					GetWord(vert, readPos2, index, '/');
					verts[j] = std::atoi(index.c_str());
				}
				faceVerts[i] = Vertex();

				//verts[0] += indexOffset[0];
				//verts[1] += indexOffset[1];
				//verts[2] += indexOffset[2];

				if (verts[0] >= vertPos.size() || verts[1] >= vertTex.size() || verts[2] >= vertNorm.size())
				{
					std::string error = "Warning: A face in the.obj file for " + name + " is referencing indicies which don't exsist! Skipping this face.\n";
					OutputDebugString(error.c_str());
					i = -1;
					break;
				}

				faceVerts[i].position = verts[0] >= 0 ? vertPos[verts[0]].position : vertPos[vertPos.size() + (size_t)verts[0]].position;
				faceVerts[i].tex = verts[1] >= 0 ? vertTex[verts[1]].tex : vertTex[vertTex.size() + (size_t)verts[1]].tex;
				faceVerts[i].normal = verts[2] >= 0 ? vertNorm[verts[2]].normal : vertNorm[vertNorm.size() + (size_t)verts[2]].normal;

				i++;
			}

			if (i == 3)
			{
				mesh.PushTriangle(faceVerts[0], faceVerts[1], faceVerts[2]);
				indexCounts[matIndex] += 3;
			}
			else if(i == 4)
			{
				// This engine only supports triangle faces currently, so covert all quads to double tris
				mesh.PushTriangle(faceVerts[0], faceVerts[1], faceVerts[2]);
				mesh.PushTriangle(faceVerts[2], faceVerts[3], faceVerts[0]);
				indexCounts[matIndex] += 6;
			}
			else if(i != -1)
			{
				std::string error;
				if(i < 3)
					 error = "Error in reading model: " + name + " at face number: " + std::to_string(mesh.vertices.size() / 3.) + ", the .obj file has faces with fewer than 3 indices, this is clearly an error!\n";
				else
					error = "Error in reading model: " + name + " at face number: " + std::to_string(mesh.vertices.size() / 3.) + ", the .obj file has faces with more than 4 indices, this is not currently supported.\n";
				OutputDebugString(error.c_str());
				return false;
			}


		}
		else if (id == "#")
		{
			readPos = 0;
			data.pop();
			continue;
		}
		else
		{
			continue;
		}

		readPos = 0;
		data.pop();
	}

	SetStaticBindID(name);
	

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
		
		AddStaticBind(new IndexBuffer(mesh.indices));
		AddStaticBind(new VertexBuffer(mesh.vertices));
		AddStaticBind(vs);
		AddStaticBind(new PixelShader(L"PhongPS.cso"));
		AddStaticBind(new Sampler());
	}

	AddBind(new WorldConstantBuffer(transform));
	return true;
}

// Returns true if end of line
bool Model::GetWord(const std::string& line, int& index, std::string& result, char delim)
{
	result = "";
	while (line[index] != delim && line[index] != ' ' && line[index] != '\t')
	{
		if (index == line.size())
		{
			return true;
		}
		result += line[index];
		index++;
	}
	index++;
	while (line[index] == ' ' || line[index] == '\t')
	{
		index++;
		if (index == line.size())
		{
			return true;
		}
	}
	return false;
}

void Model::Update()
{
	GetBindable<WorldConstantBuffer>(DXBindable::BindableType::WORLD_CONSTANT_BUFFER)->Update(transform);
}

void Model::Draw()
{
	for (auto& [key, value] : staticBindables[staticBindID])
	{
		value->Bind();
	}
	for (auto& [key, value] : bindables)
	{
		value->Bind();
	}
	
	UINT indexTotal = 0;
	for (int i = 0; i < textures.size(); i++)
	{
		textures[i].Bind();
		graphics->GetContext()->DrawIndexed(indexCounts[i], indexTotal, 0u);
		indexTotal += indexCounts[i];
	}

}

std::mutex Model::streamLock;