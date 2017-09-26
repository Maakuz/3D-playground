#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "ThrowIfFailed.h"
#include "Keyboard.h"

#pragma comment (lib, "d3d11.lib")

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080

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

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mContext;
	ID3D11Debug* mDebugDevice;
	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mBackBufferRTV;
	std::unique_ptr<DirectX::Keyboard> mKeyboard;
	//std::unique_ptr<DirectX::Mouse> mMouse;
	bool isFullscreen;

	void initializeWindow();
	HRESULT createSwapChain();
	long long timer();
};