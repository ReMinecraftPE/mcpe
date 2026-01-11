#pragma once

#include <string>
#include <stdarg.h>

#ifdef ANDROID
#include <android/log.h>

enum eLogLevel
{
	LOG_INFO = ANDROID_LOG_INFO,
	LOG_WARN = ANDROID_LOG_WARN,
	LOG_ERR = ANDROID_LOG_ERROR
};
#else
enum eLogLevel
{
	LOG_INFO,
	LOG_WARN,
	LOG_ERR
};
#endif

class Logger
{
private:
	static Logger* m_singleton;
public:
	static Logger* singleton();
	static void setSingleton(Logger*);

	virtual ~Logger();

	const char* GetTag(eLogLevel ll);
	virtual void print(eLogLevel, const char* const str);
	virtual void print(eLogLevel, std::string str);
	virtual void vprintf(eLogLevel, const char* const fmt, va_list argPtr);
	virtual void printf(eLogLevel, const char* const fmt, ...);
};

#if !defined(NDEBUG) || defined(MC_LOG_LEVEL)

static inline void mc_vlog(enum eLogLevel loglevel, const char *fmt, va_list ap)
{
#ifdef ANDROID
	// TODO: Add a LoggerAndroid
	__android_log_vprint(loglevel, "ReMinecraftPE", fmt, ap);
#else
	Logger::singleton()->vprintf(loglevel, fmt, ap);
#endif
}

static inline void mc_log(enum eLogLevel loglevel, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	mc_vlog(loglevel, fmt, ap);
	va_end(ap);
}

static inline void mc_log_info(const char *fmt, ...)
{
	va_list	ap;
	va_start(ap, fmt);
	mc_vlog(LOG_INFO, fmt, ap);
	va_end(ap);
}

static inline void mc_log_warn(const char *fmt, ...)
{
	va_list	ap;
	va_start(ap, fmt);
	mc_vlog(LOG_WARN, fmt, ap);
	va_end(ap);
}

static inline void mc_log_err(const char *fmt, ...)
{
	va_list	ap;
	va_start(ap, fmt);
	mc_vlog(LOG_ERR, fmt, ap);
	va_end(ap);
}

#define LOG mc_log
#define LOG_I mc_log_info
#define LOG_W mc_log_warn
#define LOG_E mc_log_err

#else

static inline void mc_log(enum eLogLevel loglevel, const char *fmt, ...) {}
static inline void mc_log_nothing(const char *fmt, ...) {}

#define LOG if (0) mc_log
#define LOG_I if (0) mc_log_nothing
#define LOG_W if (0) mc_log_nothing
#define LOG_E if (0) mc_log_nothing

#endif
