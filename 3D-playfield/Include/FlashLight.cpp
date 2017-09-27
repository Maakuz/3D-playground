#include "FlashLight.h"
#include "ThrowIfFailed.h"
#include "Keyboard.h"
FlashLight::FlashLight(ID3D11Device * device)
{
	lightData.lightOn = true;

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(LightStruct);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	ThrowIfFailed(device->CreateBuffer(&desc, NULL, &buffer));
}

FlashLight::~FlashLight()
{
	SAFE_RELEASE(buffer);
}

void FlashLight::update(ID3D11DeviceContext * context, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 forward)
{
	lightData.dir = forward;
	lightData.pos = pos;

	//toggle


	D3D11_MAPPED_SUBRESOURCE data;

	context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

	memcpy(data.pData, &lightData, sizeof(lightData));

	context->Unmap(buffer, 0);

}
