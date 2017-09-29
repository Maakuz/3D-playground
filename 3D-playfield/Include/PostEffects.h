#pragma once
#include "Resource_Handlers\ComputeShader.h"
#include "Resource_Handlers\Silly_Types\RenderTargetResource.h"
#include "Contants.h"

#define SHADER_PATH(path) L"Include/Shaders/" path

class PostEffects
{
public:
	PostEffects(ID3D11Device * device, DirectX::CommonStates * states);
	~PostEffects();

	void generateBloomMap(ID3D11DeviceContext * context, ID3D11ShaderResourceView * imageToBlur, ID3D11UnorderedAccessView * outputTexture);

private:
	ComputeShader bloomPicker;
	ComputeShader gaussianHorizontal;
	ComputeShader gaussianVertical;
	ComputeShader bloomCombinder;

	RenderTargetResource bloomSelectionPass;
	RenderTargetResource blurPass;
	RenderTargetResource blurPass2;

	DirectX::CommonStates * states;
};