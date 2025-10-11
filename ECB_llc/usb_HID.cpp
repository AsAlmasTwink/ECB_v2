#include "pch.h"

#define WNDCLASSNAME "ECB_llc_wclass"

struct DEVPARAMS {
	char* PID;
	char* VID;
};

DEVPARAMS* result = 0;

LRESULT CALLBACK WinHandler(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_CREATE: {
		HWND x;
		x = CreateWindowA(WC_BUTTONA, "ŒÚÏÂÌ‡", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 150, 150, 40, wnd, (HMENU)IDCANCEL, 0, 0);
		if (!x)
			PostQuitMessage(0);
		x = CreateWindowA(WC_STATICA, "Œ∆»ƒ¿Õ»≈ œŒƒ Àﬁ◊≈Õ»ﬂ ”—“–Œ…—“¬¿...", WS_CHILD | SS_CENTER | WS_VISIBLE, 10, 10, 380, 40, wnd, 0, 0, 0);
		// change font of WC_STATICA...
		x = CreateWindowA(PROGRESS_CLASSA, 0, WS_CHILD | PBS_MARQUEE | WS_VISIBLE, 10, 100, 380, 40, wnd, (HMENU)1001, 0, 0);
		if (!x)
			PostQuitMessage(0);
		RAWINPUTDEVICE rwDC = { 0 };
		rwDC.usUsage = 0x0001;
		rwDC.usUsagePage = 0x0002 | 0x0006;
		rwDC.dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY;
		rwDC.hwndTarget = wnd;
	}break;
	case WM_COMMAND: {
		int cmID = LOWORD(wParam);
		switch (cmID) {
		case IDCANCEL: {
			if (result != 0)
			{
				free(result);
				result = 0;
			}
			SendMessageA(wnd, WM_CLOSE, 0, 0);
		}break;
		}
	}break;
	case WM_CLOSE: { PostQuitMessage(0); }break;
	}
	return DefWindowProcA(wnd, msg, wParam, lParam);
}

extern "C" cdecl void* GetConnectedDevice(int waitTimeInSec) {
	WNDCLASSEXA wndClass = { 0 };
	wndClass.cbSize = sizeof(wndClass);
	wndClass.cbClsExtra = 0;
	wndClass.lpfnWndProc = WinHandler;
	wndClass.lpszClassName = WNDCLASSNAME;
	wndClass.hInstance = GetModuleHandleA(0);
	wndClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
	wndClass.hIcon = (HICON)LoadIconA(0, MAKEINTRESOURCEA(IDI_EXCLAMATION));
	wndClass.hCursor = (HCURSOR)LoadCursorA(0, MAKEINTRESOURCEA(IDC_ARROW));
	wndClass.hIconSm = wndClass.hIcon;
	
	if (RegisterClassExA(&wndClass) == 0)
		return 0;

	HWND wnd = CreateWindowExA(
		WS_EX_TOPMOST, WNDCLASSNAME, "Œ∆»ƒ¿Õ»≈ œŒƒ Àﬁ◊≈Õ»ﬂ ”—“–Œ…—“¬¿", WS_VISIBLE | WS_POPUPWINDOW,
		0, 0, 400, 200, 0, 0, wndClass.hInstance, 0
	);

	if (wnd == 0)
		return 0;
	ShowWindow(wnd, SW_SHOW);
	int X, Y;
	X = (GetSystemMetrics(SM_CXSCREEN) / 2) - 100;
	Y = (GetSystemMetrics(SM_CYSCREEN) / 2) - 200;
	SetWindowPos(wnd, HWND_TOPMOST, X, Y, 400, 200, SWP_NOSIZE);
	MSG msg;
	while (GetMessageA(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return 0;
}

extern "C" cdecl char* GetDevicePID(void* device) {
	return 0;
}

extern "C" cdecl char* GetDeviceVID(void* device) {
	return 0;
}