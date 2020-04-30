#pragma once
#include <vector>
#include "Graphics.h"

class DXBindable
{
public:
	enum class BindableType
	{
		INDEX_BUFFER,
		INPUT_LAYOUT,
		PIXEL_SHADER,
		SAMPLER,
		TEXTURE,
		VERTEX_BUFFER,
		VERTEX_SHADER,
		WORLD_CONSTANT_BUFFER,
		PIXEL_BUFFER,
		BLEND_STATE
	};

	DXBindable() = default;
	virtual ~DXBindable() {};

	virtual void Bind() = 0;
	virtual BindableType GetType() = 0;

	static void SetGraphics(Graphics* g) { graphics = g; };
protected:
	static Graphics* graphics;
};



