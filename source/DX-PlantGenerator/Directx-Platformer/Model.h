#pragma once
#include "DXDrawable.h"
#include "Transform.h"
#include "Texture.h"
#include <mutex>

class Model : public DXDrawable
{
public:
	Model() = default;
	~Model();

	bool LoadFromFile(std::string filepath, std::string name);

	void Update();
	void Draw() override;

	Transform transform;

private:
	bool GetWord(const std::string& line, int& index, std::string& result, char delim = ' ');

	static std::mutex streamLock;

	std::vector<UINT> indexCounts;
	std::vector<Texture> textures;
};

