#pragma once
#include "Resource_Handlers\Shader.h"
#include "Resource_Handlers\DepthStencil.h"
#include "Resource_Handlers\ObjLoader.h"
#include "FlashLight.h"
#include <vector>
#include <unordered_map>
#include "Camera.h"

#define MODELCOUNT 1

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080



struct InstanceData
{
	DirectX::SimpleMath::Matrix transform;
};

struct RenderInfo
{
	MODEL_ID id;
	InstanceData data;
};


#include "CommonStates.h"
class Renderer
{
public:
	Renderer(ID3D11Device * device, ID3D11DeviceContext * context, ID3D11RenderTargetView * backBuffer);
	~Renderer();

	void addItem(RenderInfo * info);
	void render(Camera * camera, FlashLight * flashLight);
	void clear();


private:
	//Stolen from henkvik, pretty good
	typedef  std::unordered_map<MODEL_ID, std::vector<InstanceData>> InstanceQueue_t;
	std::vector<RenderInfo*> renderInfo;

	InstanceQueue_t instanceQueue;

	ID3D11Device * device;
	ID3D11DeviceContext * context;
	ID3D11RenderTargetView * backBuffer;

	DepthStencil depthStencil;

	D3D11_VIEWPORT viewport;
	std::unique_ptr<DirectX::CommonStates> states;

	ObjLoader loader;
	Shader forward;

	void sortQueue();
	void writeInstanceData();

};