#include "log.h"
#include "include.h"

void LOG_SetStdOut(const char * _filename)
{
    int iFd = -1;

    iFd = open(_filename, O_RDWR | O_APPEND | O_CREAT, 0664);
    if (iFd >= 0)
    {
        dup2(iFd, 1);
        close(iFd);
    }
}

void LOG_SetStdErr(const char * _filename)
{
    int iFd = -1;

    iFd = open(_filename, O_RDWR | O_APPEND | O_CREAT, 0664);
    if (iFd >= 0)
    {
        dup2(iFd, 2);
        close(iFd);
    }
}
