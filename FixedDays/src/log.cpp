#include "log.h"
#include <windows.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>

void Log(const char* fmt, ...)
{
    char path[MAX_PATH]{};
    GetModuleFileNameA(nullptr, path, MAX_PATH);

    char* slash = strrchr(path, '\\');
    if (slash) lstrcpyA(slash + 1, "days_fix.log");

    FILE* f = nullptr;
    if (fopen_s(&f, path, "a") != 0 || !f) return;

    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);

    fputc('\n', f);
    fclose(f);
}
