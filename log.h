#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <syslog.h>

enum log_level
{
    LOG_LEVEL_CRIT,
    LOG_LEVEL_ERR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_NOTICE,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_NUM
};

extern enum log_level _log_level;

#define _log_message(level, priority, format, ...)                                                              \
do{                                                                                                             \
    level > _log_level ? 0 : syslog(priority, #priority" [%s:%u] "format, __FILE__, __LINE__, ##__VA_ARGS__);   \
}while(0)

#define log_crit(format, ...)       _log_message(LOG_LEVEL_CRIT, LOG_CRIT, format, ##__VA_ARGS__)
#define log_err(format, ...)        _log_message(LOG_LEVEL_ERR, LOG_ERR, format, ##__VA_ARGS__)
#define log_warning(format, ...)    _log_message(LOG_LEVEL_WARNING, LOG_WARNING, format, ##__VA_ARGS__)
#define log_notice(format, ...)     _log_message(LOG_LEVEL_NOTICE, LOG_NOTICE, format, ##__VA_ARGS__)
#define log_info(format, ...)       _log_message(LOG_LEVEL_INFO, LOG_INFO, format, ##__VA_ARGS__)
#define log_debug(format, ...)      _log_message(LOG_LEVEL_DEBUG, LOG_DEBUG, format, ##__VA_ARGS__)

#define log_alarm(format, ...)      _log_message(LOG_LEVEL_WARNING, LOG_LOCAL0|LOG_WARNING, format, ##__VA_ARGS__)
#define log_audit(format, ...)      _log_message(LOG_LEVEL_NOTICE, LOG_LOCAL1|LOG_NOTICE, format, ##__VA_ARGS__)

void log_open(const char *ident, bool tostderr);

void log_set_level(enum log_level level);

void log_close(void);

#endif
