#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "../ThrowIfFailed.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")

class ComputeShader
{
public:
	ComputeShader(ID3D11Device * device, LPCWSTR path);
	~ComputeShader();

	operator ID3D11ComputeShader *() { return computeShader; }

private:
	ID3D11ComputeShader * computeShader;
};