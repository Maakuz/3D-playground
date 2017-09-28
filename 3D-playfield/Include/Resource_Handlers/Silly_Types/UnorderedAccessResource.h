#pragma once
#include <d3d11.h>
#include "../ThrowIfFailed.h"
class UnorderedAccessResource
{
public:
	UnorderedAccessResource(ID3D11Device * device, UINT width, UINT height);
	~UnorderedAccessResource();

	operator ID3D11UnorderedAccessView*() const { return unorderedView; }
	operator ID3D11ShaderResourceView*() const { return shaderResourceView; }
private:
	ID3D11UnorderedAccessView * unorderedView;
	ID3D11ShaderResourceView * shaderResourceView;
};