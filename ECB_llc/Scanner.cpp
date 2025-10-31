#include "pch.h"
#include "usb_HID.h"

#include <vector>
#include <SetupAPI.h>
#include <hidsdi.h>
#include <winusb.h>
#include <exception>

#define SCANCLASS "SCANllcwnd"
#define CreateID(pid, vid) ((pid<<16) | vid)

#pragma comment(lib, "SetupAPI.lib")
#pragma comment(lib, "Hid.lib")


struct DEVPARAMS {
	USHORT pid = 0, vid = 0;
};
typedef void (*SC_OBJ_READCBF)(UINT status, char* data);

LRESULT CALLBACK ScanWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool LoadPIDandVID(char* devName, __out DEVPARAMS* result);

class ScannerObject
{
public:
	ScannerObject(char*pid, char*vid) : pid(pid), vid(vid) {
		if (pid == 0 || vid == 0)
			throw new std::exception("PID or VID of device is incorrect!");
		
		this->ID = CreateID(std::strtoul(pid, 0, 16), std::strtoul(vid, 0, 16));
	}

	bool CheckExist() {
		GUID hidGUID;
		HidD_GetHidGuid(&hidGUID);
		HDEVINFO hDevInfo = SetupDiGetClassDevsA(&hidGUID, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
		if (hDevInfo == INVALID_HANDLE_VALUE) {
			return false;
		}

		SP_DEVINFO_DATA DevInfoData;
		ZeroMemory(&DevInfoData, sizeof(SP_DEVINFO_DATA));
		DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		DWORD DevI = 0;

		while (SetupDiEnumDeviceInfo(hDevInfo, DevI, &DevInfoData)) {
			DevI++;
			DWORD sz;
			SetupDiGetDeviceInstanceIdA(hDevInfo, &DevInfoData, 0, 0, &sz);
			char* DevID = new char[sz];
			ZeroMemory(DevID, sz);
			if (!DevID) {
				SetupDiDestroyDeviceInfoList(hDevInfo);
				throw new std::exception("Memory error: 'ECB_llc.dll'");
				return false;
			}
			if (!SetupDiGetDeviceInstanceIdA(hDevInfo, &DevInfoData, DevID, sz, &sz)) {
				delete[] DevID;
				continue;
			}

			DEVPARAMS* cur = new DEVPARAMS;
			if (!cur)
			{
				delete[] DevID;
				SetupDiDestroyDeviceInfoList(hDevInfo);
				throw new std::exception("Memory error: 'ECB_llc.dll'");
				return false;
			}
			if (!LoadPIDandVID(DevID, cur)) {
				delete[] DevID;
				delete cur;
				continue;
			}
			delete[] DevID;
			if (this->ID == CreateID(cur->pid, cur->vid)) {
				delete cur;
				SetupDiDestroyDeviceInfoList(hDevInfo);
				return true;
			}
		}

		return false;
	}

	void AddKey(char key){
		switch (key) {
		case '\r':
		case '\n':
		case '\0':
			this->RegistrateReadedData((char*)CurrInput.c_str());
			CurrInput.clear();
			return;
		}
		CurrInput += key;
	}

	void SetDataCallbackFunc(SC_OBJ_READCBF rcb) {
		if (this->ReadCallback != 0)
			this->ReadCallback(this->RCB_CHANGE, rcb == 0 ? (char*)"HANDLE DISCONNECT" : (char*)"HANDLE CHANGE");
		this->ReadCallback = rcb;
	}

	UINT64 GetID() {
		return ID;
	}

	~ScannerObject() {
	}

private:
	void RegistrateReadedData(char* data) {
		if (data == 0 || this->ReadCallback == 0)
			return;
		size_t dLength = lstrlenA(data);
		if (dLength > 1024 || dLength <= 4)
			return;
		dLength++;
		char* buffer = new char[dLength];
		if (buffer == 0)
			return;
		ZeroMemory(buffer, dLength);
		memcpy(buffer, data, dLength--);
		this->ReadCallback(this->DATA, buffer);
		delete[] buffer;
	}
	enum STATUS_MSG {
		DATA,
		RCB_CHANGE,
		SC_DISCONNECT,
		SC_APP_FREE,
		SC_CONNECT
	};
	std::string CurrInput;
	std::string pid, vid;
	char* lastBuf = 0;
	SC_OBJ_READCBF ReadCallback = 0;
	UINT64 ID;
};

static ScannerObject* scannerObjects[256] = { 0 };
bool IsCanToInteractWithScanner = false;
HWND ScanWnd = 0;
HANDLE ScanThread = 0;

DWORD WINAPI ScanThreadProc(LPVOID lpParam) {
	HWND wnd = *((HWND*)lpParam);
	MSG msg;
	while (GetMessageA(&msg, wnd, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	return 0;
}

size_t GetHash(UINT64 id) {
	return (id % 256);
}

extern "C" UINT64 CreateScannerObject(char* pid, char* vid) {
	IsCanToInteractWithScanner = false;

	ScannerObject* scanner = new ScannerObject(pid, vid);
	if(!scanner->CheckExist()) {
		delete scanner;
		IsCanToInteractWithScanner = true;
		MessageBoxA(0, "ОШИБКА: недостаточно прав или устройство не найдено", "ОШИБКА МОДУЛЯ", MB_ICONERROR | MB_OK);
		return 0xffffffffffffffff;
	}
	size_t hash = GetHash(scanner->GetID());
	if(scannerObjects[hash] != 0)
		delete scannerObjects[hash];
	scannerObjects[hash] = scanner;
	UINT64 id = scanner->GetID();

	IsCanToInteractWithScanner = true;
	return id;
}

extern "C" void SetScannerReadCallback(UINT64 id, SC_OBJ_READCBF func) {
	IsCanToInteractWithScanner = false;
	size_t i = GetHash(id);
	if (scannerObjects[i] == 0)
	{
		IsCanToInteractWithScanner = true;
		return;
	}
	scannerObjects[i]->SetDataCallbackFunc(func);
	IsCanToInteractWithScanner = true;
}

extern "C" bool RegistrateObjectsReader() {
	if(ScanWnd != 0)
		return true;
	WNDCLASSEXA wc = { 0 };
	wc.lpszClassName = SCANCLASS;
	wc.cbSize = sizeof(WNDCLASSEXA);
	wc.lpfnWndProc = ScanWndProc;
	wc.hInstance = GetModuleHandleA(0);
	if (!RegisterClassExA(&wc))
		return false;
	ScanWnd = CreateWindowA(SCANCLASS, SCANCLASS, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, GetModuleHandleA(0), 0);
	if(ScanWnd == 0)
		return false;
	if (!RegistrateInput(ScanWnd))
	{
		DestroyWindow(ScanWnd);
		ScanWnd = 0;
		return false;
	}
	return true;
}

extern "C" bool StartupScannersReader() {
	if (ScanWnd == 0)
		return false;
	if(ScanThread != 0)
		return true;
	IsCanToInteractWithScanner = true;
	ScanThread = CreateThread(0, 0, ScanThreadProc, (LPVOID)&ScanWnd, 0, 0);
	return true;
}

extern "C" void UnregistrateObjectsReader() {
	if (ScanWnd == 0)
		return;
	IsCanToInteractWithScanner = false;
	DestroyWindow(ScanWnd);
	ScanWnd = 0;
	WaitForSingleObject(ScanThread, 2000);
	CloseHandle(ScanThread);
	ScanThread = 0;
}

extern "C" void DeleteScannerObject(UINT64 id) {
	IsCanToInteractWithScanner = false;
	size_t hash = GetHash(id);
	if (scannerObjects[hash] != 0) {
		delete scannerObjects[hash];
		scannerObjects[hash] = 0;
	}
	IsCanToInteractWithScanner = true;
}


bool LoadPIDandVID(char* devName, __out DEVPARAMS* result) {
	if (!result)
		return false;

	std::string str = devName;
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
		return std::toupper(c);
		});
	if (!ExtractHexValue(str, pid, &result->pid) || !ExtractHexValue(str, vid, &result->vid)) {
		return false;
	}
	return true;
}

LRESULT ScanWndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INPUT:
	{
		if (!IsCanToInteractWithScanner)
			break;
		HRAWINPUT hRawInput = (HRAWINPUT)lParam;
		UINT dataSize = 0;

		// Получаем размер данных
		GetRawInputData(hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

		// Выделяем буфер
		LPBYTE data = new BYTE[dataSize];

		if (GetRawInputData(hRawInput, RID_INPUT, data, &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
		{
			RAWINPUT* raw = (RAWINPUT*)data;
			if (raw->header.dwType == RIM_TYPEKEYBOARD) {
				GetRawInputDeviceInfoA(raw->header.hDevice, RIDI_DEVICENAME, 0, &dataSize);
				char*name = new char[dataSize];
				ZeroMemory(name, dataSize);
				
				if (GetRawInputDeviceInfoA(raw->header.hDevice, RIDI_DEVICENAME, name, &dataSize)) {
					DEVPARAMS params = { 0 };
					if (LoadPIDandVID(name, &params)) {
						UINT64 devID = CreateID(params.pid, params.vid);
						size_t i = GetHash(devID);
						if (scannerObjects[i] != 0) {
							RAWKEYBOARD& kbd = raw->data.keyboard;

							if (kbd.Message == WM_KEYDOWN || kbd.Message == WM_SYSKEYDOWN) {
								scannerObjects[i]->AddKey(kbd.VKey);
								return FALSE;
							}
						}
					}
				}
			}
			else if (raw->header.dwType == RIM_TYPEHID)
			{
				// Получаем информацию об устройстве
				RID_DEVICE_INFO deviceInfo;
				UINT deviceInfoSize = sizeof(deviceInfo);

				if (GetRawInputDeviceInfoA(raw->header.hDevice, RIDI_DEVICEINFO,
					&deviceInfo, &deviceInfoSize) > 0)
				{
						USHORT usagePage = deviceInfo.hid.usUsagePage;
						USHORT usage = deviceInfo.hid.usUsage;
						
						if (usagePage == 0x008C && usage == 0x0002)
							MessageBoxA(0, "Данные поступили, "
								"НО К СОЖАЛЕНИЮ НАШЕ ПРИЛОЖЕНИЕ ЕЩЁ НЕ УМЕЕТ РАБОТАТЬ С "
								"\"hidscanner.dll\"!", "ОШИБКА В ГОЛОВЕ У РАЗРАБОТЧИКА",
								MB_ICONEXCLAMATION);
					
				}
			}
		}

		delete[] data;
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0; break;
	}
	return DefWindowProcA(wnd, msg, wParam, lParam);
}
