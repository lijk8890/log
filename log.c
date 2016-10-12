#include "log.h"

enum log_level _log_level = LOG_LEVEL_INFO;

void log_open(const char *ident, bool tostderr)
{
    int option = LOG_PID;
    int facility = LOG_USER;

    if (tostderr) option |= LOG_PERROR;

    openlog(ident, option, facility);
}

void log_set_level(enum log_level level)
{
    _log_level = level;
}

void log_close(void)
{
    closelog();
}
