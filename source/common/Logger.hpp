#pragma once

#include <string>

#ifdef ANDROID
#include <android/log.h>

enum eLogLevel
{
	LOG_INFO = ANDROID_LOG_INFO,
	LOG_WARN = ANDROID_LOG_WARN,
	LOG_ERR = ANDROID_LOG_ERROR,
};
#else
enum eLogLevel
{
	LOG_INFO,
	LOG_WARN,
	LOG_ERR,
};
#endif

class Logger
{
private:
	static Logger* m_singleton;
public:
	static Logger* const singleton();
	static void setSingleton(Logger*);

	virtual ~Logger();

	const char* GetTag(eLogLevel ll);
	virtual void print(eLogLevel, const char* const str);
	virtual void print(eLogLevel, std::string str);
	virtual void vprintf(eLogLevel, const char* const fmt, va_list argPtr);
	virtual void printf(eLogLevel, const char* const fmt, ...);
};

#ifndef NDEBUG

#ifdef ANDROID
// TODO: Add a LoggerAndroid
#define LOG(level, ...) __android_log_print(level, "ReMinecraftPE", __VA_ARGS__)
#else
#define LOG(level, ...) Logger::singleton()->printf(level, __VA_ARGS__)
#endif

#define LOG_I(...) LOG(LOG_INFO, __VA_ARGS__)
#define LOG_W(...) LOG(LOG_WARN, __VA_ARGS__)
#define LOG_E(...) LOG(LOG_ERR,  __VA_ARGS__)

#else

#define LOG(...)	((void)0)
#define LOG_I(...)	((void)0)
#define LOG_W(...)	((void)0)
#define LOG_E(...)	((void)0)

#endif
