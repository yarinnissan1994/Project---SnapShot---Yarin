#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <psapi.h>
#include <time.h>
#pragma warning(disable:4996)
#include "SnapShots.h"

#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

t_DllList* DllHead = NULL;
t_DllList* DllTail = NULL;

t_ProcessList* ProcessHead = NULL;
t_ProcessList* ProcessTail = NULL;

t_SampleList* SampleHead = NULL;
t_SampleList* SampleTail = NULL;

t_DllProcessesUsers* DllProcessesHead = NULL;
t_DllProcessesUsers* DllProcessesTail = NULL;

t_GeneralDllList* GeneralDllHead = NULL;
t_GeneralDllList* GeneralDllTail = NULL;


unsigned int sampleCount = 0;
unsigned int processCount = 0;
unsigned int dllCount = 0;
unsigned int sampleDllCount = 0;
unsigned int GeneralDllCount = 0;
unsigned int DllUsersCount = 0;

char FileNameHolder[50] = "last file is empty";

int err;
char* strE;
 
//--------------------------------------------------------------------
// Single Snap Shot
//--------------------------------------------------------------------

void AddSampleIntoList(t_SampleList* currentS) //Adding SnapShot sample to a linked list 
{
	sampleCount++;
	currentS->sampleNum = sampleCount;
	currentS->next = NULL;
	if (!SampleHead)
	{
		SampleHead = currentS;
		SampleTail = currentS;
		currentS->prev = NULL;
	}
	else
	{
		currentS->prev = SampleTail;
		SampleTail->next = currentS;
		SampleTail = currentS;
	}
}

//--------------------------------------------------------------------

void AddProcessIntoList(t_ProcessList* currentP) //Adding process to a linked list 
{
	processCount++;
	currentP->sampleNum = sampleCount + 1;
	currentP->next = NULL;
	if (!ProcessHead)
	{
		ProcessHead = currentP;
		ProcessTail = currentP;
		currentP->prev = NULL;
	}
	else
	{
		currentP->prev = ProcessTail;
		ProcessTail->next = currentP;
		ProcessTail = currentP;
	}
}

//--------------------------------------------------------------------

void AddDllIntoList(t_DllList* currentDll) //Adding dll to a linked list 
{
	dllCount++;
	currentDll->next = NULL;
	if (!DllHead)
	{
		DllHead = currentDll;
		DllTail = currentDll;
		currentDll->prev = NULL;
	}
	else
	{
		currentDll->prev = DllTail;
		DllTail->next = currentDll;
		DllTail = currentDll;
	}
}

//--------------------------------------------------------------------

void GetMemoryInfo(DWORD processID, unsigned int sampleCount, char time[25], t_SampleList* currentS)
{
	//create memory alloction and place given parameters
	t_ProcessList* currentP = (t_ProcessList*)malloc(sizeof(t_ProcessList));
	currentP->processID = processID;
	currentP->sampleNum = sampleCount;
	currentP->next = NULL;
	currentP->prev = NULL;
	currentP->DllList = NULL;
	currentP->DllCount = 0;
	strcpy(currentP->sampleTime, time);

	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	//open process by ID
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (!hProcess)
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return;
	}

	HMODULE hMods[1024];
	DWORD cbNeeded;
	TCHAR Buffer[MAX_PATH];
	TCHAR Buffer2[MAX_PATH];

	//process name (buffer)
	if (GetModuleFileNameEx(hProcess, 0, Buffer, MAX_PATH))
	{
		//convert process name to a char string
		size_t numConverted;
		wcstombs_s(&numConverted, currentP->processName, MAX_PATH, Buffer, MAX_PATH);
	}
	else
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
	}

	//place all memory values
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		currentP->PageFaultCount = pmc.PageFaultCount;
		currentP->WorkingSetSize = pmc.WorkingSetSize;
		currentP->QuotaPagedPoolUsage = pmc.QuotaPagedPoolUsage;
		currentP->QuotaPeakPagedPoolUsage = pmc.QuotaPeakPagedPoolUsage;
		currentP->PagefileUsage = pmc.PagefileUsage;
		currentS->WorkingSetSum = currentS->WorkingSetSum + pmc.WorkingSetSize;
	}

	//Get Dlls List
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			//dll name (buffer2)
			if (GetModuleFileNameEx(hProcess, hMods[i], Buffer2, MAX_PATH))
			{
				t_DllList* currentDll = (t_DllList*)malloc(sizeof(t_DllList));

				//convert dll name to a char string
				size_t numConverted;
				wcstombs_s(&numConverted, currentDll->dllName, MAX_PATH, Buffer2, MAX_PATH);

				AddDllIntoList(currentDll);
			}
		}
		//place dll list and count in current process
		currentP->DllList = DllHead;
		currentP->DllCount = dllCount;
		DllHead = NULL;
		DllTail = NULL;
		dllCount = 0;
	}
	//add current process into processes list
	AddProcessIntoList(currentP);
	CloseHandle(hProcess);
}

