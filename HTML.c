#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <psapi.h>
#include <time.h>
#pragma warning(disable:4996)
#include "HTML.h"

int err;
char* strE;

//--------------------------------------------------------------------
// Generate HTML - creating data relevant to html pages and sorting it
//--------------------------------------------------------------------

void AddingNewDllUsers(char ProcessName[MAX_PATH])
{
	t_DllProcessesUsers* currentP = DllProcessesHead;
	if (!DllProcessesHead) //create Users list for every dll if dosent exist already
	{
		t_DllProcessesUsers* newP = (t_DllProcessesUsers*)malloc(sizeof(t_DllProcessesUsers));
		if (!newP)
		{
			err = GetLastError();
			strE = strerror(err);
			LogError(strE);
			return;
		}
		strcpy(newP->ProcessName, ProcessName);
		DllProcessesHead = newP;
		DllProcessesTail = newP;
		newP->next = NULL;
		newP->prev = NULL;
		DllUsersCount++;
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
		t_DllProcessesUsers* newP = (t_DllProcessesUsers*)malloc(sizeof(t_DllProcessesUsers));
		if (!newP)
		{
			err = GetLastError();
			strE = strerror(err);
			LogError(strE);
			return;
		}
		strcpy(newP->ProcessName, ProcessName);
		newP->prev = DllProcessesTail;
		newP->next = NULL;
		DllProcessesTail->next = newP;
		DllProcessesTail = newP;
		DllUsersCount++;
	}
}

//--------------------------------------------------------------------

void DllUsersListGenerator()
{
	//when activated run on every process in all samples and creates users list for each Dll
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
		t_GeneralDllList* newD = (t_GeneralDllList*)malloc(sizeof(t_GeneralDllList));
		if (!newD)
		{
			err = GetLastError();
			strE = strerror(err);
			LogError(strE);
			return;
		}
		strcpy(newD->dllName, dllName);
		GeneralDllHead = newD;
		GeneralDllTail = newD;
		newD->next = NULL;
		newD->prev = NULL;
		GeneralDllCount++;
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
		t_GeneralDllList* newD = (t_GeneralDllList*)malloc(sizeof(t_GeneralDllList));
		if (!newD)
		{
			err = GetLastError();
			strE = strerror(err);
			LogError(strE);
			return;
		}
		strcpy(newD->dllName, dllName);
		newD->prev = GeneralDllTail;
		newD->next = NULL;
		GeneralDllTail->next = newD;
		GeneralDllTail = newD;
		GeneralDllCount++;
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

//--------------------------------------------------------------------

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
				if (currentPro->DllCount < currentProSwap->DllCount)
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
				if (currentPro->DllCount > currentProSwap->DllCount)
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

void ResetHtmlDataCollections()
{
	t_GeneralDllList* currentGD = GeneralDllHead;
	t_GeneralDllList* freeGD = NULL;
	while (currentGD) // freeing sample malloc
	{
		t_DllProcessesUsers* currentP = currentGD->DllUsersList;
		t_DllProcessesUsers* freeP = NULL;
		while (currentP) // freeing processes malloc
		{
			freeP = currentP;
			currentP = currentP->next;
			free(freeP);
		}
		freeGD = currentGD;
		currentGD = currentGD->next;
		free(freeGD);
	}
	// insurance
	GeneralDllHead = NULL;
	GeneralDllTail = NULL;
	DllProcessesHead = NULL;
	DllProcessesTail = NULL;
	GeneralDllCount = 0;
	DllUsersCount = 0;
}

//--------------------------------------------------------------------
// Generate HTML - creating pages (home, sample, Dll)
//--------------------------------------------------------------------

#define HPSEPERATOR "[[sample + Dll tables]]"
#define SPSEPERATOR "[[processes table]]"
#define DPSEPERATOR "[[Dll users table]]"

//--------------------------------------------------------------------

void HtmlSaveIntoFile(char* fileName, char* buff)
{
	FILE* fi = fopen(fileName, "w");
	if (!fi)
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return;
	}

	fputs(buff, fi);


	fclose(fi);
}

