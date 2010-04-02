
#include "logger.h"

Logger *Logger::defaultLogger = NULL;


Logger::~Logger()
{
	if (fd > 2)
	{
		close(fd);
	}
}

