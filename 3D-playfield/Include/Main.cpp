
#include "Window.h"
#include <crtdbg.h>

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	

	Window window(hInstance, WIN_WIDTH, WIN_HEIGHT);

	return window.run();
}