#include "Renderer.h"
#include "Keyboard.h"

Renderer::Renderer(ID3D11Device * device, ID3D11DeviceContext * context, ID3D11RenderTargetView * backBuffer) :
	loader(device),
	forward(device, SHADER_PATH("Forward.hlsl"), FORWARD_DESC, true),
	depthStencil(device, WIN_WIDTH, WIN_HEIGHT),
	middleBuffer0(device, WIN_WIDTH, WIN_HEIGHT),
	middleBuffer1(device, WIN_WIDTH, WIN_HEIGHT),
	fullScreenTriangle(device, SHADER_PATH("FullScreenTriangle.hlsl"), { { "POSITION", 0, DXGI_FORMAT_R8_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } }),
	effects(device, &states),
	states(device)
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

void Renderer::render(Camera * camera, FlashLight * flashLight)
{
	sortQueue();
	writeInstanceData();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	context->VSSetShader(forward, nullptr, 0);
	context->PSSetShader(forward, nullptr, 0);
	context->GSSetShader(forward, nullptr, 0);

	auto samplerState = states.PointClamp();
	context->PSSetSamplers(0, 1, &samplerState);

	context->RSSetViewports(1, &viewport);

	ID3D11Buffer * camBuf = camera->getBuffer();

	context->VSSetConstantBuffers(0, 1, &camBuf);
	context->PSSetConstantBuffers(0, 1, &camBuf);

	ID3D11Buffer * psBuf = flashLight->getBuffer();

	context->PSSetConstantBuffers(1, 1, &psBuf);

	context->IASetInputLayout(forward);

	ID3D11RenderTargetView * rtv = middleBuffer0;

	context->OMSetRenderTargets(1, &rtv, depthStencil);

	UINT intanceOffset = 0;
	for (InstanceQueue_t::value_type & pair : instanceQueue)
	{
		ModelInfo * model = loader.getModelInfo(pair.first);

		static UINT stride = sizeof(Vertex), instanceStride = sizeof(InstanceData), offset = 0;
		context->IASetVertexBuffers(0, 1, &model->vertexBuffer, &stride, &offset);
		context->IASetVertexBuffers(1, 1, &model->instanceBuffer, &instanceStride, &offset);

		

		ID3D11ShaderResourceView * srvs[] =
		{
			model->diffuseTexture,
			model->normalTexture,
			model->specularTexture
		};

		context->PSSetShaderResources(0, 3, srvs);

		context->DrawInstanced(model->vertexCount, (UINT)pair.second.size(), 0, 0);
	}

	rtv = nullptr;
	context->OMSetRenderTargets(1, &rtv, depthStencil);
	context->GSSetShader(nullptr, nullptr, 0);

	//////POSTEFFECTS///////

	//temp
	auto ks = DirectX::Keyboard::Get().GetState();

	if (!ks.D1)
	{
		effects.generateBloomMap(context, middleBuffer0, middleBuffer1);
		drawToBackBuffer(middleBuffer1);
	}

	else
		drawToBackBuffer(middleBuffer0);
}


///DDONT DO ANY POST EFFECXTS HERE BY MISTAKE I WILL KILL AAAHFG
void Renderer::renderDirectlyToBackBuffer(Camera * camera, FlashLight * flashLight)
{
	sortQueue();
	writeInstanceData();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(forward, nullptr, 0);
	context->PSSetShader(forward, nullptr, 0);
	context->GSSetShader(forward, nullptr, 0);

	auto samplerState = states.PointClamp();
	context->PSSetSamplers(0, 1, &samplerState);

	context->RSSetViewports(1, &viewport);

	ID3D11Buffer * camBuf = camera->getBuffer();

	context->VSSetConstantBuffers(0, 1, &camBuf);
	context->PSSetConstantBuffers(0, 1, &camBuf);

	ID3D11Buffer * psBuf = flashLight->getBuffer();

	context->PSSetConstantBuffers(1, 1, &psBuf);

	context->IASetInputLayout(forward);

	context->OMSetRenderTargets(1, &backBuffer, depthStencil);

	UINT intanceOffset = 0;
	for (InstanceQueue_t::value_type & pair : instanceQueue)
	{
		ModelInfo * model = loader.getModelInfo(pair.first);

		static UINT stride = sizeof(Vertex), instanceStride = sizeof(InstanceData), offset = 0;
		context->IASetVertexBuffers(0, 1, &model->vertexBuffer, &stride, &offset);
		context->IASetVertexBuffers(1, 1, &model->instanceBuffer, &instanceStride, &offset);



		ID3D11ShaderResourceView * srvs[] =
		{
			model->diffuseTexture,
			model->normalTexture,
			model->specularTexture
		};

		context->PSSetShaderResources(0, 3, srvs);

		context->DrawInstanced(model->vertexCount, (UINT)pair.second.size(), 0, 0);
	}
}

void Renderer::clear()
{
	static float clearColor[] = {0, 0, 0, 0};

	context->ClearRenderTargetView(backBuffer, clearColor);
	context->ClearRenderTargetView(middleBuffer0, clearColor);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH, 1.f, 0);

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

void Renderer::drawToBackBuffer(ID3D11ShaderResourceView * toBeDrawn)
{
	context->IASetInputLayout(fullScreenTriangle);
	context->VSSetShader(fullScreenTriangle, nullptr, 0);
	context->PSSetShader(fullScreenTriangle, nullptr, 0);

	context->OMSetRenderTargets(1, &backBuffer, nullptr);
	auto samplerState = states.PointWrap();
	context->PSSetSamplers(0, 1, &samplerState);
	context->PSSetShaderResources(0, 1, &toBeDrawn);

	context->Draw(3, 0);

	ID3D11ShaderResourceView * srvNull = nullptr;
	context->PSSetShaderResources(0, 1, &srvNull);
}
