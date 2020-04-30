#include "ComponentPool.h"
#include "Component.h"

ComponentPool::ComponentPool()
{
	for (; nextId < 10; nextId++)
	{
		idPool.push(nextId);
	}
}