//--------------------------------------------------------------------

char* ReadAllFile(char* fileName)
{
	FILE* f = fopen(fileName, "r");
	if (!f)
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return NULL;
	}


	// Get the file size
	char* buff = (char*)malloc(1000);
	if (!buff)
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return;
	}
	char* read;
	int fileSize = 0;
	while ((read = fgets(buff, 1000, f)))
	{
		fileSize += strlen(buff);
	}

	free(buff);
	fclose(f);
	fileSize++;
	// alloc space as file size
	buff = (char*)malloc(fileSize);
	if (!buff)
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return;
	}

	f = fopen(fileName, "r");
	if (!f)
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return NULL;
	}
	int readPosition = 0;
	char charToRead;
	while ((charToRead = fgetc(f)) != EOF)
	{
		buff[readPosition] = charToRead;
		readPosition++;
	}
	buff[readPosition] = NULL;

	fclose(f);

	return buff;
}

//--------------------------------------------------------------------

void HomePgaeGenerator()
{
	t_SampleList* currentS = SampleHead;
	char* htmlTemplate = ReadAllFile("HTML_templates\\Tindex.html"); // get html template a from local folder
	char HTML_REPLACE[10000];
	char* found = strstr(htmlTemplate, HPSEPERATOR); // find token adress
	unsigned long len = found - htmlTemplate; // found (bigger adress) - htmlTemplate (smaller adress)
	char* newFileSpace = (char*)malloc(strlen(htmlTemplate) + (strlen(HTML_REPLACE) * sampleCount) + (strlen(HTML_REPLACE) * GeneralDllCount));
	if (!newFileSpace)
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return;
	}

	strncpy(newFileSpace, htmlTemplate, len); // adding template till token position 
	newFileSpace[len] = NULL;

	while (currentS)
	{
		// creates sample table rows
		sprintf(HTML_REPLACE, "<tr><td data-label=\"ID\">%d</td><td data-label=\"link\"><a href=\"sample%d.html\">Sample%d</a></td><td data-label=\"Process Count\">%d</td><td data-label=\"Dll Count\">%d</td><td data-label=\"WorkingSet Average\">%zu</td></tr>", currentS->sampleNum, currentS->sampleNum, currentS->sampleNum, currentS->ProcessesCount, currentS->DllCount, currentS->WorkingSetAverage);
		strcat(newFileSpace, HTML_REPLACE);
		len = len + strlen(HTML_REPLACE);
		newFileSpace[len] = NULL;
		currentS = currentS->next;
		if (!currentS)
		{
			// creates general Dll table header
			sprintf(HTML_REPLACE, "</table><br><br><h1>General Dll`s List (%d)</h1><table><tr><th>Dll Name</th><th>Dll Page</th></tr>", GeneralDllCount);
			strcat(newFileSpace, HTML_REPLACE);
			len = len + strlen(HTML_REPLACE);
			newFileSpace[len] = NULL;
			t_GeneralDllList* currentD = GeneralDllHead;
			unsigned int counter = 0;
			while (currentD)
			{
				//creates general Dll table rows
				counter++;
				sprintf(HTML_REPLACE, "<tr><td data-label=\"Dll Name\">%s</td><td data-label=\"Dll Page\"><a href=\"dll%d.html\">Link</a></td></tr>", currentD->dllName, counter);
				strcat(newFileSpace, HTML_REPLACE);
				len = len + strlen(HTML_REPLACE);
				newFileSpace[len] = NULL;
				currentD = currentD->next;
			}
		}
	}

	strcat(newFileSpace, found + strlen(HPSEPERATOR)); // adding the rest of the template

	HtmlSaveIntoFile("HTML_dist\\index.html", newFileSpace);


	free(newFileSpace);
	free(htmlTemplate);
}

//--------------------------------------------------------------------

