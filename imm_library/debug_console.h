////////////////
// debug_console.h
// echo info with a console window, test and debug for app
// endrollex
////////////////
////////////////
#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <clocale>
#include <DirectXMath.h>
using namespace DirectX;
namespace imm
{
////////////////
// Adding Console I/O to a Win32 GUI App
// Windows Developer Journal, December 1997
////////////////
////////////////
// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;
bool RedirectIOToConsole()
{
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	// allocate a console for this app
	AllocConsole();
	SetConsoleTitle(L"imm Console");
	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "r");
	*stdin = *fp;
	setvbuf(stdin, NULL, _IONBF, 0);
	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stderr = *fp;
	setvbuf(stderr, NULL, _IONBF, 0);
	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	std::ios::sync_with_stdio();
	return true;
}
////////////////
// echo type cast
////////////////
////////////////
std::ostream &operator<<(std::ostream &os, const XMFLOAT2 dest)
{
	os << "(" << dest.x << ", " << dest.y << ")";
	return os;
}
//
std::ostream &operator<<(std::ostream &os, const XMFLOAT3 dest)
{
	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ")";
	return os;
}
//
std::ostream &operator<<(std::ostream &os, const XMFLOAT4 dest)
{
	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ", " << dest.w << ")";
	return os;
}
//
std::ostream& operator<<(std::ostream &os, const XMFLOAT4X4 m)
{
	os << "####\n";
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) os << m(i, j) << ' ';
		if (i < 3) os << '\n';
		else os << "####" << std::flush;
	}
	return os;
}
//
std::ostream& operator<<(std::ostream &os, CXMVECTOR v)
{
	XMFLOAT4 dest;
	XMStoreFloat4(&dest, v);
	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ", " << dest.w << ")";
	return os;
}
//
std::ostream& operator<<(std::ostream &os, CXMMATRIX m)
{
	os << "####\n";
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) os << m.r[i].m128_f32[j] << ' ';
		if (i < 3) os << '\n';
		else os << "####" << std::flush;
	}
	return os;
}
////////////////
// echo_init
////////////////
////////////////
bool DEBUG_IS_CONSOLE = false;
void echo_init()
{
	if (!DEBUG_IS_CONSOLE) {
		RedirectIOToConsole();
		// for prints chinese
		std::setlocale(LC_ALL, "");
		DEBUG_IS_CONSOLE = true;
	}
}
////////////////
// echo function
////////////////
////////////////
template <typename T>
void echo(const T &info)
{
	echo_init();
	std::cerr << info << std::endl;
}
//
template <typename T>
void echo_wstr(const T &info)
{
	echo_init();
	std::wostringstream outs;
	outs << info;
	std::wcerr << outs.str() << std::endl;
}
//
void echo_wostr(std::wostringstream &info)
{
	echo_init();
	std::wcerr << info.str() << std::endl;
}
//
template <typename T>
void echo_message(const T &info)
{
	std::wostringstream outs;
	outs << info;
	MessageBox(0, outs.str().c_str(), L"Message", 0);}
}
#endif