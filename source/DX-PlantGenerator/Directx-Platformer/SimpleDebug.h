#pragma once
#include "Winerror.h"

class SimpleDebug
{
public:
	SimpleDebug();
	~SimpleDebug();

	static void ProcessHresult(HRESULT result);
};

