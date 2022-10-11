#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#pragma warning (disable : 4996)
#include "LOG.h"

char Log_file_name[1000];

void LogInit(char fileName[1000])
{
    strcpy(Log_file_name, fileName);
}

void Log(char message[1000])
{
    time_t t;
    time(&t);
    struct tm* timeinfo;
    timeinfo = localtime(&t);
    char time[1000];

    sprintf(time, "%d.%d.%d-%d:%d:%d - %s\n\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, message);
    FILE* f = fopen(Log_file_name, "a");
    fputs(time, f);
    fclose(f);
}

void LogError(char message[100])
{
    char error[100] = "Error - ";
    strcat(error, message);
    Log(error);
}

void LogWarning(char message[100])
{
    char warning[100] = "Warning - ";
    strcat(warning, message);
    Log(warning);
}

void LogEvent(char message[100])
{
    char event[100] = "Event - ";
    strcat(event, message);
    Log(event);
}