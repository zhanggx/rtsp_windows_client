#include "Log.h"


#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/async.h>

#define LOG_TAG         "MediaLibLog"
#define DEF_LOG_BUF_SIZE (1024 * 8)


std::shared_ptr<Log> Log::m_instance_ptr;
std::mutex Log::m_mutex;


std::string getAppDataRoamingPath()
{
    char roaming[MAX_PATH] = { 0 };
    DWORD size = GetEnvironmentVariableA("appdata", roaming, MAX_PATH);
    if (size >= MAX_PATH || size == 0) {
        //std::cout << "fail to get %appdata%: " << GetLastError() << std::endl;
        return "";
    }

    std::string result(roaming, size);
    result += "\\MediaLiveClient";
    return result;// string2MMKVPath_t(result);
}
Log::Log()
{

	std::string path = getAppDataRoamingPath();
    path+="\\log\\medialib.log";
    auto rotating_logger = spdlog::daily_logger_mt<spdlog::async_factory>(LOG_TAG, path.c_str());
	//specify the log level
	rotating_logger->set_level(spdlog::level::debug);
	rotating_logger->flush_on(spdlog::level::debug);
    spdlog::set_default_logger(rotating_logger);

    auto console_logger = spdlog::stdout_color_mt("console");
    console_logger->set_level(spdlog::level::debug);
    console_logger->set_pattern("%^[%Y-%m-%d %H:%M:%S:%e] [%n] [%t] [%l] %v%$");

    LogI("===============================MediaLibLog Started================================");
}
Log::~Log()
{
	//spdlog::shutdown();
	
}
void Log::LogD(const char* log)
{
    try {
        spdlog::debug(log);
#ifdef _DEBUG
        spdlog::get("console")->debug(log);
        std::printf("RtspFFMpegLog#LogD: %s\n", log);
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("RtspFFMpegLog#LogD failed: %s\n", ex.what());
    }
}
void Log::LogW(const char* log)
{
    try {
        spdlog::warn(log);
#ifdef _DEBUG
        spdlog::get("console")->warn(log);
        std::printf("RtspFFMpegLog#LogW: %s\n", log);
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("LogW failed: %s\n", ex.what());
    }
}
void Log::LogI(const char* log)
{
    try {
        spdlog::info(log);
#ifdef _DEBUG
        spdlog::get("console")->info(log);
        std::printf("RtspFFMpegLog#LogI: %s\n", log);
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("LogI failed: %s\n", ex.what());
    }
}
void Log::LogE(const char* log)
{
    try {
        spdlog::error(log);
#ifdef _DEBUG
        spdlog::get("console")->error(log);
        std::printf("RtspFFMpegLog#LogE: %s\n", log);
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("LogE failed: %s\n", ex.what());
    }
}
void Log::LogDebug(const char* fmt, ...)
{
    if (!fmt) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    char logbuf[DEF_LOG_BUF_SIZE] = { 0 };
    vsnprintf(logbuf, DEF_LOG_BUF_SIZE - 1, fmt, args);
    va_end(args);

    try {
        spdlog::debug(logbuf);
#ifdef _DEBUG
        spdlog::get("console")->debug(logbuf);
        std::printf("RtspFFMpegLog#LogDebug: %s\n", logbuf);
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("LogDebug failed: %s\n", ex.what());
    }
}
void Log::LogWarning(const char* fmt, ...)
{
    if (!fmt) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    char logbuf[DEF_LOG_BUF_SIZE] = { 0 };
    vsnprintf(logbuf, DEF_LOG_BUF_SIZE - 1, fmt, args);
    va_end(args);

    try {
        spdlog::warn(logbuf);
#ifdef _DEBUG
        spdlog::get("console")->warn(logbuf);
        std::printf("RtspFFMpegLog#LogWarning: %s\n", logbuf);
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("LogWarning failed: %s\n", ex.what());
    }
}
void Log::LogInfo(const char* fmt, ...)
{
    if (!fmt) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    char logbuf[DEF_LOG_BUF_SIZE] = { 0 };
    vsnprintf(logbuf, DEF_LOG_BUF_SIZE - 1, fmt, args);
    va_end(args);

    try {
        spdlog::info(logbuf);
#ifdef _DEBUG
        spdlog::get("console")->info(logbuf);
        std::printf("RtspFFMpegLog#LogInfo: %s\n", logbuf);
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("LogInfo failed: %s\n", ex.what());
    }
}
void Log::LogError(const char* fmt, ...)
{
    if (!fmt) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    char logbuf[DEF_LOG_BUF_SIZE] = { 0 };
    vsnprintf(logbuf, DEF_LOG_BUF_SIZE - 1, fmt, args);
    va_end(args);

    try {
        spdlog::error(logbuf);
#ifdef _DEBUG
        spdlog::get("console")->error(logbuf);
        std::printf("RtspFFMpegLog#LogError: %s\n", logbuf);
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("LogError failed: %s\n", ex.what());
    }
}
void Log::LogDebug(std::string& debug)
{
    try {
        spdlog::debug(debug);
#ifdef _DEBUG
        spdlog::get("console")->debug(debug);
        std::printf("RtspFFMpegLog#LogDebug: %s\n", debug.c_str());
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("LogDebug failed: %s\n", ex.what());
    }
}
void Log::LogWarning(std::string& wanning)
{
    try {
        spdlog::warn(wanning);
#ifdef _DEBUG
        spdlog::get("console")->warn(wanning);
        std::printf("RtspFFMpegLog#LogWarning: %s\n", wanning.c_str());
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("LogWarning failed: %s\n", ex.what());
    }
}
void Log::LogInfo(std::string& info)
{
    try {
        spdlog::info(info);
#ifdef _DEBUG
        spdlog::get("console")->info(info);
        std::printf("RtspFFMpegLog#LogInfo: %s\n", info.c_str());
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("LogInfo failed: %s\n", ex.what());
    }
}
void Log::LogError(std::string& error)
{
    try {
        spdlog::error(error);
#ifdef _DEBUG
        spdlog::get("console")->error(error);
        std::printf("RtspFFMpegLog#LogError: %s\n", error.c_str());
#endif
    }
    catch (const spdlog::spdlog_ex & ex) {
        std::printf("LogError failed: %s\n", ex.what());
    }
}