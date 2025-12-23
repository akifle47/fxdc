#pragma once
#define WIN32_LEAN_AND_MEAN
#include <cstdio>
#include <cstdlib>
#include <windows.h>

namespace Log
{
    template<typename ...Args>
    inline void Info(const char *fmt, Args ...args)
    {
        printf(fmt, args...);
        printf("\n");
    }

    template<typename ...Args>
    inline void Warn(const char *fmt, Args ...args)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);

        printf("Warning: ");
        printf(fmt, args...);
        printf("\n");

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    template<typename ...Args>
    inline void Error(const char *fmt, Args ...args)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        printf("ERROR: ");
        printf(fmt, args...);
        printf("\n");
     
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}