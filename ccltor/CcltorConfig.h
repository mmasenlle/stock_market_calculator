#ifndef _CCLTORCONFIG_H_
#define _CCLTORCONFIG_H_

#include <string>

class CcltorConfig
{
protected:
    static void init_pre(int argc, char *argv[]);
    static void init_post();
public:
    int log_level;


};

#endif