//--------------------------------------------------------------------

void SampleDllCounter(t_SampleList* sample)
{
	t_GeneralDllList* tempHead = NULL;
	t_GeneralDllList* tempTail = NULL;
	t_SampleList* currentS = sample;
	while (currentS)
	{
		t_ProcessList* currentP = currentS->ProcessList;
		while (currentP)
		{
			t_DllList* currentD = currentP->DllList;
			while (currentD)
			{
				int stoper = 1;
				t_GeneralDllList* currentSD = tempHead;
				if (!tempHead) //create General Dll list if dosent exist already
				{
					sampleDllCount++;
					t_GeneralDllList* newD = (t_GeneralDllList*)malloc(sizeof(t_GeneralDllList));
					strcpy(newD->dllName, currentD->dllName);
					tempHead = newD;
					tempTail = newD;
					newD->next = NULL;
					newD->prev = NULL;
				}
				else
				{
					while (currentSD)
					{
						if (0 == (strcmp(currentSD->dllName, currentD->dllName)))
						{
							stoper = 0;
							break;
						}
						currentSD = currentSD->next;
					}
					if (stoper == 1)
					{
						sampleDllCount++;
						t_GeneralDllList* newD = (t_GeneralDllList*)malloc(sizeof(t_GeneralDllList));
						strcpy(newD->dllName, currentD->dllName);
						newD->prev = tempTail;
						newD->next = NULL;
						tempTail->next = newD;
						tempTail = newD;
					}
				}
				currentD = currentD->next;
			}
			currentP = currentP->next;
		}
		currentS = currentS->next;
	}
	sample->DllCount = sampleDllCount;
	sampleDllCount = 0;
	t_GeneralDllList* current = tempHead;
	t_GeneralDllList* freeSD = NULL;
	while (current) // freeing Dll malloc
	{
		freeSD = current;
		current = current->next;
		free(freeSD);
	}
}

//--------------------------------------------------------------------

void SingleSnapShot(int singleInd) //call all processes ID (singleInd indicates if function used by user or by other snapshot functions)
{
	if (singleInd) printf("Taking Single Info Shot\n\n");

	//currect time string generator
	time_t t;
	time(&t);
	struct tm* timeinfo;
	timeinfo = localtime(&t);
	char time[25];
	sprintf(time, "%d.%d.%d-%d:%d:%d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	//all processes ID are located inside aProcesses array
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return 1;
	}

	//Calculate how many processes identifiers were returned
	cProcesses = cbNeeded / sizeof(DWORD);

	t_SampleList* currentS = (t_SampleList*)malloc(sizeof(t_SampleList));
	currentS->DllCount = 0;
	currentS->WorkingSetAverage = 0;
	currentS->WorkingSetSum = 0;
	currentS->HighestWorkingset = NULL;

	//get processes info by ID (aProcesses[i])
	for (i = 0; i < cProcesses; i++)
	{
		GetMemoryInfo(aProcesses[i], sampleCount, time, currentS);
	}

	//place all values inside sample list
	currentS->ProcessList = ProcessHead;
	currentS->ProcessesCount = processCount;
	currentS->DllCount = sampleDllCount;
	currentS->next = NULL;
	currentS->prev = NULL;
	if (singleInd)
	{
		currentS->WorkingSetAverage = currentS->WorkingSetSum / currentS->ProcessesCount;
		SampleDllCounter(currentS);
	}
	strcpy(currentS->sampleTime, time);
	AddSampleIntoList(currentS);

	ProcessHead = NULL;
	ProcessTail = NULL;
	processCount = 0;
}

//--------------------------------------------------------------------
// 20 Seconds SnapShot
//--------------------------------------------------------------------