void SamplePgaeGenerator(t_SampleList* currentS)
{
	char* htmlTemplate = ReadAllFile("HTML_templates\\Tsample.html"); // get html template a from local folder
	char HTML_REPLACE[10000];
	char* found = strstr(htmlTemplate, SPSEPERATOR); // find token adress
	unsigned long len = found - htmlTemplate; // found (bigger adress) - htmlTemplate (smaller adress)
	char* newFileSpace = (char*)malloc(strlen(htmlTemplate) + (strlen(HTML_REPLACE) * currentS->ProcessesCount * currentS->ProcessList->DllCount));
	if (!newFileSpace)
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return;
	}

	strncpy(newFileSpace, htmlTemplate, len); // adding template till token position 
	newFileSpace[len] = NULL;

	// adding sample page header + processes table header
	sprintf(HTML_REPLACE, "<h2>ID : %d</h2><h2>Sample Time : %s</h2><h2>Process Count : %d</h2><h2>Dll Count : %d</h2><h2>WorkingSet Average : %zu</h2><br><br><h1>Process List</h1><table><tr><th>Process ID</th><th>Process Name</th><th>PageFaultCount</th><th>WorkingSetSize</th><th>QuotaPagedPoolUsage</th><th>QuotaPeakPagedPoolUsage</th><th>PagefileUsage</th><th>Dll List</th></tr>", currentS->sampleNum, currentS->sampleTime, currentS->ProcessesCount, currentS->DllCount, currentS->WorkingSetAverage);
	strcat(newFileSpace, HTML_REPLACE);
	len = len + strlen(HTML_REPLACE);
	newFileSpace[len] = NULL;
	t_ProcessList* currentP = currentS->ProcessList;
	while (currentP)
	{
		// creates processes table rows
		if (currentP->processID == currentS->HighestWorkingset->processID)
		{
			sprintf(HTML_REPLACE, "<tr><td data-label=\"Process ID\">%lu <i class=\"fa-solid fa-triangle-exclamation\"></td><td data-label=\"Process Name\">%s</td><td data-label=\"PageFaultCount\">%lu</td><td data-label=\"WorkingSetSize\">%zu <i class=\"fa-solid fa-triangle-exclamation\"></i></td><td data-label=\"QuotaPagedPoolUsage\">%zu</td><td data-label=\"QuotaPeakPagedPoolUsage\">%zu</td><td data-label=\"PagefileUsage\">%zu</td>", currentP->processID, currentP->processName, currentP->PageFaultCount, currentP->WorkingSetSize, currentP->QuotaPagedPoolUsage, currentP->QuotaPeakPagedPoolUsage, currentP->PagefileUsage);
			strcat(newFileSpace, HTML_REPLACE);
			len = len + strlen(HTML_REPLACE);
			newFileSpace[len] = NULL;
		}
		else
		{
			sprintf(HTML_REPLACE, "<tr><td data-label=\"Process ID\">%lu</td><td data-label=\"Process Name\">%s</td><td data-label=\"PageFaultCount\">%lu</td><td data-label=\"WorkingSetSize\">%zu</td><td data-label=\"QuotaPagedPoolUsage\">%zu</td><td data-label=\"QuotaPeakPagedPoolUsage\">%zu</td><td data-label=\"PagefileUsage\">%zu</td>", currentP->processID, currentP->processName, currentP->PageFaultCount, currentP->WorkingSetSize, currentP->QuotaPagedPoolUsage, currentP->QuotaPeakPagedPoolUsage, currentP->PagefileUsage);
			strcat(newFileSpace, HTML_REPLACE);
			len = len + strlen(HTML_REPLACE);
			newFileSpace[len] = NULL;
		}
		t_DllList* currentD = currentP->DllList;
		if (!currentD)
		{
			strcpy(HTML_REPLACE, "<td>No acces to Dll list</td></tr>");
			strcat(newFileSpace, HTML_REPLACE);
			len = len + strlen(HTML_REPLACE);
			newFileSpace[len] = NULL;
		}
		while (currentD)
		{
			// creates processes Dll select tag
			if (!currentD->prev)
			{
				sprintf(HTML_REPLACE, "<td data-label=\"Dll List\"><select><option>%s</option>", currentD->dllName);
				strcat(newFileSpace, HTML_REPLACE);
				len = len + strlen(HTML_REPLACE);
				newFileSpace[len] = NULL;
			}
			else
			{
				sprintf(HTML_REPLACE, "<option>%s</option>", currentD->dllName);
				strcat(newFileSpace, HTML_REPLACE);
				len = len + strlen(HTML_REPLACE);
				newFileSpace[len] = NULL;
			}
			currentD = currentD->next;
		}
		if (currentP->DllList)
		{
			strcpy(HTML_REPLACE, "</select></td></tr>");
			strcat(newFileSpace, HTML_REPLACE);
			len = len + strlen(HTML_REPLACE);
			newFileSpace[len] = NULL;
		}
		currentP = currentP->next;
	}
	strcat(newFileSpace, found + strlen(SPSEPERATOR)); // adding the rest of the template

	char pageName[25];
	sprintf(pageName, "HTML_dist\\sample%d.html", currentS->sampleNum);
	HtmlSaveIntoFile(pageName, newFileSpace);

	free(newFileSpace);
	free(htmlTemplate);
}

