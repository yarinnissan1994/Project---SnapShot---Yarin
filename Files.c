#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <psapi.h>
#pragma warning (disable : 4996)
#include "Files.h"

//--------------------------------------------------------------------
// save into file
//--------------------------------------------------------------------

int err;
char* strE;

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
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
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
			LogError("Failed to use fwrite function");
			err = GetLastError();
			strE = strerror(err);
			LogError(strE);
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
				LogError("Failed to use fwrite function");
				err = GetLastError();
				strE = strerror(err);
				LogError(strE);
			}
			currentP = currentS->ProcessList;
			for (int j = 0; j < currentS->ProcessesCount; j++)
			{
				write = fwrite(currentP, sizeof(t_ProcessList), 1, f); // write processes to file
				if (!write)
				{
					LogError("Failed to use fwrite function");
					err = GetLastError();
					strE = strerror(err);
					LogError(strE);
				}
				currentD = currentP->DllList;
				for (int k = 0; k < currentP->DllCount; k++)
				{
					write = fwrite(currentD, sizeof(t_DllList), 1, f); // write Dll to file
					if (!write)
					{
						LogError("Failed to use fwrite function");
						err = GetLastError();
						strE = strerror(err);
						LogError(strE);
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

int LoadFile()
{
	FILE* f = fopen(FileNameHolder, "rb");
	if (!f)
	{
		err = GetLastError();
		strE = strerror(err);
		LogError(strE);
		return 1;
	}
	else
	{
		t_FileHeader* header = (t_FileHeader*)malloc(sizeof(t_FileHeader));
		int read;
		read = fread(header, sizeof(t_FileHeader), 1, f); // read header from file
		if (!read)
		{
			LogError("Faild to use fread function");
			err = GetLastError();
			strE = strerror(err);
			LogError(strE);
		}
		else
		{
			for (int i = 0; i < header->samples; i++)
			{
				t_SampleList* currentS = (t_SampleList*)malloc(sizeof(t_SampleList)); //current sample
				read = fread(currentS, sizeof(t_SampleList), 1, f); // read sample from file
				if (!read)
				{
					LogError("Faild to use fread function");
					err = GetLastError();
					strE = strerror(err);
					LogError(strE);
				}
				for (int j = 0; j < currentS->ProcessesCount; j++)
				{
					t_ProcessList* currentP = (t_ProcessList*)malloc(sizeof(t_ProcessList)); //current process
					read = fread(currentP, sizeof(t_ProcessList), 1, f); // read process from file
					if (!read)
					{
						LogError("Faild to use fread function");
						err = GetLastError();
						strE = strerror(err);
						LogError(strE);
					}
					for (int k = 0; k < currentP->DllCount; k++)
					{
						t_DllList* currentD = (t_DllList*)malloc(sizeof(t_DllList)); //current dll
						read = fread(currentD, sizeof(t_DllList), 1, f); // read Dll from file 
						AddDllIntoList(currentD); // adding dll to linked list
						if (!read)
						{
							LogError("Faild to use fread function");
							err = GetLastError();
							strE = strerror(err);
							LogError(strE);
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
		return 0;
	}
}