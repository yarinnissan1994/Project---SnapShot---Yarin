#pragma once
#include "SnapShots.h"

void AddingNewDllUsers(char ProcessName[MAX_PATH]);
void DllUsersListGenerator();
void AddingNewGeneralDll(char dllName[MAX_PATH]);
void GeneralDllListGenerator();
void Swap(t_ProcessList* Adress);
void SortSampleByDllAmount();
void HighestWorksetProcess();
void ResetHtmlDataCollections();
void HtmlSaveIntoFile(char* fileName, char* buff);
char* ReadAllFile(char* fileName);
void HomePgaeGenerator();
void SamplePgaeGenerator(t_SampleList* currentS);
void SamplePgaeGeneratorLoop();
void DllPgaeGenerator(t_GeneralDllList* currentD, unsigned int counter);
void DllPgaeGeneratorLoop();