//--------------------------------------------------------------------

void SamplePgaeGeneratorLoop()
{
	t_SampleList* currentS = SampleHead;
	while (currentS)
	{
		SamplePgaeGenerator(currentS);
		currentS = currentS->next;
	}
}

//--------------------------------------------------------------------

void DllPgaeGenerator(t_GeneralDllList* currentD, unsigned int counter)
{
	char* htmlTemplate = ReadAllFile("HTML_templates\\Tdll.html");  // get html template a from local folder
	char HTML_REPLACE[10000];
	char* found = strstr(htmlTemplate, DPSEPERATOR); // find token adress
	unsigned long len = found - htmlTemplate; // found (bigger adress) - htmlTemplate (smaller adress)
	char* newFileSpace = (char*)malloc(strlen(htmlTemplate) + (strlen(HTML_REPLACE) * currentD->AmountOfUsers));
	if (!newFileSpace)
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return;
	}

	strncpy(newFileSpace, htmlTemplate, len); // adding template till token position 
	newFileSpace[len] = NULL;

	// adding Dll page header + Dll users table header
	sprintf(HTML_REPLACE, "<h2>Dll name : %s</h2><h2>Dll users count : %d</h2><br><br><h1>Dll Users List(Processes)</h1><table><tr><th>Process Name</th></tr>", currentD->dllName, currentD->AmountOfUsers);
	strcat(newFileSpace, HTML_REPLACE);
	len = len + strlen(HTML_REPLACE);
	newFileSpace[len] = NULL;
	t_DllProcessesUsers* currentP = currentD->DllUsersList;
	while (currentP)
	{
		// creates Dll users table rows
		sprintf(HTML_REPLACE, "<tr><td>%s</td></tr>", currentP->ProcessName);
		strcat(newFileSpace, HTML_REPLACE);
		len = len + strlen(HTML_REPLACE);
		newFileSpace[len] = NULL;
		currentP = currentP->next;
	}
	strcat(newFileSpace, found + strlen(DPSEPERATOR));  // adding the rest of the template

	char pageName[25];
	sprintf(pageName, "HTML_dist\\dll%d.html", counter);
	HtmlSaveIntoFile(pageName, newFileSpace);

	free(newFileSpace);
	free(htmlTemplate);
}

//--------------------------------------------------------------------

void DllPgaeGeneratorLoop()
{
	t_GeneralDllList* currentD = GeneralDllHead;
	unsigned int counter = 0;
	while (currentD)
	{
		counter++;
		DllPgaeGenerator(currentD, counter);
		currentD = currentD->next;
	}
}
