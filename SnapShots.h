#pragma once
#include "structure_def.h"

extern t_DllList* DllHead;
extern t_DllList* DllTail;

extern t_ProcessList* ProcessHead;
extern t_ProcessList* ProcessTail;

extern t_SampleList* SampleHead;
extern t_SampleList* SampleTail;

extern t_DllProcessesUsers* DllProcessesHead;
extern t_DllProcessesUsers* DllProcessesTail;

extern t_GeneralDllList* GeneralDllHead;
extern t_GeneralDllList* GeneralDllTail;

extern unsigned int sampleCount;
extern unsigned int processCount;
extern unsigned int dllCount;
extern unsigned int sampleDllCount;
extern unsigned int GeneralDllCount;
extern unsigned int DllUsersCount;

extern char FileNameHolder[50];

void AddSampleIntoList(t_SampleList* currentS);
void AddProcessIntoList(t_ProcessList* currentP);
void AddDllIntoList(t_DllList* currentDll);
void GetMemoryInfo(DWORD processID, unsigned int sampleCount, char time[25], t_SampleList* currentS);
void SampleDllCounter(t_SampleList* sample);
void SingleSnapShot(int singleInd);
void AddingNewDll(t_ProcessList* processAddress, char dllName[MAX_PATH]);
t_ProcessList* searchProcess(DWORD processID, char processName[MAX_PATH]);
void AddMemoryInfo(DWORD processID, unsigned int sampleCount, char time[25]);
void AddProcessesInfo();
void SnapShot20();
void LongSnapShot();
void ResetCollections();