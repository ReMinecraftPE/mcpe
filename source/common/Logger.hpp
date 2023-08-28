#pragma once

#include <string>

enum eLogLevel
{
	LOG_INFO,
	LOG_WARN,
	LOG_ERR,
};

class Logger
{
private:
	static Logger* m_singleton;
public:
	static Logger* const singleton();

	Logger();
	~Logger();

	const char* GetTag(eLogLevel ll);
	virtual void print(eLogLevel, const char* const str);
	virtual void print(eLogLevel, std::string str);
	virtual void vprintf(eLogLevel, const char* const fmt, va_list argPtr);
	virtual void printf(eLogLevel, const char* const fmt, ...);
};

#ifdef _DEBUG

#define LOG(level, ...) Logger::singleton()->printf(level, __VA_ARGS__)

#ifdef PLATFORM_ANDROID
// TODO: Add a LoggerAndroid
#define LOG(ignored, ...) __android_log_print(ANDROID_LOG_DEFAULT, "ReMinecraftPE", __VA_ARGS__)
#define LOG_I(...)        __android_log_print(ANDROID_LOG_INFO,    "ReMinecraftPE", __VA_ARGS__)
#define LOG_W(...)        __android_log_print(ANDROID_LOG_WARN,    "ReMinecraftPE", __VA_ARGS__)
#define LOG_E(...)        __android_log_print(ANDROID_LOG_ERROR,   "ReMinecraftPE", __VA_ARGS__)
#else
#define LOG_I(...) LOG(LOG_INFO, __VA_ARGS__)
#define LOG_W(...) LOG(LOG_WARN, __VA_ARGS__)
#define LOG_E(...) LOG(LOG_ERR,  __VA_ARGS__)
#endif

#else

#define LOG(...)
#define LOG_I(...)
#define LOG_W(...)
#define LOG_E(...)

#endif
