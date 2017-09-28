#include "UnorderedAccessResource.h"

UnorderedAccessResource::UnorderedAccessResource(ID3D11Device * device, UINT width, UINT height)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;

	ID3D11Texture2D* texture;
	ThrowIfFailed(device->CreateTexture2D(&textureDesc, NULL, &texture));

	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedDesc = {};
	unorderedDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	unorderedDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	ThrowIfFailed(device->CreateUnorderedAccessView(texture, &unorderedDesc, &unorderedView));

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc = {};
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceDesc.Texture2D.MipLevels = 1;

	ThrowIfFailed(device->CreateShaderResourceView(texture, &resourceDesc, &shaderResourceView));

	texture->Release();
}

UnorderedAccessResource::~UnorderedAccessResource()
{
	this->shaderResourceView->Release();
	this->unorderedView->Release();
}