void AddingNewDll(t_ProcessList* processAddress, char dllName[MAX_PATH])
{
	t_DllList* currentD = processAddress->DllList;
	while (currentD) //search dll by name in a process dll list
	{
		if (0 == (strcmp(currentD->dllName, dllName)))
		{
			return;
		}
		currentD = currentD->next;
	}
	//if dll was not found - adding new dll to process dll list
	t_DllList* newD = (t_DllList*)malloc(sizeof(t_DllList));
	currentD = processAddress->DllList;
	currentD->prev = newD;
	strcpy(newD->dllName, dllName);
	newD->next = currentD;
	newD->prev = NULL;
	processAddress->DllList = newD;
}

//--------------------------------------------------------------------

t_ProcessList* searchProcess(DWORD processID, char processName[MAX_PATH])
{
	t_ProcessList* currentP = SampleTail->ProcessList;
	while (currentP)
	{
		if (processID == currentP->processID && 0 == (strcmp(currentP->processName, processName)))
		{
			return currentP; //if search found process by ID and by name
		}
		currentP = currentP->next;
	}
	return 0; //if process was not found
}

//--------------------------------------------------------------------

void AddMemoryInfo(DWORD processID, unsigned int sampleCount, char time[25])
{
	//create memory alloction and place first values

	char processName[MAX_PATH];
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	//open process by ID
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (!hProcess)
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return;
	}

	HMODULE hMods[1024];
	DWORD cbNeeded;
	TCHAR Buffer[MAX_PATH];
	TCHAR Buffer2[MAX_PATH];

	//process name (buffer)
	if (GetModuleFileNameEx(hProcess, 0, Buffer, MAX_PATH))
	{
		size_t numConverted;
		wcstombs_s(&numConverted, processName, MAX_PATH, Buffer, MAX_PATH);
	}
	else
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
	}

	t_ProcessList* currentP = searchProcess(processID, processName);

	if (currentP) //if searchProcess function returned a process pointer
	{
		//ADD all memory values
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
		{
			currentP->PageFaultCount = currentP->PageFaultCount + pmc.PageFaultCount;
			currentP->WorkingSetSize = currentP->WorkingSetSize + pmc.WorkingSetSize;
			currentP->QuotaPagedPoolUsage = currentP->QuotaPagedPoolUsage + pmc.QuotaPagedPoolUsage;
			currentP->QuotaPeakPagedPoolUsage = currentP->QuotaPeakPagedPoolUsage + pmc.QuotaPeakPagedPoolUsage;
			currentP->PagefileUsage = currentP->PagefileUsage + pmc.PagefileUsage;
			SampleTail->WorkingSetSum = SampleTail->WorkingSetSum + pmc.WorkingSetSize;
		}
		//Get Dlls List
		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
			{
				TCHAR szModName[MAX_PATH];
				char dllName[MAX_PATH];

				//dll name (buffer2)
				if (GetModuleFileNameEx(hProcess, hMods[i], Buffer2, MAX_PATH))
				{
					size_t numConverted;
					wcstombs_s(&numConverted, dllName, MAX_PATH, Buffer2, MAX_PATH);
					AddingNewDll(currentP, dllName);
				}
			}
			t_DllList* DllCountUpdate = currentP->DllList;
			int newDllCount = 0;
			while (DllCountUpdate)
			{
				newDllCount++;
				DllCountUpdate = DllCountUpdate->next;
			}
			currentP->DllCount = newDllCount;
		}
	}
	else //creates new process in process list (if searchProcess function returned NULL)
	{
		if (!strstr(processName, "exe") || !strstr(processName, "EXE"))
		{
			return;
		}
		t_ProcessList* newP = (t_ProcessList*)malloc(sizeof(t_ProcessList));
		newP->processID = processID;
		newP->sampleNum = sampleCount;
		strcpy(newP->sampleTime, time);
		strcpy(newP->processName, processName);
		//place all memory values
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
		{
			newP->PageFaultCount = pmc.PageFaultCount;
			newP->WorkingSetSize = pmc.WorkingSetSize;
			newP->QuotaPagedPoolUsage = pmc.QuotaPagedPoolUsage;
			newP->QuotaPeakPagedPoolUsage = pmc.QuotaPeakPagedPoolUsage;
			newP->PagefileUsage = pmc.PagefileUsage;
		}

		//Get Dlls List
		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			unsigned int dllCounter = 0;

			for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
			{
				TCHAR szModName[MAX_PATH];

				//dll name (buffer2)
				if (GetModuleFileNameEx(hProcess, hMods[i], Buffer2, MAX_PATH))
				{
					t_DllList* currentDll = (t_DllList*)malloc(sizeof(t_DllList));

					size_t numConverted;
					wcstombs_s(&numConverted, currentDll->dllName, MAX_PATH, Buffer2, MAX_PATH);

					AddDllIntoList(currentDll);

					dllCounter++;
				}
			}
			//place dll list and count in current process
			newP->DllList = DllHead;
			newP->DllCount = dllCounter;
			DllHead = NULL;
			DllTail = NULL;
		}
		//Adding new process to sample
		currentP = SampleTail->ProcessList;
		currentP->prev = newP;
		newP->next = currentP;
		newP->prev = NULL;
		SampleTail->ProcessList = newP;
	}
	CloseHandle(hProcess);
}

