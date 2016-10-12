#include <stdio.h>
#include <libgen.h>
#include "log.h"

int main(int argc, char *argv[])
{
    char *filename = NULL;
    char *pathname = NULL;
    unsigned char data[] = "\x78\x64\x6a\x61\x44\x32\x42\x33\x32\x31\x31\x31\x00\x03\x1a\x91\x78\x64\x6a\x61\x44\x32\x42\x33\x32\x31\x31\x31\x00\x03\x1a\x91";

    filename = basename(argv[0]);
    pathname = dirname(argv[0]);

    log_open(filename, true);

    log_debug("%s: %s %s", argv[0], pathname, filename);
    log_info("%s: %s %s", argv[0], pathname, filename);
    log_err("%s: %s %s", argv[0], pathname, filename);

    log_dump(data, 32);
    log_print(data, 32);

    log_close();

    return 0;
}
