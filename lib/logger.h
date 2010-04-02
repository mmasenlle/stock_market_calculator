
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
 * STD_ERROR also logs over stderr when lev <= ERROR
 * PRINT_TAG prints LOGGER_TAG
 * PRINT_TID prints tid
 * PRINT_MILIS appends miliseconds to timestamp
 * PRINT_FUNC prepends the function name to the log's string
 */
#define START_LEVEL 2
#ifndef LOG_COMPILATION_LEVEL
#	define LOG_COMPILATION_LEVEL DEBUG_L
#	undef START_LEVEL
#	define START_LEVEL DEBUG_L
#endif
//#define STD_OUTPUT
//#define STD_ERROR
//#define PRINT_TAG
//#define PRINT_TID
#define PRINT_MILIS
#define PRINT_FUNC

class Logger
{
public:
	int fd;
	int level;
	static Logger *defaultLogger;
	Logger(const char *fn = NULL);
	~Logger();
	template<int lev> void log(const char *tag, const char *fmt, ...);
	void dump(const char *buf, int len) { int ignored = ::write(fd, buf, len); };
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
void Logger::log(
#ifdef PRINT_FUNC
	const char *func,
#endif
	const char *fmt, ...)
{
	struct timeval tv;
	struct tm tm;
	gettimeofday( &tv, NULL );
	char buf[1024];
	localtime_r( &tv.tv_sec, &tm );
	strftime( buf, sizeof( buf ), "%F %k:%M:%S", &tm );
	std::ostringstream aux;
	aux << "[";
#if defined(PRINT_TAG) && defined(LOGGER_TAG)
	aux << LOGGER_TAG;
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
		aux << "[SYSERR] " << errno << " '" <<
			strerror_r(errno, buf, sizeof(buf)) << "': ";
		break;
	case ERROR_L: aux << "[ERROR] "; break;
	case WARN_L:  aux << "[WARN ] "; break;
	case INFO_L:  aux << "[INFO ] "; break;
	case DEBUG_L: aux << "[DEBUG] "; break;
	}
#ifdef PRINT_FUNC
	aux << func;
#endif
	va_list args;
	va_start( args, fmt );
	vsnprintf( buf, sizeof( buf ), fmt, args );
	va_end( args );
	aux << buf << "\n";
	int ignored = write(fd, aux.str().c_str(), aux.str().length());
	
#if defined(STD_OUTPUT) && (LOG_COMPILATION_LEVEL >= DEBUG_L)
	if (fd > 1)
		int ignored = write(1, aux.str().c_str(), aux.str().length());
#elif defined(STD_ERROR)
	if (lev <= ERROR_L && fd > 2)
		int ignored = write(2, aux.str().c_str(), aux.str().length());
#endif
}

#define INIT_DEFAULT_LOGGER if (!Logger::defaultLogger) Logger::defaultLogger = new Logger
#define END_DEFAULT_LOGGER delete Logger::defaultLogger; Logger::defaultLogger = NULL
#define SET_LOGGER_LEVEL(l) if (Logger::defaultLogger) Logger::defaultLogger->level = l

#ifdef PRINT_FUNC
#define FUNC_ARG __FUNCTION__,
#else
#define FUNC_ARG
#endif
#define __PRE_LOG(lev) do { if (__builtin_expect(!!(Logger::defaultLogger), 1) && \
    __builtin_expect((Logger::defaultLogger->level >= lev), 0)) \
        Logger::defaultLogger->log<lev> ( FUNC_ARG

#define SELOG(fmt, args...) __PRE_LOG(SYSERR_L) fmt, ## args) ; } while(0)
#define ELOG(fmt, args...)  __PRE_LOG(ERROR_L)  fmt, ## args) ; } while(0)
#define WLOG(fmt, args...)
#define ILOG(fmt, args...)
#define DLOG(fmt, args...)
#define DUMPLOG(buf, len)

#if LOG_COMPILATION_LEVEL >= WARN_L
#	undef WLOG
#	define WLOG(fmt, args...) __PRE_LOG(WARN_L) fmt, ## args) ; } while(0)
#	if LOG_COMPILATION_LEVEL >= INFO_L
#		undef ILOG
#		define ILOG(fmt, args...) __PRE_LOG(INFO_L) fmt, ## args) ; } while(0)
#		if LOG_COMPILATION_LEVEL >= DEBUG_L
#			undef DLOG
#			define DLOG(fmt, args...) __PRE_LOG(DEBUG_L) fmt, ## args) ; } while(0)
#			undef DUMPLOG
#			define DUMPLOG(buf, len) do { if (Logger::defaultLogger) Logger::defaultLogger->dump(buf, len); } while(0)
#		endif
#	endif
#endif

#endif

