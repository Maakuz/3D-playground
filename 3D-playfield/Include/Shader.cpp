#include "Shader.h"
#ifdef DEBUG
#define SHADER_COMPILE_FLAGS D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION
#else
#define SHADER_COMPILE_FLAGS NULL
#endif

Shader::Shader(ID3D11Device * device, LPCWSTR path, std::initializer_list<D3D11_INPUT_ELEMENT_DESC> inputDesc, bool gs)
{
	geometryShader = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	layout = nullptr;
	ID3DBlob * vsBlob = nullptr, *psBlob = nullptr, *gsBlob = nullptr, *errorMsg = nullptr;

	HRESULT hr = D3DCompileFromFile(path, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", SHADER_COMPILE_FLAGS, NULL, &vsBlob, &errorMsg);

	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errorMsg->GetBufferPointer());
		throw "Failed to compile Pixel Shader";
	}

	hr = D3DCompileFromFile(path, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", SHADER_COMPILE_FLAGS, NULL, &psBlob, &errorMsg);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errorMsg->GetBufferPointer());
		throw "Failed to compile Pixel Shader";
	}

	if (gs)
	{
		hr = D3DCompileFromFile(path, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS", "gs_5_0", SHADER_COMPILE_FLAGS, NULL, &gsBlob, &errorMsg);
		if (FAILED(hr))
		{
			OutputDebugStringA((char*)errorMsg->GetBufferPointer());
			throw "Failed to compile Pixel Shader";
		}
	}
	

	ThrowIfFailed(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vertexShader));
	ThrowIfFailed(device->CreateInputLayout(inputDesc.begin(), inputDesc.size(), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &layout));
	ThrowIfFailed(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &pixelShader));

	if(gs)
		ThrowIfFailed(device->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), NULL, &geometryShader));
}

Shader::~Shader()
{
	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(layout);
	SAFE_RELEASE(geometryShader);
}
