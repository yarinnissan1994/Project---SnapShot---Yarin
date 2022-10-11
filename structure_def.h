//--------------------------------------------------------------------

typedef struct DllList {
    char dllName[MAX_PATH];
    struct DllList* next;
    struct DllList* prev;
} t_DllList;

t_DllList* DllHead = NULL;
t_DllList* DllTail = NULL;

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

t_ProcessList* ProcessHead = NULL;
t_ProcessList* ProcessTail = NULL;

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

t_SampleList* SampleHead = NULL;
t_SampleList* SampleTail = NULL;

//--------------------------------------------------------------------

typedef struct DllProcessesUsers {
    char ProcessName[MAX_PATH];
    struct DllProcessesUsers* next;
    struct DllProcessesUsers* prev;
} t_DllProcessesUsers;

t_DllProcessesUsers* DllProcessesHead = NULL;
t_DllProcessesUsers* DllProcessesTail = NULL;

//--------------------------------------------------------------------

typedef struct GeneralDllList {
    char dllName[MAX_PATH];
    t_DllProcessesUsers* DllUsersList;
    unsigned int AmountOfUsers;
    struct GeneralDllList* next;
    struct GeneralDllList* prev;
} t_GeneralDllList;

t_GeneralDllList* GeneralDllHead = NULL;
t_GeneralDllList* GeneralDllTail = NULL;

//--------------------------------------------------------------------

typedef struct FileHeader {
    int Version;
    int samples;
    char reserve[1000];
}t_FileHeader;

//--------------------------------------------------------------------

unsigned int sampleCount = 0;
unsigned int processCount = 0;
unsigned int dllCount = 0;
unsigned int sampleDllCount = 0;
unsigned int GeneralDllCount = 0;
unsigned int DllUsersCount = 0;

char FileNameHolder[50];