#pragma once
#include <d3d11.h>
#include "SimpleMath.h"
#include "../ThrowIfFailed.h"
#include <unordered_map>
#define MAX_INSTANCES 3000

enum MODEL_ID
{
	CUBE = 0,
	BARREL = 1
};

struct Vertex
{
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 normal;
	DirectX::SimpleMath::Vector2 uv;
};

struct ModelInfo
{
	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * instanceBuffer;
	ID3D11ShaderResourceView * diffuseTexture;
	ID3D11ShaderResourceView * normalTexture;
	int vertexCount;
};

class ObjLoader
{
public:
	std::unordered_map<MODEL_ID, ModelInfo> models;

	ObjLoader(ID3D11Device * device);
	~ObjLoader();

	ModelInfo * getModelInfo(MODEL_ID id);

private:
	ModelInfo loadObject(ID3D11Device * device, char* path, bool objIsLeft = false);


};