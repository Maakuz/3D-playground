#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "ThrowIfFailed.h"
#include <initializer_list>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")



class Shader
{
public:
	Shader(ID3D11Device * device, LPCWSTR path, std::initializer_list<D3D11_INPUT_ELEMENT_DESC> inputDesc, bool gs = false);
	~Shader();

	operator ID3D11VertexShader*() { return vertexShader; }
	operator ID3D11PixelShader*() { return pixelShader; }
	operator ID3D11GeometryShader*() { return geometryShader; }
	operator ID3D11InputLayout*() { return layout; }

private:
	ID3D11InputLayout * layout;
	ID3D11VertexShader * vertexShader;
	ID3D11PixelShader * pixelShader;
	ID3D11GeometryShader * geometryShader;
};