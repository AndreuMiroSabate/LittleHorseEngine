#pragma once
#include "Module.h"
class ModuleD3D12 :
    public Module
{
public:
	ModuleD3D12(HWND hWnd);
	~ModuleD3D12();
	bool init() override;

private:
	HWND hWnd = NULL;
};

