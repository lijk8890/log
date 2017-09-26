#include "log.h"

enum log_level _log_level = LOG_LEVEL_INFO;

void log_open(const char *ident, bool tostderr)
{
    int option = LOG_CONS | LOG_PID;
    int facility = LOG_USER;

    if (tostderr) option |= LOG_PERROR;

    openlog(ident, option, facility);
}

// level: crit, err, warning, notice, info, debug
enum log_level log_get_level(const char *level)
{
    if(level == NULL)
        return LOG_LEVEL_INFO;

    if(strcasecmp(level, "crit") == 0)
        return LOG_LEVEL_CRIT;
    else if(strcasecmp(level, "err") == 0)
        return LOG_LEVEL_ERR;
    else if(strcasecmp(level, "warning") == 0)
        return LOG_LEVEL_WARNING;
    else if(strcasecmp(level, "notice") == 0)
        return LOG_LEVEL_NOTICE;
    else if(strcasecmp(level, "info") == 0)
        return LOG_LEVEL_INFO;
    else if(strcasecmp(level, "debug") == 0)
        return LOG_LEVEL_DEBUG;
    else
        return LOG_LEVEL_INFO;
}

void log_set_level(enum log_level level)
{
    _log_level = level;
}

void log_close(void)
{
    closelog();
}

/**
 * hex_dump_to_buffer - convert a blob of data to "hex ASCII" in memory
 * @buf: data blob to dump
 * @len: number of bytes in the @buf
 * @rowsize: number of bytes to print per line; must be 16 or 32
 * @groupsize: number of bytes to print at a time (1, 2, 4, 8; default = 1)
 * @linebuf: where to put the converted data
 * @linebuflen: total size of @linebuf, including space for terminating NUL
 * @ascii: include ASCII after the hex output
 *
 * hex_dump_to_buffer() works on one "line" of output at a time, i.e.,
 * 16 or 32 bytes of input data converted to hex + ASCII output.
 *
 * Given a buffer of uint8_t data, hex_dump_to_buffer() converts the input data
 * to a hex + ASCII dump at the supplied memory location.
 * The converted output is always NUL-terminated.
 *
 * E.g.:
 *   hex_dump_to_buffer(frame->data, frame->len, 16, 1,
 *          linebuf, sizeof(linebuf), 1);
 *
 * example output buffer:
 * 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f  @ABCDEFGHIJKLMNO
 */
void hex_dump_to_buffer(const void *buf, size_t len, int rowsize,
            int groupsize, char *linebuf, size_t linebuflen,
            bool ascii)
{
    const char hex_asc[] = "0123456789abcdef";
    #define hex_asc_lo(x)   hex_asc[((x) & 0x0f)]
    #define hex_asc_hi(x)   hex_asc[((x) & 0xf0) >> 4]

    const uint8_t *ptr = buf;
    uint8_t ch;
    int j, lx = 0;
    int ascii_column;

    if (rowsize != 16 && rowsize != 32)
        rowsize = 16;

    if (!len)
        goto nil;
    if (len > rowsize)      /* limit to one line at a time */
        len = rowsize;
    if ((len % groupsize) != 0) /* no mixed size output */
        groupsize = 1;

    switch (groupsize) {
    case 8: {
        const uint64_t *ptr8 = buf;
        int ngroups = len / groupsize;

        for (j = 0; j < ngroups; j++)
            lx += snprintf(linebuf + lx, linebuflen - lx,
                "%s%16.16llx", j ? " " : "",
                (unsigned long long)*(ptr8 + j));
        ascii_column = 17 * ngroups + 2;
        break;
    }

    case 4: {
        const uint32_t *ptr4 = buf;
        int ngroups = len / groupsize;

        for (j = 0; j < ngroups; j++)
            lx += snprintf(linebuf + lx, linebuflen - lx,
                "%s%8.8x", j ? " " : "", *(ptr4 + j));
        ascii_column = 9 * ngroups + 2;
        break;
    }

    case 2: {
        const uint16_t *ptr2 = buf;
        int ngroups = len / groupsize;

        for (j = 0; j < ngroups; j++)
            lx += snprintf(linebuf + lx, linebuflen - lx,
                "%s%4.4x", j ? " " : "", *(ptr2 + j));
        ascii_column = 5 * ngroups + 2;
        break;
    }

    default:
        for (j = 0; (j < len) && (lx + 3) <= linebuflen; j++) {
            ch = ptr[j];
            linebuf[lx++] = hex_asc_hi(ch);
            linebuf[lx++] = hex_asc_lo(ch);
            linebuf[lx++] = ' ';
        }
        if (j)
            lx--;

        ascii_column = 3 * rowsize + 2;
        break;
    }
    if (!ascii)
        goto nil;

    while (lx < (linebuflen - 1) && lx < (ascii_column - 1))
        linebuf[lx++] = ' ';
    for (j = 0; (j < len) && (lx + 2) < linebuflen; j++)
        linebuf[lx++] = (isascii(ptr[j]) && isprint(ptr[j])) ? ptr[j]
                : '.';
nil:
    linebuf[lx++] = '\0';
}

void print_hex_dump(enum log_level level, const void *buf, size_t len)
{
    #define min(x,y) ({ \
            typeof(x) _x = (x);     \
            typeof(y) _y = (y);     \
            (void) (&_x == &_y);    \
            _x < _y ? _x : _y; })

    #define max(x,y) ({ \
            typeof(x) _x = (x);     \
            typeof(y) _y = (y);     \
            (void) (&_x == &_y);    \
            _x > _y ? _x : _y; })

    const uint8_t *ptr = buf;
    int i, linelen, remaining = len;
    char linebuf[200];

    int rowsize = 16;
    int groupsize = 1;
    bool ascii = true;

    if(buf == NULL || len <= 0)
        return;

    for (i = 0; i < len; i += rowsize) {
        linelen = min(remaining, rowsize);
        remaining -= rowsize;
        hex_dump_to_buffer(ptr + i, linelen, rowsize, groupsize,
                linebuf, sizeof(linebuf), ascii);

        switch (level) {
        case LOG_LEVEL_CRIT:
            log_crit("0x%08x:  %s", i, linebuf);
            break;
        case LOG_LEVEL_ERR:
            log_err("0x%08x:  %s", i, linebuf);
            break;
        case LOG_LEVEL_WARNING:
            log_warning("0x%08x:  %s", i, linebuf);
            break;
        case LOG_LEVEL_NOTICE:
            log_notice("0x%08x:  %s", i, linebuf);
            break;
        case LOG_LEVEL_INFO:
            log_info("0x%08x:  %s", i, linebuf);
            break;
        case LOG_LEVEL_DEBUG:
            log_debug("0x%08x:  %s", i, linebuf);
            break;
        default:
            printf("0x%08x:  %s\n", i, linebuf);
            break;
        }
    }
}
