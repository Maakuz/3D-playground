#include "ComputeShader.h"
#ifdef DEBUG
#define SHADER_COMPILE_FLAGS D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION
#else
#define SHADER_COMPILE_FLAGS NULL
#endif

ComputeShader::ComputeShader(ID3D11Device * device, LPCWSTR path)
{
	ID3DBlob * csBlob = nullptr, * errorMsg = nullptr;

	HRESULT hr = D3DCompileFromFile(path, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS", "cs_5_0", SHADER_COMPILE_FLAGS, NULL, &csBlob, &errorMsg);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errorMsg->GetBufferPointer());
		throw "Failed to compile Compute Shader";
	}

	ThrowIfFailed(device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), NULL, &computeShader));

}

ComputeShader::~ComputeShader()
{
	SAFE_RELEASE(computeShader);
}