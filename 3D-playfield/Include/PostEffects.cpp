#include "PostEffects.h"

PostEffects::PostEffects(ID3D11Device * device, DirectX::CommonStates * states):
	bloomSelectionPass(device, WIN_WIDTH, WIN_HEIGHT),
	blurPass(device, WIN_WIDTH / 2, WIN_HEIGHT / 2),
	blurPass2(device, WIN_WIDTH / 2, WIN_HEIGHT / 2),
	bloomPicker(device, SHADER_PATH("bloomThreshold.hlsl")),
	gaussianHorizontal(device, SHADER_PATH("GaussianHorizontal.hlsl")),
	gaussianVertical(device, SHADER_PATH("GaussianVertical.hlsl")),
	bloomCombinder(device, SHADER_PATH("BloomCombinder.hlsl"))
{
	this->states = states;
}

PostEffects::~PostEffects()
{
}

void PostEffects::generateBloomMap(ID3D11DeviceContext * context, ID3D11ShaderResourceView * imageToBlur, ID3D11UnorderedAccessView * outputTexture)
{
	context->CSSetShaderResources(0, 1, &imageToBlur);
	ID3D11UnorderedAccessView * uav = bloomSelectionPass;
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	context->CSSetShader(bloomPicker, nullptr, 0);
	context->Dispatch(120, 78, 1);

	uav = blurPass;
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	ID3D11ShaderResourceView * srv = bloomSelectionPass;
	context->CSSetShaderResources(0, 1, &srv);

	context->CSSetShader(gaussianHorizontal, nullptr, 0);
	context->Dispatch(60, 34, 1);


	uav = blurPass2;
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	srv = blurPass;
	context->CSSetShaderResources(0, 1, &srv);
	context->CSSetShader(gaussianVertical, nullptr, 0);
	context->Dispatch(60, 34, 1);


	uav = outputTexture;
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

	ID3D11ShaderResourceView *srvs[] =
	{
		imageToBlur,
		blurPass2
	};
	context->CSSetShaderResources(0, 2, srvs);

	auto samplerState = states->LinearClamp();
	context->CSSetSamplers(0, 1, &samplerState);
	context->CSSetShader(bloomCombinder, nullptr, 0);
	context->Dispatch(120, 78, 1);

	////NULLING
	uav = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	srv = nullptr;
	context->CSSetShaderResources(0, 1, &srv);
	context->CSSetShaderResources(1, 1, &srv);
}
