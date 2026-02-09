#define WIN32_LEAN_AND_MEAN

#include "Log.h"

#include <cstdio>
#include <cstdarg>
#include <windows.h>
#include <mutex>

std::mutex sMutex;

namespace Log
{
    void Info(const char* fmt, ...)
    {
        std::lock_guard guard {sMutex};

        va_list args;
        va_start(args, fmt);

        vprintf(fmt, args);
        printf("\n");

        va_end(args);
    }

    void Warn(const char* fmt, ...)
    {
        std::lock_guard guard{sMutex};

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);

        va_list args;
        va_start(args, fmt);

        vprintf(fmt, args);
        printf("\n");

        va_end(args);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    void Error(const char* fmt, ...)
    {
        std::lock_guard guard{sMutex};

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        va_list args;
        va_start(args, fmt);

        printf("Error: ");
        vprintf(fmt, args);
        printf("\n");

        va_end(args);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}