#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <psapi.h>
#include <time.h>
#pragma warning(disable:4996)
#include "structure_def.h"

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
		// You better call GetLastError() here
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
		// You better call GetLastError() here
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
		// Error. Write to log
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
		// You better call GetLastError() here
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
		// You better call GetLastError() here
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
		// Error. Write to log
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

	printf("Taking Info Shots\n%d\n", 1);

	for (int i = 1; i < 20; i++)
	{
		Sleep(1000);
		printf("%d\n", i+1);
		AddProcessesInfo(); //Adding all information to the value base
	}
	SampleTail->WorkingSetAverage = SampleTail->WorkingSetSum / SampleTail->ProcessesCount;
	SampleDllCounter(SampleTail);
	
}

//--------------------------------------------------------------------
// Long SnapShot
//--------------------------------------------------------------------

void LongSnapShot()
{
	SingleSnapShot(0); //creating new sample as a value adding base 
	printf("Press \"E\" when you done (make sure caps lock is on)\n");
	printf("Taking Info Shots\n%d\n", 1);

	int pause = 1;
	int shotCount = 2;

	do 
	{
		Sleep(1000);
		printf("%d\n",shotCount);
		AddProcessesInfo(); //Adding all information to the value base
		shotCount++;
		if (kbhit()) //event listener waiting user to end adding information
		{
			char ch1 = getch();
			if (ch1 == 'E')
			{
				pause = 0;
			}
		}
	} while (pause);
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

//--------------------------------------------------------------------
// save into file
//--------------------------------------------------------------------

void SaveIntoFile()
{
	if (!SampleHead) return;//if there is no SnapShots


	//currect time string generator
	time_t t;
	time(&t);
	struct tm* timeinfo;
	timeinfo = localtime(&t);

	//dynamic file name generetor
	char fileName[100];
	sprintf(fileName, "Report_%d-%d-%d_%d-%d-%d.bin", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	strcpy(FileNameHolder, fileName);

	FILE* f = fopen(fileName, "wb");
	if (!f)
	{
		//error
		return;
	}
	else
	{
		//file meta data
		t_FileHeader* header = (t_FileHeader*)malloc(sizeof(t_FileHeader));
		header->Version = 1;
		header->samples = sampleCount;
		int write;
		int sampleCounter = 0;
		int processCounter = 0;
		int dllCounter = 0;
		write = fwrite(header, sizeof(t_FileHeader), 1, f); // write headwe to file
		if (!write)
		{
			//error
		}
		t_SampleList* currentS; //current sample
		t_ProcessList* currentP; //current process
		t_DllList* currentD; //current dll
		currentS = SampleHead;
		for (int i = 0; i < sampleCount; i++)
		{
			write = fwrite(currentS, sizeof(t_SampleList), 1, f); // write samples to file
			if (!write)
			{
				//error
			}
			currentP = currentS->ProcessList;
			for (int j = 0; j < currentS->ProcessesCount; j++)
			{
				write = fwrite(currentP, sizeof(t_ProcessList), 1, f); // write processes to file
				if (!write)
				{
					//error
				}
				currentD = currentP->DllList;
				for (int k = 0; k < currentP->DllCount; k++)
				{
					write = fwrite(currentD, sizeof(t_DllList), 1, f); // write Dll to file
					if (!write)
					{
						//error
					}
					currentD = currentD->next;
					dllCounter++;
				}
				currentP = currentP->next;
				processCounter++;
			}
			currentS = currentS->next;
			sampleCounter++;
		}
		free(header);
		fclose(f);
	}
}

//--------------------------------------------------------------------
// Load file
//--------------------------------------------------------------------

void LoadFile()
{
	FILE* f = fopen(FileNameHolder, "rb");
	if (!f)
	{
		//error
		return;
	}
	else
	{
		t_FileHeader* header = (t_FileHeader*)malloc(sizeof(t_FileHeader));
		int read;
		read = fread(header, sizeof(t_FileHeader), 1, f); // read header from file
		if (!read)
		{
			//error
		}
		else
		{
			for (int i = 0; i < header->samples; i++)
			{
				t_SampleList* currentS = (t_SampleList*)malloc(sizeof(t_SampleList)); //current sample
				read = fread(currentS, sizeof(t_SampleList), 1, f); // read sample from file
				if (!read)
				{
					//error
				}
				for (int j = 0; j < currentS->ProcessesCount; j++)
				{
					t_ProcessList* currentP = (t_ProcessList*)malloc(sizeof(t_ProcessList)); //current process
					read = fread(currentP, sizeof(t_ProcessList), 1, f); // read process from file
					if (!read)
					{
						//error
					}
					for (int k = 0; k < currentP->DllCount; k++)
					{
						t_DllList* currentD = (t_DllList*)malloc(sizeof(t_DllList)); //current dll
						read = fread(currentD, sizeof(t_DllList), 1, f); // read Dll from file 
						AddDllIntoList(currentD); // adding dll to linked list
						if (!read)
						{
							//error
						}
					}
					currentP->DllList = DllHead;
					DllHead = NULL;
					DllTail = NULL;
					AddProcessIntoList(currentP); // adding process to linked list
				}
				currentS->ProcessList = ProcessHead;
				ProcessHead = NULL;
				ProcessTail = NULL;
				AddSampleIntoList(currentS); // adding asample to linked list
			}
			free(header);
		}
		fclose(f);
	}
}

//--------------------------------------------------------------------
// Generate HTML
//--------------------------------------------------------------------

void AddingNewDllUsers(char ProcessName[MAX_PATH])
{
	t_DllProcessesUsers* currentP = DllProcessesHead;
	if (!DllProcessesHead) //create Users list for every dll if dosent exist already
	{
		DllUsersCount++;
		t_DllProcessesUsers* newP = (t_DllProcessesUsers*)malloc(sizeof(t_DllProcessesUsers));
		strcpy(newP->ProcessName, ProcessName);
		DllProcessesHead = newP;
		DllProcessesTail = newP;
		newP->next = NULL;
		newP->prev = NULL;
	}
	else
	{
		while (currentP) //search process by name in every general dll
		{
			if (0 == (strcmp(currentP->ProcessName, ProcessName)))
			{
				return;
			}
			currentP = currentP->next;
		}
		//if process was not found - adding new process to General dll
		DllUsersCount++;
		t_DllProcessesUsers* newP = (t_DllProcessesUsers*)malloc(sizeof(t_DllProcessesUsers));
		strcpy(newP->ProcessName, ProcessName);
		newP->prev = DllProcessesTail;
		newP->next = NULL;
		DllProcessesTail->next = newP;
		DllProcessesTail = newP;
	}
}

//--------------------------------------------------------------------

void DllUsersListGenerator()
{
	//when activated run on every dll in linked list and create genral list of all dll used
	t_GeneralDllList* currentGD = GeneralDllHead;
	while (currentGD)
	{
		t_SampleList* currentS = SampleHead;
		while (currentS)
		{
			t_ProcessList* currentP = currentS->ProcessList;
			while (currentP)
			{
				t_DllList* currentD = currentP->DllList;
				while (currentD)
				{
					if (0 == (strcmp(currentD->dllName, currentGD->dllName)))
					{
						AddingNewDllUsers(currentP->processName);
						break;
					}
					currentD = currentD->next;
				}
				currentP = currentP->next;
			}
			currentS = currentS->next;
		}
		currentGD->AmountOfUsers = DllUsersCount;
		currentGD->DllUsersList = DllProcessesHead;
		DllUsersCount = 0;
		DllProcessesHead = NULL;
		DllProcessesTail = NULL;
		currentGD = currentGD->next;
	}
}

//--------------------------------------------------------------------

void AddingNewGeneralDll(char dllName[MAX_PATH])
{
	t_GeneralDllList* currentD = GeneralDllHead;
	if (!GeneralDllHead) //create General Dll list if dosent exist already
	{
		GeneralDllCount++;
		t_GeneralDllList* newD = (t_GeneralDllList*)malloc(sizeof(t_GeneralDllList));
		strcpy(newD->dllName, dllName);
		GeneralDllHead = newD;
		GeneralDllTail = newD;
		newD->next = NULL;
		newD->prev = NULL;
	}
	else
	{
		while (currentD) //search dll by name in a General dll list
		{
			if (0 == (strcmp(currentD->dllName, dllName)))
			{
				return;
			}
			currentD = currentD->next;
		}
		//if dll was not found - adding new dll to General dll list
		GeneralDllCount++;
		t_GeneralDllList* newD = (t_GeneralDllList*)malloc(sizeof(t_GeneralDllList));
		strcpy(newD->dllName, dllName);
		newD->prev = GeneralDllTail;
		newD->next = NULL;
		GeneralDllTail->next = newD;
		GeneralDllTail = newD;
	}
}

//--------------------------------------------------------------------

void GeneralDllListGenerator()
{
	//when activated run on every dll in linked list and create genral list of all dll used
	t_SampleList* currentS = SampleHead;
	while (currentS)
	{
		t_ProcessList* currentP = currentS->ProcessList;
		while (currentP)
		{
			t_DllList* currentD = currentP->DllList;
			while (currentD)
			{
				AddingNewGeneralDll(currentD->dllName);
				currentD = currentD->next;
			}
			currentP = currentP->next;
		}
		currentS = currentS->next;
	}
	DllUsersListGenerator();
}

//--------------------------------------------------------------------

t_ProcessList* tempHead = NULL;
t_ProcessList* tempTail = NULL;

void Swap(t_ProcessList* Adress)
{
	if (Adress == NULL || Adress->next == NULL)
	{
		return;
	}
	t_ProcessList temp;
	temp.next = Adress->next->next;
	temp.prev = Adress->next;
	Adress->next->next = Adress;
	Adress->next->prev = Adress->prev;
	Adress->next = temp.next;
	Adress->prev = temp.prev;
	if (Adress->prev->prev == NULL)
	{
		tempHead = Adress->prev;
	}
	else
	{
		Adress->prev->prev->next = Adress->prev;
	}
	if (Adress->next == NULL)
	{
		tempTail = Adress;
	}
	else
	{
		Adress->next->prev = Adress;
	}
}

//--------------------------------------------------------------------

void SortSampleByDllAmount()
{
	t_SampleList* currentS = SampleHead;
	while (currentS)
	{
		t_ProcessList* currentP = currentS->ProcessList;

		while (currentP)
		{
			if (!currentP->next)
			{
				tempHead = currentS->ProcessList;
				tempTail = currentP;
			}
			currentP = currentP->next;
		}
		t_ProcessList* currentPro = NULL;
		t_ProcessList* currentProSwap = NULL;
		t_ProcessList* headStoper = NULL;
		t_ProcessList* tailStoper = NULL;
		int swaps = 1;
		if (tempHead == NULL)
		{
			return;
		}
		currentPro = tempHead;
		currentProSwap = tempHead->next;
		do
		{
			swaps = 0;
			while (currentProSwap != NULL && currentPro->next != tailStoper)
			{
				if (currentPro->DllCount > currentProSwap->DllCount)
				{
					Swap(currentPro);
					swaps = 1;
					currentProSwap = currentPro->next;
				}
				else
				{
					currentPro = currentPro->next;
					currentProSwap = currentProSwap->next;
				}
			}
			tailStoper = currentPro;
			currentPro = currentPro->prev;
			currentProSwap = currentPro->prev;
			while (currentProSwap != NULL && currentPro->prev != headStoper)
			{
				if (currentPro->DllCount < currentProSwap->DllCount)
				{
					Swap(currentProSwap);
					swaps = 1;
					currentProSwap = currentPro->prev;
				}
				else
				{
					currentPro = currentPro->prev;
					currentProSwap = currentProSwap->prev;
				}
			}
			headStoper = currentPro;
			currentPro = currentPro->next;
			currentProSwap = currentPro->next;
		} while (swaps);
		currentS->ProcessList = tempHead;
		tempHead = NULL;
		tempTail = NULL;
		currentS = currentS->next;
	}
}

//--------------------------------------------------------------------

void HighestWorksetProcess()
{
	t_SampleList* currentS = SampleHead;
	while (currentS)
	{
		t_ProcessList* currentP = currentS->ProcessList;
		t_ProcessList* highestHolder;
		highestHolder = currentP;
		while (currentP)
		{
			if (currentP->WorkingSetSize > highestHolder->WorkingSetSize)
			{
				highestHolder = currentP;
			}
			currentP = currentP->next;
		}
		currentS->HighestWorkingset = highestHolder;
		currentS = currentS->next;
	}
}

//--------------------------------------------------------------------


#define DEBUG

#ifdef DEBUG
main()
{
	SingleSnapShot(1);
	SingleSnapShot(1);
	SingleSnapShot(1);
	SingleSnapShot(1);
	SingleSnapShot(1);
	SingleSnapShot(1);
	SnapShot20();
	LongSnapShot();
	LongSnapShot();
	SaveIntoFile();
	ResetCollections();
	LoadFile();
	//GeneralDllListGenerator();
	//SortSampleByDllAmount();
	//HighestWorksetProcess();
	//stop
	return 0;
}
#endif