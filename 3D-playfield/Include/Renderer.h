#pragma once
#include "Resource_Handlers\Shader.h"
#include "Resource_Handlers\Silly_Types\DepthStencil.h"
#include "Resource_Handlers\ObjLoader.h"
#include "PostEffects.h"
#include "FlashLight.h"
#include <vector>
#include <unordered_map>
#include "Camera.h"

#define MODELCOUNT 1

struct InstanceData
{
	DirectX::SimpleMath::Matrix transform;
};

struct RenderInfo
{
	MODEL_ID id;
	InstanceData data;
};


class Renderer
{
public:
	Renderer(ID3D11Device * device, ID3D11DeviceContext * context, ID3D11RenderTargetView * backBuffer);
	~Renderer();

	void addItem(RenderInfo * info);
	void render(Camera * camera, FlashLight * flashLight);
	void renderDirectlyToBackBuffer(Camera * camera, FlashLight * flashLight);
	void clear();


private:
	//Stolen from henkvik, pretty good
	typedef  std::unordered_map<MODEL_ID, std::vector<InstanceData>> InstanceQueue_t;
	std::vector<RenderInfo*> renderInfo;

	InstanceQueue_t instanceQueue;

	ID3D11Device * device;
	ID3D11DeviceContext * context;
	ID3D11RenderTargetView * backBuffer;
	RenderTargetResource middleBuffer0;
	RenderTargetResource middleBuffer1;

	DepthStencil depthStencil;

	D3D11_VIEWPORT viewport;
	DirectX::CommonStates states;

	PostEffects effects;
	ObjLoader loader;
	Shader forward;
	Shader fullScreenTriangle;


	void sortQueue();
	void writeInstanceData();
	void drawToBackBuffer(ID3D11ShaderResourceView * toBeDrawn);

};