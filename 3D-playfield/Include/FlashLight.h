#pragma once
#include <d3d11.h>
#include "SimpleMath.h"
class FlashLight
{
	struct LightStruct
	{
		DirectX::SimpleMath::Vector4 pos;
		DirectX::SimpleMath::Vector4 dir;
		int lightOn;
		DirectX::SimpleMath::Vector3 pad;
	};


public:
	FlashLight(ID3D11Device * device);
	~FlashLight();

	void update(ID3D11DeviceContext * context, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 forward);
	ID3D11Buffer * getBuffer() { return buffer; }

private:

	ID3D11Buffer * buffer;
	LightStruct lightData;

};