//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <time.h>
//#include <conio.h>
//#include <windows.h>
//#include <psapi.h>
//#pragma warning (disable : 4996)
//#include "LOG.h"
//#include "structure_def.h"
//
//int main()
//{
//    //currect time string generator
//    time_t t;
//    time(&t);
//    struct tm* timeinfo;
//    timeinfo = localtime(&t);
//
//    //dynamic file name generetor
//    char logFileName[50];
//    sprintf(logFileName, "Report_%d-%d-%d_%d-%d-%d.bin", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
//    LogInit("ProjectSnapshot.log");
//    LogEvent("program started");
//
//    strcpy(FileNameHolder, "test.test");
//
//    char ind;
//    do
//    {
//        printf("*********MENU*********\n~ Type Option Number For Execution ~\n1.Take One SnapShot.\n2.Take 20 Seconds SnapShot.\n3.Take Long SnapShot.\n4.Generate HTML Report.\n5.Reset Collections.\n6.Save in File.\n7.Load from File.\n8.Quit\ntype here:");
//        ind = getch();
//        printf("\n\n");
//        switch (ind)
//        {
//        case '1':
//            //SingleSnapShot();
//            break;
//        case '2':
//            //SnapShot20();
//            break;
//        case '3':
//            //LongSnapShot();
//            break;
//        case '4':
//            break;
//        case '5':
//            //ResetCollections();
//            break;
//        case '6':
//            //SaveIntoFile();
//            break;
//        case '7':
//            printf("~ Load options ~\n1. Load - \"%s\"\n2. Load other file\n", FileNameHolder);
//            char option = getch();
//            if (option == '1')
//            {
//                //LoadFile();
//            }
//            else if (option == '2')
//            {
//                printf("write file name without spaces:");
//                gets(FileNameHolder);
//                //LoadFile();
//            }
//            break;
//        case '8':
//            break;
//        default:
//            printf("invalid key");
//            printf("\n\n");
//            break;
//        }
//    } while (ind != '8');
//    LogEvent("program ended");
//	return 0;
//}