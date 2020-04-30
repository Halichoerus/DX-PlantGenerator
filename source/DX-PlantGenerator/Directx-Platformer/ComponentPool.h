#pragma once
#include <vector>
#include <stack>
#include "Component.h"

class ComponentPool
{
public:
	static ComponentPool& instance()
	{
		static ComponentPool inst;
		return inst;
	}

	// 
	template <class T>
	T CreateComponent()
	{
		bool newComponent = true;
		// Loop through the pool to see if this object type already has a vector
		for(int i = 0; i < pool.size(); i++)
		{
			if (!pool[i].empty && dynamic_cast<T>(pool[i][0]))
			{
				pool[i].push_back(Component(pool[i].size(),i));
				newComponent = false;
				break;
			}
		}

		if (newComponent)
		{
			pool.push_back(std::vector<Component>());
			pool.back.push_back(Component(0, pool.size()-1));
		}
	}

	template <class T>
	T GetComponent()
	{

	}

private:
	ComponentPool();
	ComponentPool(ComponentPool const&) = delete;
	ComponentPool operator=(ComponentPool const&) = delete;

	std::vector<std::vector<Component>> pool;
	std::stack<int> idPool;
	int nextId = 0;
};

