#pragma once
#include "Shapes.h"
#include "Shader.h"
#include <vector>
#include <unordered_map>
#include "Camera.h"

#define MODELCOUNT 1
#define MAX_INSTANCES 3000

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080

enum MODEL_ID
{
	CUBE = 0
};

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
	void render(Camera * camera);
	void clear();


private:
	//Stolen from henkvik, pretty good
	typedef  std::unordered_map<MODEL_ID, std::vector<InstanceData>> InstanceQueue_t;
	std::vector<RenderInfo*> renderInfo;

	InstanceQueue_t instanceQueue;

	ID3D11Device * device;
	ID3D11DeviceContext * context;
	ID3D11RenderTargetView * backBuffer;
	ID3D11Buffer * instanceBuffer;

	D3D11_VIEWPORT viewport;

	Cube cube;
	Shader forward;

	void sortQueue();
	void writeInstanceData();

};