#include "Window.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);

		break;

	case WM_ACTIVATEAPP:
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(msg, wparam, lparam);
		break;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}

	return 0;
}

Window::Window(HINSTANCE hInstance, int width, int height)
{
	this->mHeight = height;
	this->mWidth = width;
	this->hInstance = hInstance;
	this->initializeWindow();

	this->isFullscreen = false;
	this->mKeyboard = std::make_unique<DirectX::Keyboard>();
	this->mMouse = std::make_unique<DirectX::Mouse>();
	this->mMouse->SetWindow(window);
}

Window::~Window()
{
	delete renderer;
	delete camera;

	this->mSwapChain->Release();
	this->device->Release();
	this->context->Release();
	this->backBufferRTV->Release();

	//Enable this to get additional information about live objects
	//this->mDebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	mDebugDevice->Release();
}

int Window::run()
{
	MSG msg = { 0 };
	long long start = this->timer();
	long long prev = start;
	long long currentTime = 0;
	long long deltaTime = 0;
	long long totalTime = 0;

	this->createSwapChain();
	renderer = new Renderer(device, context, backBufferRTV);
	camera = new Camera(device, WIN_WIDTH, WIN_HEIGHT, 300);
	FlashLight flashLight(device);

	camera->setPos(DirectX::SimpleMath::Vector3(0, 1, 3));
	camera->setForward(DirectX::SimpleMath::Vector3(0, 0, -1));


	while (WM_QUIT != msg.message)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		currentTime = this->timer();
		deltaTime = currentTime - prev;
		prev = currentTime;
		totalTime += deltaTime;

		auto ks = this->mKeyboard->GetState();

		if (ks.Escape)
			PostQuitMessage(0);
		///////
		static float t = 0;
		t += 0.0001;
		static RenderInfo test =
		{
			MODEL_ID::BARREL,
			DirectX::SimpleMath::Matrix::CreateRotationZ(t)
		};

		test.data.transform = DirectX::SimpleMath::Matrix::CreateRotationZ(t) * DirectX::SimpleMath::Matrix::CreateTranslation({ 0, 1, 0 });

		static RenderInfo floor =
		{
			MODEL_ID::CUBE,
			DirectX::SimpleMath::Matrix::CreateScale(1000, 0.1, 1000) * DirectX::SimpleMath::Matrix::CreateTranslation({0, -1, 0})
		};

		renderer->addItem(&test);
		renderer->addItem(&floor);

		camera->update(context);
		flashLight.update(context, camera->getPos(), camera->getForward());

		renderer->clear();
		renderer->render(camera, &flashLight);
		mSwapChain->Present(0, 0);
	}


	return 0;
}

void Window::initializeWindow()
{
	WNDCLASS wc = { 0 };

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = this->hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = L"Basic test";




	if (!RegisterClass(&wc))
	{
		MessageBox(this->window, L"registerClass failed", L"Error", MB_OK);
	}

	RECT rc = { 0, 0, mWidth, mHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	this->window = CreateWindow(
		L"Basic test",
		L"Stort spel",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		0,
		0,
		this->hInstance,
		0);

	if (this->window == 0)
	{
		MessageBox(this->window, L"window creation failed", L"Error", MB_OK);
	}

	SetWindowPos(GetConsoleWindow(), 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(this->window, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	ShowWindow(this->window, SW_SHOWDEFAULT);
	UpdateWindow(this->window);
}

HRESULT Window::createSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	desc.BufferCount = 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = this->window;
	desc.SampleDesc.Count = 1;
	desc.Windowed = true;
	desc.BufferDesc.Height = this->mHeight;
	desc.BufferDesc.Width = this->mWidth;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;



	ThrowIfFailed(D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&desc,
		&mSwapChain,
		&this->device,
		NULL,
		&this->context));


	ID3D11Texture2D* backBuffer = nullptr;
	ThrowIfFailed(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer));


	ThrowIfFailed(device->CreateRenderTargetView(backBuffer, NULL, &backBufferRTV));

	backBuffer->Release();

	//Creates a debug device to check for memory leaks etc
	ThrowIfFailed(this->device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast <void **>(&mDebugDevice)));

	return true;
}

long long Window::timer()
{
	static LARGE_INTEGER frequency;
	static BOOL highResCounter = QueryPerformanceFrequency(&frequency);
	if (highResCounter)
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		//returns time in ms
		return(1000LL * now.QuadPart) / frequency.QuadPart;
	}
	else
	{
		return GetTickCount();
	}
}
