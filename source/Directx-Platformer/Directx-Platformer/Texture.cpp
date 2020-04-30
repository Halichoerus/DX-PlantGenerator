#include "Texture.h"
#include "SimpleDebug.h"
#include "WICTextureLoader.h"

std::mutex Texture::texLoadLock;

Texture::Texture(const wchar_t* filePath)
{
	texLoadLock.lock();
	SimpleDebug::ProcessHresult(CreateWICTextureFromFile(graphics->GetDevice(), filePath, &texture, &textureView));
	texLoadLock.unlock();
}

void Texture::Bind()
{
	graphics->GetContext()->PSSetShaderResources(0u, 1u, textureView.GetAddressOf());
}
