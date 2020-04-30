#include "SimpleDebug.h"



SimpleDebug::SimpleDebug()
{
}


SimpleDebug::~SimpleDebug()
{
}

void SimpleDebug::ProcessHresult(HRESULT result)
{
	if (result != 0x00000000)
	{
		throw(result);
	}
}