//--------------------------------------------------------------------

void AddProcessesInfo() //call all processes ID
{
	//all processes ID are located inside aProcesses array
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return 1;
	}

	//Calculate how many processes identifiers were returned
	cProcesses = cbNeeded / sizeof(DWORD);

	//get processes info by ID (aProcesses[i])
	for (i = 0; i < cProcesses; i++)
	{
		AddMemoryInfo(aProcesses[i], sampleCount, SampleTail->sampleTime);
	}
	t_ProcessList* ProcessCountUpdate = SampleTail->ProcessList;
	int newProcessCount = 0;
	while (ProcessCountUpdate)
	{
		newProcessCount++;
		ProcessCountUpdate = ProcessCountUpdate->next;
	}
	SampleTail->ProcessesCount = newProcessCount;
}

//--------------------------------------------------------------------

void SnapShot20()
{
	SingleSnapShot(0); //creating new sample as a value adding base 

	printf("Taking Info Shots\nshot count: %d\n", 1);

	for (int i = 1; i < 20; i++)
	{
		Sleep(1000);
		gotoxy(1, 2);
		printf("shot count: %d\n", i + 1);
		AddProcessesInfo(); //Adding all information to the value base
	}
	printf("\n");
	SampleTail->WorkingSetAverage = SampleTail->WorkingSetSum / SampleTail->ProcessesCount;
	SampleDllCounter(SampleTail);

}

//--------------------------------------------------------------------
// Long SnapShot
//--------------------------------------------------------------------

void LongSnapShot()
{
	SingleSnapShot(0); //creating new sample as a value adding base 
	printf("Press \"E\" when you done\n");
	printf("Taking Info Shots\nshot count: %d\n", 1);

	int pause = 1;
	int shotCount = 2;

	do
	{
		Sleep(1000);
		gotoxy(1, 3);
		printf("shot count: %d\n", shotCount);
		AddProcessesInfo(); //Adding all information to the value base
		shotCount++;
		if (kbhit()) //event listener waiting user to end adding information
		{
			char ch1 = getch();
			if (ch1 == 'E' || ch1 == 'e')
			{
				pause = 0;
			}
		}
	} while (pause);
	printf("\n");
	SampleTail->WorkingSetAverage = SampleTail->WorkingSetSum / SampleTail->ProcessesCount;
	SampleDllCounter(SampleTail);
}

//--------------------------------------------------------------------
// reset collections
//--------------------------------------------------------------------

void ResetCollections()
{
	t_SampleList* currentS = SampleHead;
	t_SampleList* freeS = NULL;
	while (currentS) // freeing sample malloc
	{
		t_ProcessList* currentP = currentS->ProcessList;
		t_ProcessList* freeP = NULL;
		while (currentP) // freeing processes malloc
		{
			t_DllList* currentD = currentP->DllList;
			t_DllList* freeD = NULL;
			while (currentD) // freeing Dll malloc
			{
				freeD = currentD;
				currentD = currentD->next;
				free(freeD);
			}
			freeP = currentP;
			currentP = currentP->next;
			free(freeP);
		}
		freeS = currentS;
		currentS = currentS->next;
		free(freeS);
	}
	// insurance
	DllHead = NULL;
	DllTail = NULL;
	ProcessHead = NULL;
	ProcessTail = NULL;
	SampleHead = NULL;
	SampleTail = NULL;
	sampleCount = 0;
	processCount = 0;
	dllCount = 0;
}