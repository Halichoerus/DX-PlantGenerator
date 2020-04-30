#include "DXDrawable.h"
#include "IndexBuffer.h"
#include "Texture.h"

Graphics* DXDrawable::graphics = nullptr;
std::map<std::string, std::map<DXBindable::BindableType, std::unique_ptr<DXBindable>>> DXDrawable::staticBindables;
std::map<std::string, int> DXDrawable::objectCount;
std::string DXDrawable::lastBindId = "";
std::mutex* DXDrawable::staticBindLock = new std::mutex();

DXDrawable::DXDrawable()
{
	context = graphics->GetContext();
}

DXDrawable::~DXDrawable()
{
	staticBindLock->lock();
	if (objectCount.count(staticBindID) == 0)
	{
		staticBindLock->unlock();
		return;
	}

	objectCount[staticBindID]--;
	if (objectCount[staticBindID] == 0)
	{
		staticBindables.erase(staticBindID);
	}
	staticBindLock->unlock();
}

void DXDrawable::SetStaticBindID(std::string id)
{
	staticBindLock->lock();
	staticBindID = id;
	objectCount[staticBindID]++;
	staticBindLock->unlock();
}


void DXDrawable::Draw()
{
	staticBindLock->lock();
	if (staticBindID != lastBindId)
	{
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

void DXDrawable::BindAll()
{
	staticBindLock->lock();
	if (staticBindID != lastBindId)
	{
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
}

void DXDrawable::DrawOnly()
{
	context->DrawIndexed(indexCount, 0u, 0u);
}

