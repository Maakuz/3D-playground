#pragma once
#include <Windows.h>
#include "Renderer.h"

#pragma comment (lib, "d3d11.lib")

class Window
{
public:
	Window(HINSTANCE hInstance, int width, int height);
	virtual ~Window();

	int run();

private:
	HWND window;
	int mWidth;
	int mHeight;
	HINSTANCE hInstance;

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Debug* mDebugDevice;
	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* backBufferRTV;
	std::unique_ptr<DirectX::Keyboard> mKeyboard;
	std::unique_ptr<DirectX::Mouse> mMouse;
	bool isFullscreen;

	Renderer* renderer;
	Camera * camera;

	void initializeWindow();
	HRESULT createSwapChain();
	long long timer();
};