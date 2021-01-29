#pragma once
#include <memory>
#include <string>
#include <mutex>

#define LOGD Log::getLog()->LogD
#define LOGW Log::getLog()->LogW
#define LOGI Log::getLog()->LogI
#define LOGE Log::getLog()->LogE
#define LOG_DEBUG Log::getLog()->LogDebug
#define LOG_ERROR Log::getLog()->LogError
#define LOG_INFO Log::getLog()->LogInfo
#define LOG_WANNING Log::getLog()->LogWarning

class Log
{
public:
	Log();
    ~Log();

    void LogD(const char* log);
    void LogW(const char* log);
    void LogI(const char* log);
    void LogE(const char* log);
    void LogDebug(const char* fmt, ...);
    void LogWarning(const char* fmt, ...);
    void LogInfo(const char* fmt, ...);
    void LogError(const char* fmt, ...);
    void LogDebug(std::string& debug);
    void LogWarning(std::string& wanning);
    void LogInfo(std::string& info);
    void LogError(std::string& error);
    static std::shared_ptr<Log>& getLog() {
        if (!m_instance_ptr) {
            m_mutex.lock();
            if (!m_instance_ptr) {
				m_instance_ptr = std::make_shared<Log>();
            }
            m_mutex.unlock();
        }
        return m_instance_ptr;
    }
private:
    static std::shared_ptr<Log> m_instance_ptr;
	static std::mutex m_mutex;
};