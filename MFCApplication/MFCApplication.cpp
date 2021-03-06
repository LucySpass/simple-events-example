// MFCApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MFCApplication.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;

using namespace std;

HANDLE hConfirm = CreateEvent(NULL, FALSE, FALSE, "EventConfirm");
HANDLE hThreadStopped = CreateEvent(NULL, FALSE, FALSE, "EventThreadStopped");
HANDLE hStopLastThread = CreateEvent(NULL, TRUE, FALSE, "EventStopIt");

HANDLE hEvents[4];

vector<HANDLE> BegovichContainer;

DWORD WINAPI BegovichThreadStart(LPVOID lpParameter) 
{
	const int index = BegovichContainer.size() - 1;
	std::cout << "Starting " << index << " thread..." << endl;
	
	SetEvent(hConfirm);

	while (1) {
		WaitForSingleObject(hStopLastThread, INFINITE);

		if (index == (BegovichContainer.size() - 1)) {

			ResetEvent(hStopLastThread);
			CloseHandle(lpParameter);
			cout << "Thread " << index << " stopped!" << endl;
			
			SetEvent(hThreadStopped);

			return 0;
		}
		else if (BegovichContainer.size() < 1) {
			cout << "Container is empty!" << endl;
			SetEvent(hConfirm);
		}
		else {
			cout << "Not the last one! Checked index: " << index << endl;
		}
	}
	return 0;
}

void start()
{
	hEvents[0] = CreateEvent(NULL, FALSE, FALSE, "EventStart");
	hEvents[1] = CreateEvent(NULL, FALSE, FALSE, "EventQuit");
	hEvents[2] = CreateEvent(NULL, FALSE, FALSE, "EventStartThread");
	hEvents[3] = CreateEvent(NULL, FALSE, FALSE, "EventStop");

	while (1)
	{
		DWORD dwResult = WaitForMultipleObjects(4, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0;
		switch (dwResult)
		{
			case 0:
			{
				std::cout << "Appplication start" << endl;
				SetEvent(hConfirm);
				break;
			}
			case 1:
			{
				std::cout << "App quit" << endl;
				CloseHandle(hEvents[0]);
				CloseHandle(hEvents[1]);
				CloseHandle(hEvents[2]);				
				CloseHandle(hThreadStopped);
				CloseHandle(hEvents[3]);
				BegovichContainer.clear();

				SetEvent(hConfirm);
				CloseHandle(hConfirm);

				return;
			}
			case 2: {

				HANDLE hThread;				
				BegovichContainer.push_back(hThread);
				hThread = CreateThread(NULL, 0, BegovichThreadStart, (LPVOID)hThread, 0, NULL);
	
				break;
			}
			case 3: {		
				SetEvent(hStopLastThread);
				WaitForSingleObject(hThreadStopped, INFINITE);

				BegovichContainer.erase(BegovichContainer.end() - 1);
				SetEvent(hConfirm);			

				break;
			}
		}
	}
}

int main()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			start();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}
