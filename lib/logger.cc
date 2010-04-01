
#include "logger.h"

/*__thread*/ Logger *Logger::defaultLogger = NULL;


Logger::~Logger()
{
//	log<INFO_L>("logger ", " ### Shutting down logger...");
	if(fd > 2)
	{
		close(fd);
	}
}

