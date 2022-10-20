#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <psapi.h>
#pragma warning (disable : 4996)
#include "Files.h"

int main()
{
    //currect time string generator
    time_t t;
    time(&t);
    struct tm* timeinfo;
    timeinfo = localtime(&t);

    //dynamic Log file name generetor
    char logFileName[50];
    sprintf(logFileName, "LOG_%d-%d-%d_%d-%d-%d.log", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    LogInit(logFileName);
    LogEvent("program started");

    char ind;
    do
    {
        printf("*********MENU*********\n~ Type Option Number For Execution ~\n1.Take One SnapShot.\n2.Take 20 Seconds SnapShot.\n3.Take Long SnapShot.\n4.Generate HTML Report.\n5.Reset Collections.\n6.Save in File.\n7.Load from File.\n8.Quit\ntype here:");
        ind = getch();
        printf("\n\n");
        switch (ind)
        {
        case '1':
            LogEvent("taking single SnapShot");
            SingleSnapShot(1);
            LogEvent("done single SnapShot");
            break;
        case '2':
            LogEvent("taking 20 seconds SnapShot");
            SnapShot20();
            LogEvent("done 20 seconds SnapShot");
            break;
        case '3':
            LogEvent("taking long SnapShot");
            LongSnapShot();
            LogEvent("done long SnapShot");
            break;
        case '4':
            LogEvent("Generate HTML starts");
            if (SampleHead)
            {
                printf("Generating Web Report\n\n");
                GeneralDllListGenerator();
                SortSampleByDllAmount();
                HighestWorksetProcess();
                HomePgaeGenerator();
                SamplePgaeGeneratorLoop();
                DllPgaeGeneratorLoop();
                ResetHtmlDataCollections();
                printf("DONE!\n\n");
            }
            else
            {
                printf("Faield To Generate Web Report - No Collections Found\n\n");
                LogError("sample list is empty - no data to use");
            }
            LogEvent("Generate HTML ends");
            break;
        case '5':
            LogEvent("reset collections starts");
            if (SampleHead)
            {
                printf("Cleaning All Collections\n\n");
                ResetCollections();
                printf("DONE!\n\n");
            }
            else
            {
                printf("Faield To Reset - No Collections Found\n\n");
                LogError("sample list is empty - no data to reset");
            }
            LogEvent("reset collections ends");
            break;
        case '6':
            LogEvent("save starts");
            if (SampleHead)
            {
                printf("Saving File\n\n");
                SaveIntoFile();
                printf("DONE!\n\n");
            }
            else
            {
                printf("Faield To Save File - No Collections Found\n\n");
                LogError("sample list is empty - no data to save");
            }
            LogEvent("save ends");
            break;
        case '7':
            LogEvent("load starts");
            printf("~ Load options ~\n1. Load - \"%s\"\n2. Load other file\n\n", FileNameHolder);
            char option = getch();
            if (option == '1')
            {
                if (!strcmp(FileNameHolder, "last file is empty"))
                {
                    printf("last file is empty\n\n");
                    break;
                }
                printf("Loading File\n\n");
                LoadFile();
                printf("DONE!\n\n");
            }
            else if (option == '2')
            {
                printf("write file name without spaces:");
                gets(FileNameHolder);
                printf("Loading File\n\n");
                int errorInd = LoadFile();
                if (errorInd)
                {
                    printf("file dosent found\n\n");
                }
                printf("DONE!\n\n");
            }
            LogEvent("load ends");
            break;
        case '8':
            LogEvent("quit starts");
            printf("thank u for using my program - see u again soon =]\n\n");
            ResetCollections();
            LogEvent("quit ends");
            break;
        default:
            printf("invalid key");
            printf("\n\n");
            break;
        }
    } while (ind != '8');
    LogEvent("program ended");
	return 0;
}