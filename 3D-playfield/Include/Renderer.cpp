#include "Renderer.h"

#define FORWARD_DESC {\
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },\
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },\
	{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },\
	{ "TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },\
	{ "TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },\
	{ "TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },\
	{ "TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }}

Renderer::Renderer(ID3D11Device * device, ID3D11DeviceContext * context, ID3D11RenderTargetView * backBuffer) :
	loader(device),
	forward(device, L"Include/Shaders/Forward.hlsl", FORWARD_DESC),
	depthStencil(device, WIN_WIDTH, WIN_HEIGHT)
{
	this->device = device;
	this->context = context;
	this->backBuffer = backBuffer;

	viewport = {};
	viewport.Height = WIN_HEIGHT;
	viewport.Width = WIN_WIDTH;
	viewport.MaxDepth = 1.0;

}

Renderer::~Renderer()
{
	
}

void Renderer::addItem(RenderInfo * info)
{
	renderInfo.push_back(info);
}

void Renderer::render(Camera * camera)
{
	sortQueue();
	writeInstanceData();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	UINT intanceOffset = 0;
	for (InstanceQueue_t::value_type & pair : instanceQueue)
	{
		ModelInfo * model = loader.getModelInfo(pair.first);

		static UINT stride = sizeof(Vertex), instanceStride = sizeof(InstanceData), offset = 0;
		context->IASetVertexBuffers(0, 1, &model->vertexBuffer, &stride, &offset);
		context->IASetVertexBuffers(1, 1, &model->instanceBuffer, &instanceStride, &offset);

		context->VSSetShader(forward, nullptr, 0);

		ID3D11Buffer * buffers[] =
		{
			camera->getBuffer()
		};
		context->VSSetConstantBuffers(0, 1, buffers);

		context->IASetInputLayout(forward);


		context->PSSetShader(forward, nullptr, 0);
		context->OMSetRenderTargets(1, &backBuffer, depthStencil);
		context->RSSetViewports(1, &viewport);

		context->DrawInstanced(model->vertexCount, (UINT)pair.second.size(), 0, 0);
	}
}

void Renderer::clear()
{
	static float clearColor[] = {0, 0, 0, 0};

	context->ClearRenderTargetView(backBuffer, clearColor);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH, 1.f, 0.f);

}

void Renderer::sortQueue()
{
	instanceQueue.clear();

	for (RenderInfo * info : renderInfo)
	{
		instanceQueue[info->id].push_back({ info->data });
	}
	renderInfo.clear();
}

void Renderer::writeInstanceData()
{
	D3D11_MAPPED_SUBRESOURCE data = {};

	for (InstanceQueue_t::value_type & pair : instanceQueue)
	{
		context->Map(loader.getModelInfo(pair.first)->instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		void * toCopy = pair.second.data();
		size_t size = pair.second.size() * sizeof(InstanceData);
		memcpy(data.pData, toCopy, size);
		data.pData = (InstanceData*)((char*)data.pData + size);
		context->Unmap(loader.getModelInfo(pair.first)->instanceBuffer, 0);
	}

	


}
