#include "pch.h"
#include <Dbt.h>
#include <string>
#include <algorithm>
#define WNDCLASSNAME "ECB_llc_wclass"
#define WM_TIMEOUT	WM_USER+5
struct DEVPARAMS {
	USHORT pid = 0, vid = 0;
};

DEVPARAMS* result = 0;


constexpr auto pid = "PID_";
constexpr auto vid = "VID_";

bool ExtractHexValue(const std::string& str, const std::string& prefix, USHORT* value) {
	size_t pos = str.find(prefix);
	if (pos == std::string::npos) {
		return false;
	}

	pos += prefix.length(); // Перемещаемся после префикса

	// Ищем конец hex значения (до '&' или конца строки)

	// Извлекаем подстроку с hex значением
	std::string hex_str = str.substr(pos, str.length());

	// Парсим hex значение
	return sscanf_s(hex_str.c_str(), "%4hx", value) == 1;
}

bool LoadPIDandVID(char* devName) {
	result = new DEVPARAMS;
	if (!result)
		return false;

	std::string str = devName;
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
		return std::toupper(c);
		});
	if (!ExtractHexValue(str, pid, &result->pid) || !ExtractHexValue(str, vid, &result->vid)) {
		delete result;
		result = 0;
		return false;
	}
	return true;
}

int IDevice = 0;
LRESULT CALLBACK WinHandler(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_CREATE: {
		HWND x;
		x = CreateWindowA(WC_BUTTONA, "Отмена", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 150, 150, 40, wnd, (HMENU)IDCANCEL, 0, 0);
		if (!x)
			PostQuitMessage(0);
		x = CreateWindowA(WC_STATICA, "ОЖИДАНИЕ ПОДКЛЮЧЕНИЯ УСТРОЙСТВА...", WS_CHILD | SS_CENTER | WS_VISIBLE, 10, 10, 380, 40, wnd, 0, 0, 0);
		// change font of WC_STATICA...
		x = CreateWindowA(PROGRESS_CLASSA, 0, WS_CHILD | PBS_MARQUEE | WS_VISIBLE, 10, 100, 380, 40, wnd, (HMENU)1001, 0, 0);
		if (!x)
			PostQuitMessage(0);
	}break;
	case WM_INPUT_DEVICE_CHANGE: {
		if (wParam == GIDC_ARRIVAL) {
			HANDLE hDevice = (HANDLE)lParam;
			RID_DEVICE_INFO devInfo = { 0 };
			UINT infoSize = sizeof(devInfo);
			if (GetRawInputDeviceInfoA(hDevice, RIDI_DEVICEINFO, &devInfo, &infoSize) == -1) {
				int mbr = MessageBoxA(0, "ОШИБКА: возможно вы не обладаете необходимыми правами доступа на этом устройстве, пожалуйста обратитесь к администратору!", "ОШИБКА МОДУЛЯ", MB_ICONERROR | MB_RETRYCANCEL);
				PostQuitMessage(0);
				if (mbr == IDRETRY) {
					MessageBoxA(0, "ДАННАЯ ФУНКЦИЯ ЕЩЁ НЕ РЕАЛИЗОВАНА!", "ОШИБКА МОДУЛЯ", MB_ICONERROR | MB_OK);
				}
				return DefWindowProcA(wnd, msg, wParam, lParam);
			}
			switch (devInfo.dwType) {
			case RIM_TYPEKEYBOARD: 
			{
				IDevice++;
				if (IDevice <= 1)
					return DefWindowProcA(wnd, msg, wParam, lParam);
				GetRawInputDeviceInfoA(hDevice, RIDI_DEVICENAME, 0, &infoSize);
				
				char* DevName = new char[infoSize + 1];
				ZeroMemory(DevName, infoSize);
				if (GetRawInputDeviceInfoA(hDevice, RIDI_DEVICENAME, DevName, &infoSize) != -1) {
					std::string str = "ID устройства: ";
					str += DevName;
					str += "\r\nЭто оно?";
					infoSize = MessageBoxA(wnd, str.c_str(), "УСТРОЙСТВО НАЙДЕНО", MB_ICONINFORMATION | MB_YESNO);
					if (infoSize == IDYES) {
						if (LoadPIDandVID(DevName)) {

						}
					}
				}
				
			}break;
			case RIM_TYPEHID: {
				MessageBoxA(wnd, "HID device is connected! Ещё не реализовано!", "INFO", MB_ICONINFORMATION | MB_OK);
			}break;
			}
		}
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

void CALLBACK TimeOut(HWND wnd, UINT o, UINT_PTR t, DWORD f) {

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
		WS_EX_TOPMOST, WNDCLASSNAME, "ОЖИДАНИЕ ПОДКЛЮЧЕНИЯ УСТРОЙСТВА", WS_VISIBLE | WS_POPUPWINDOW,
		0, 0, 400, 200, 0, 0, wndClass.hInstance, 0
	);

	if (wnd == 0)
		return 0;
	IDevice = 0;

	ShowWindow(wnd, SW_SHOW);
	int X, Y;
	X = (GetSystemMetrics(SM_CXSCREEN) / 2) - 100;
	Y = (GetSystemMetrics(SM_CYSCREEN) / 2) - 200;
	SetWindowPos(wnd, HWND_TOPMOST, X, Y, 400, 200, SWP_NOSIZE);

	RAWINPUTDEVICE rwDC[2];
	rwDC[0].usUsagePage = 0x0001;
	rwDC[0].usUsage = 0x0006;
	rwDC[0].dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY;
	rwDC[0].hwndTarget = wnd;

	rwDC[1].usUsagePage = 0x008C;
	rwDC[1].usUsage = 0x0002;
	rwDC[1].dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY;
	rwDC[1].hwndTarget = wnd;

	if (!RegisterRawInputDevices(rwDC, 2, sizeof(RAWINPUTDEVICE)))
	{
		int mbr = GetLastError();
		MessageBoxA(wnd, "ОШИБКА: возможно вы не обладаете необходимыми правами доступа на этом устройстве, пожалуйста обратитесь к администратору!", "ОШИБКА МОДУЛЯ", MB_ICONERROR);
		DestroyWindow(wnd);
		return 0;
	}
	
	if (SetTimer(wnd, WM_TIMEOUT, min(USER_TIMER_MAXIMUM, max(USER_TIMER_MINIMUM, waitTimeInSec)), TimeOut) == 0)
		return 0;
	MSG msg;
	while (GetMessageA(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	if(result != 0)
		return result;
}

extern "C" cdecl void FreeDevice(void* device) {
	if (device == 0)
		return;
	delete device;
}

extern "C" cdecl USHORT GetDevicePID(void* device) {
	if (device == 0)
		return 0;
	DEVPARAMS* x = (DEVPARAMS*)device;
	return x->pid;
}

extern "C" cdecl USHORT GetDeviceVID(void* device) {
	if (device == 0)
		return 0;
	DEVPARAMS* x = (DEVPARAMS*)device;
	return x->vid;
}