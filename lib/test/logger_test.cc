
#include "stdlib.h"
#include "../logger.h"

int main(int argc, char *argv[])
{
    INIT_DEFAULT_LOGGER("logger_test.log");
    SET_LOGGER_LEVEL(argc > 1 ? atoi(argv[1]) : DEBUG_L);
    
    DLOG("() dlog %d", 1);
    ILOG("() ilog %d", 2);
    WLOG("() wlog %d", 3);
    ELOG("() elog %d", 4);
    SELOG("() selog %d", 5);
    
    fprintf(stdout, "\n\n\n");
    fflush(stdout);
}

