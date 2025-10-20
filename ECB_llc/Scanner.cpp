#include "pch.h"
#include "usb_HID.h"
#include <vector>
#include <exception>

#define SCANCLASS "SCANllcwnd"
#define CreateID(pid, vid) ((pid<<16) | vid)

LRESULT CALLBACK ScanWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class ScannerObject
{
public:
	ScannerObject(char*pid, char*vid) : pid(pid), vid(vid) {
		if (pid == 0 || vid == 0)
			throw new std::exception("PID or VID of device is incorrect!");
		
		this->ID = CreateID(std::strtoul(pid, 0, 16), std::strtoul(vid, 0, 16));
	}

	bool CheckExist() {
		return true;
	}

	void RegistrateReadedData(char* data) {
		if (data == 0)
			return;
		size_t dLength = lstrlenA(data);
		if (dLength > 1024)
			return;
		dLength++;
		char* buffer = new char[dLength];
		ZeroMemory(buffer, dLength);	
		memcpy(buffer, data, dLength--);
		BRCQueue.push_back(buffer);
	}

	char* GetReadedData() {
		if (BRCQueue.size() == 0)
			return 0;
		char* buffer = BRCQueue.front();
		BRCQueue.erase(BRCQueue.begin());
		return buffer;
	}

	UINT64 GetID() {
		return ID;
	}

	~ScannerObject() {
		for(char*i : BRCQueue)
			delete[] i;
		BRCQueue.clear();
	}

private:
	std::vector<char*> BRCQueue;
	std::string pid, vid;
	UINT64 ID;
};

ScannerObject* scannerObjects[256] = { 0 };
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
		return 0xffffffff;
	}
	size_t hash = GetHash(scanner->GetID());
	if(scannerObjects[hash] != 0)
		delete scannerObjects[hash];
	scannerObjects[hash] = scanner;
	UINT64 id = scanner->GetID();

	IsCanToInteractWithScanner = true;
	return id;
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
	WaitForSingleObject(ScanThread, INFINITE);
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

struct DEVPARAMS {
	USHORT pid = 0, vid = 0;
};


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
