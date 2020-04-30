#pragma once
#include <mutex>
#include "DXBindable.h"
class Texture :
	public DXBindable
{
public:
	Texture(const wchar_t* filePath);
	~Texture() = default;

	void Bind() override;
	BindableType GetType() override { return BindableType::TEXTURE; };
	Microsoft::WRL::ComPtr<ID3D11Resource> texture;
private:
	static std::mutex texLoadLock;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;

};

