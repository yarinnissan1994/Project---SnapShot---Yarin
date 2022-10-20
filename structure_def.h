#pragma once
#include "LOG.h"

typedef struct DllList {
    char dllName[MAX_PATH];
    struct DllList* next;
    struct DllList* prev;
} t_DllList;

//--------------------------------------------------------------------

typedef struct ProcessList {
    unsigned int sampleNum;
    char sampleTime[100];
    char processName[MAX_PATH];
    DWORD processID;
    DWORD PageFaultCount;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T PagefileUsage;
    t_DllList* DllList;
    unsigned int DllCount;
    struct ProcessList* next;
    struct ProcessList* prev;
} t_ProcessList;

//--------------------------------------------------------------------

typedef struct SampleList {
    t_ProcessList* ProcessList;
    t_ProcessList* HighestWorkingset;
    unsigned int sampleNum;
    unsigned int ProcessesCount;
    unsigned int DllCount;
    SIZE_T WorkingSetSum;
    SIZE_T WorkingSetAverage;
    char sampleTime[100];
    struct SampleList* next;
    struct SampleList* prev;
} t_SampleList;

//--------------------------------------------------------------------

typedef struct DllProcessesUsers {
    char ProcessName[MAX_PATH];
    struct DllProcessesUsers* next;
    struct DllProcessesUsers* prev;
} t_DllProcessesUsers;

//--------------------------------------------------------------------

typedef struct GeneralDllList {
    char dllName[MAX_PATH];
    t_DllProcessesUsers* DllUsersList;
    unsigned int AmountOfUsers;
    struct GeneralDllList* next;
    struct GeneralDllList* prev;
} t_GeneralDllList;

//--------------------------------------------------------------------

typedef struct FileHeader {
    int Version;
    int samples;
    char reserve[1000];
}t_FileHeader;