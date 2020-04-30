#pragma once
#include <map>
#include <memory>
#include <string>
#include <mutex>
#include "DXBindable.h"
#include "IndexBuffer.h"

// Objects of this class type should have a valid set of DXBinds in order for it to be drawable
class DXDrawable
{
public:
	DXDrawable();
	virtual ~DXDrawable();
	
	virtual void Draw();

	void BindAll();
	void DrawOnly();
	
	void AddBind(DXBindable* bindable)
	{
		using pair = std::pair<DXBindable::BindableType, std::unique_ptr<DXBindable>>;
		bindables.insert(pair(bindable->GetType(), bindable));
	}

	void AddBind(IndexBuffer* indexBuffer)
	{
		indexCount = indexBuffer->GetIndexCount();
		using pair = std::pair<DXBindable::BindableType, std::unique_ptr<DXBindable>>;
		bindables.insert(pair(indexBuffer->GetType(), indexBuffer));
	}
	
	void AddStaticBind(DXBindable* bindable)
	{
		using pair = std::pair<DXBindable::BindableType, std::unique_ptr<DXBindable>>;
		using sPair = std::pair<std::string, std::map<DXBindable::BindableType, std::unique_ptr<DXBindable>>>;
	
		staticBindLock->lock();
		if (staticBindables.count(staticBindID) == 0)
		{
			staticBindables.insert(sPair(staticBindID, std::map<DXBindable::BindableType, std::unique_ptr<DXBindable>>() ));
		}
	
		staticBindables[staticBindID].insert(pair(bindable->GetType(), std::unique_ptr<DXBindable>(bindable)));
		staticBindLock->unlock();
	}

	void AddStaticBind(IndexBuffer* indexBuffer)
	{
		indexCount = indexBuffer->GetIndexCount();
		using pair = std::pair<DXBindable::BindableType, std::unique_ptr<DXBindable>>;
		using sPair = std::pair <std::string, std::map<DXBindable::BindableType, std::unique_ptr<DXBindable>> > ;

		staticBindLock->lock();
		if (staticBindables.count(staticBindID) == 0)
		{

			staticBindables.insert(sPair(staticBindID, std::map<DXBindable::BindableType, std::unique_ptr<DXBindable>>()));
		}

		staticBindables[staticBindID].insert(pair(indexBuffer->GetType(), std::unique_ptr<DXBindable>(indexBuffer)));
		staticBindLock->unlock();
	}
	
	template<class t>
	t* GetBindable(const DXBindable::BindableType& bindableID)
	{
		staticBindLock->lock();
		if (bindables.count(bindableID) > 0)
		{
			auto re = static_cast<t*>((bindables[bindableID]).get());
			staticBindLock->unlock();
			return re;
		}
		else if (staticBindables[staticBindID].count(bindableID) > 0)
		{
			auto re = static_cast<t*>((staticBindables[staticBindID][bindableID]).get());
			staticBindLock->unlock();
			return re;
		}
		else
		{
			staticBindLock->unlock();
			return nullptr;
		}
	}

	static void SetGraphics(Graphics* g) { graphics = g; };

	void SetStaticBindID(std::string id);

protected:
	static Graphics* graphics;
	static std::string lastBindId;
	static std::mutex* staticBindLock;

	UINT indexCount = -1;
	static std::map<std::string, std::map<DXBindable::BindableType, std::unique_ptr<DXBindable>>> staticBindables;
	static std::map<std::string, int> objectCount;
	std::map<DXBindable::BindableType, std::unique_ptr<DXBindable>> bindables;
	std::string staticBindID = "default";

	ID3D11DeviceContext* context = nullptr;

};

