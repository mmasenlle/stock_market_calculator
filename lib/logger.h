
#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sstream>

#define SYSERR_L	1
#define ERROR_L		2
#define WARN_L 		3
#define INFO_L 		4
#define DEBUG_L		5

/*
 * Configuration of the logger behavior
 * LOG_COMPILATION_LEVEL usefull with macros
 * STD_OUTPUT also logs over stdout when level == DEBUG
 * STD_ERROR also logs over stderr when level == ERROR
 * PRINT_MILIS prints also miliseconds in the log line
 * PRINT_TID adds tid to the tag in the log line
 * LOG_TAG redefine this macro with apropiate value per module
 */
#define START_LEVEL 0
#ifndef LOG_COMPILATION_LEVEL
#	define LOG_COMPILATION_LEVEL	DEBUG_L
#	undef START_LEVEL
#	define START_LEVEL DEBUG_L
#endif
//#define STD_OUTPUT
//#define STD_ERROR
#define PRINT_MILIS
//#define PRINT_TID
//#define PRINT_TAG

class Logger
{
public:
	int fd;
	int level;
	static /*__thread*/ Logger *defaultLogger;
	Logger(const char *fn = NULL);
	~Logger();
	template<int lev> void log(const char *tag, const char *fmt, ...);
	void dump(const char *buf, int len) { ::write(fd, buf, len); };
};

inline Logger::Logger(const char *fn)
 : level(START_LEVEL), fd(1)
{
	if (fn)
	{
		if ((fd = open(fn, O_WRONLY | O_CREAT | O_APPEND, 0660)) == -1)
		{
			fd = 1;
			log<SYSERR_L>("logger ", "opening logger file '%s', setting stdout as output", fn);
		}
	}
}

template<int lev>
void Logger::log(const char *tag, const char *fmt, ...)
{
	struct timeval tv;
	struct tm tm;
	gettimeofday( &tv, NULL );
	char buf[1024];
	localtime_r( &tv.tv_sec, &tm );
	strftime( buf, sizeof( buf ), "%F %k:%M:%S", &tm );
	std::ostringstream aux;
	aux << "[";
#ifdef PRINT_TAG
	<< tag;
#endif
#ifdef PRINT_TID	
	aux << "(" << (unsigned int)pthread_self() << ") ";
#endif
	aux << buf;
#ifdef PRINT_MILIS
	sprintf(buf, "%03lu", tv.tv_usec / 1000);
	aux  << "," << buf;
#endif
	aux << "] ";
	switch (lev)
	{
	case SYSERR_L:
		aux << "[SYSERR] errno: " << errno << " '" <<
			strerror_r(errno, buf, sizeof(buf)) << "' => ";
		break;
	case ERROR_L: aux << "[ERROR] "; break;
	case WARN_L:  aux << "[WARN ] "; break;
	case INFO_L:  aux << "[INFO ] "; break;
	case DEBUG_L: aux << "[DEBUG] "; break;
	default:		  aux << "-***** -> "; break;
	}
	va_list args;
	va_start( args, fmt );
	vsnprintf( buf, sizeof( buf ), fmt, args );
	va_end( args );
	aux << buf << "\n";
	write(fd, aux.str().c_str(), aux.str().length());
	
#if defined(STD_OUTPUT) && (LOG_COMPILATION_LEVEL >= DEBUG_L)
	if (fd > 1)
		write(1, aux.str().c_str(), aux.str().length());
#elif defined(STD_ERROR)
	if (lev <= ERROR_L && fd > 2)
		write(2, aux.str().c_str(), aux.str().length());
#endif
}

#define INIT_DEFAULT_LOGGER if (!Logger::defaultLogger) Logger::defaultLogger = new Logger
#define END_DEFAULT_LOGGER delete Logger::defaultLogger; Logger::defaultLogger = NULL
#define SET_LOGGER_LEVEL(l) if (Logger::defaultLogger) Logger::defaultLogger->level = l

#define LOG_TAG "--"

#define __PRE_LOG(lev) do { if (__builtin_expect(!!(Logger::defaultLogger), 1) && __builtin_expect((Logger::defaultLogger->level >= lev), 0)) Logger::defaultLogger->log<lev>

#define SELOG(fmt, args...) __PRE_LOG(SYSERR_L) (LOG_TAG, fmt, ## args) ; } while(0)
#define ELOG(fmt, args...)  __PRE_LOG(ERROR_L)  (LOG_TAG, fmt, ## args) ; } while(0)
#define WLOG(fmt, args...)
#define ILOG(fmt, args...)
#define DLOG(fmt, args...)
#define DUMPLOG(buf, len)

#if LOG_COMPILATION_LEVEL >= WARN_L
#	undef WLOG
#	define WLOG(fmt, args...) __PRE_LOG(WARN_L) (LOG_TAG, fmt, ## args) ; } while(0)
#	if LOG_COMPILATION_LEVEL >= INFO_L
#		undef ILOG
#		define ILOG(fmt, args...) __PRE_LOG(INFO_L) (LOG_TAG, fmt, ## args) ; } while(0)
#		if LOG_COMPILATION_LEVEL >= DEBUG_L
#			undef DLOG
#			define DLOG(fmt, args...) __PRE_LOG(DEBUG_L) (LOG_TAG, fmt, ## args) ; } while(0)
#			undef DUMPLOG
#			define DUMPLOG(buf, len) do { if (Logger::defaultLogger) Logger::defaultLogger->dump(buf, len); } while(0)
#		endif
#	endif
#endif

#endif

