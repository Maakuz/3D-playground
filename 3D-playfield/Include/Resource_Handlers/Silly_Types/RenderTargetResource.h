#pragma once

#include <d3d11.h>
#include "../ThrowIfFailed.h"


class RenderTargetResource
{
public:
	RenderTargetResource(ID3D11Device * device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	//These two should be connected or this class has no purpose
	RenderTargetResource(ID3D11RenderTargetView * rtv, ID3D11ShaderResourceView * srv);
	~RenderTargetResource();

	operator ID3D11RenderTargetView*() const { return renderTargetView; }
	operator ID3D11RenderTargetView*const*() const { return &renderTargetView; }
	operator ID3D11ShaderResourceView*() const { return shaderResourceView; }
	operator ID3D11ShaderResourceView*const*() const { return &shaderResourceView; }
	operator ID3D11UnorderedAccessView*() const { return unorderedView; }
	operator ID3D11UnorderedAccessView*const*() const { return &unorderedView; }

private:

	ID3D11RenderTargetView * renderTargetView;
	ID3D11ShaderResourceView * shaderResourceView;
	ID3D11UnorderedAccessView * unorderedView;

};