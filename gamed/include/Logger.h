#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <stdio.h>

class Logger
{
public:
    static Logger& instance()
    {
        static Logger instance;
        return instance;
    }

    ~Logger();

    void setLogFile(const char* filename);

    void log(const std::string& tag, const char* funcName, const char* sourceFile, 
        unsigned int lineNum, const std::string& fmt, ...);

    /**
     * @brief Flushes output buffer. Called for critical events
     */
    void flush();

    // Used for error logging
    //static void error(bool isFatal, const std::string& msg,
    //     const char* funcName, const char* sourceFile, unsigned int lineNum);

protected:
    Logger();

private:
    Logger(const Logger&);
    Logger& operator=(const Logger&);

    void fillOutputBuffer(std::string& outputBuffer,
            const std::string& tag, const std::string& msg,
          const char* funcName, const char* sourceFile, unsigned int lineNum);

    inline FILE* loadFileStream(FILE* stream, const char* filename);

    FILE* m_pLogFile;
};


#ifndef NDEBUG // If in debug mode

// Define debugger break symbols
#ifdef __unix__ // Linux & BSD

// For SIGTRAP signal
#include <signal.h>

#define DEBUG_BREAK raise(SIGTRAP)

#elif _WIN32 // Windows 

#define DEBUG_BREAK __debugbreak()

#else

#warning "Unknown Operating System, debug break support will not be included"

// Remove support for debug break on other platforms
#define DEBUG_BREAK 0

#endif

// Note, the do-while removes the ; at the end of the call
#define CORE_ASSERT(expr) \
	do \
	{ \
		if(!(expr)) \
		{ \
			Logger::instance().log("ASSERT", __FUNCTION__, __FILE__, __LINE__, #expr); \
			Logger::instance().flush(); \
            DEBUG_BREAK; \
		} \
	} \
	while(0)\

#define CORE_FATAL(fmt, ...) \
    do \
    { \
        Logger::instance().log("FATAL", __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__);\
        Logger::instance().flush(); \
        DEBUG_BREAK; \
        std::terminate(); \
    } \
    while(0)\

#define CORE_ERROR(fmt, ...) \
    do \
    { \
        Logger::instance().log("ERROR", __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
        Logger::instance().flush(); \
    } \
    while(0)\


#define CORE_WARNING(fmt, ...) \
    do \
    { \
        Logger::instance().log("WARNING", __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__);\
        Logger::instance().flush(); \
    } \
    while(0)\


#define CORE_DEBUG(fmt, ...) \
    do \
    { \
        Logger::instance().log("DEBUG", NULL, NULL, 0, fmt, ##__VA_ARGS__); \
        Logger::instance().flush(); \
    } \
    while(0) \

#define CORE_LOG(tag, fmt, ...) \
    do \
    { \
        Logger::instance().log(tag, NULL, NULL, 0, fmt, ##__VA_ARGS__); \
        Logger::instance().flush(); \
    } \
    while(0) \


#define CORE_VERBOSE(fmt, ...) \
    do \
    { \
        Logger::instance().log("VERBOSE", __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } \
    while(0) \

#else // Not in debug mode

// Keep errors and warnings, just exclude func, file and line info
#define CORE_FATAL(fmt, ...) \
    do \
    { \
        Logger::instance().log("FATAL", NULL, NULL, 0, fmt, ##__VA_ARGS__); \
        Logger::instance().flush(); \
        std::terminate(); \
    } \
    while(0)\


#define CORE_ERROR(fmt, ...) \
    do \
    { \
        Logger::instance().log("ERROR", NULL, NULL, 0, fmt, ##__VA_ARGS__); \
        Logger::instance().flush(); \
    } \
    while(0)\


#define CORE_WARNING(fmt, ...) \
    do \
    { \
        Logger::instance().log("WARNING", NULL, NULL, 0, fmt, ##__VA_ARGS__);\
    } \
    while(0)\

// Release mode definitions of macros. Defined in such a way as to be
// ignored completelly by the compiler
#define CORE_DEBUG(fmt, ...) do { (void)sizeof(fmt##__VA_ARGS__); } while(0)
#define CORE_VERBOSE(fmt, ...) do { (void)sizeof(fmt##__VA_ARGS__); } while(0)
#define CORE_LOG(tag, fmt, ...) do { (void)sizeof(fmt##__VA_ARGS__); } while(0)
#define CORE_ASSERT(expr) do { (void)sizeof(expr); } while(0)

#endif

#define CORE_INFO(fmt, ...) \
    do \
    { \
        Logger::instance().log("INFO", NULL, NULL, 0, fmt, ##__VA_ARGS__); \
    } \
    while(0) \


#endif
