#include <stdio.h>
#include <libgen.h>
#include "log.h"

int main(int argc, char *argv[])
{
    char *filename = NULL;
    char *pathname = NULL;

    filename = basename(argv[0]);
    pathname = dirname(argv[0]);

    log_open(filename, true);

    log_debug("%s: %s %s", argv[0], pathname, filename);
    log_info("%s: %s %s", argv[0], pathname, filename);
    log_err("%s: %s %s", argv[0], pathname, filename);

    log_close();

    return 0;
}
