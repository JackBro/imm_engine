////////////////
// simple_secene
////////////////
////////////////
#ifndef UNICODE
#define UNICODE
#endif
#include "imm_app.cc"
#include "control_state.cc"
#include "control_math.cc"
//
#pragma comment(lib, "User32")
#pragma comment(lib, "Effects11")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "DirectXTK")
#pragma comment(lib, "D2d1")
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "lua")
#pragma comment(lib, "Xinput1_3")
#pragma comment(lib, "DirectXTKAudioDX")
////////////////
//
////////////////
////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	DUMMY(pCmdLine);
	DUMMY(hInstance);
	imm::imm_app win;
	if (!win.init_win()) return 0;
	if (!win.init_d3d()) return 0;
	ShowWindow(win.get_hwnd(), nCmdShow);
	UpdateWindow(win.get_hwnd());
	if (!win.init_imm()) return 0;
	return win.run();
}
