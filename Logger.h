#ifndef LOGGER_H
#define LOGGER_H


// STD
#include <fstream>
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>

// Boost
#include <boost/lockfree/queue.hpp>

#define log_fatal(str, logLevel) Logger::instance()->log(str, logLevel);
#define log_error(str, logLevel) Logger::instance()->log(str, logLevel);
#define log_warn(str, logLevel) Logger::instance()->log(str, logLevel);
#define log_info(str, logLevel) Logger::instance()->log(str, logLevel);
#define log_debug(str, logLevel) Logger::instance()->log(str, logLevel);

enum LogLevel
{
  FATAL = 0,
  ERROR = 1,
  WARN = 2,
  INFO = 3,
  DEBUG = 4
};

class LoggerPrivate
{
  public:
    LoggerPrivate(const std::string& filePath, LogLevel logLevel, unsigned int timeout = 50);
    ~LoggerPrivate();
    void log(const char* str, LogLevel logLevel);
    void terminate();
    std::string logLevelToStr(LogLevel logLevel) const;

  private:
    // timeout in milliseconds
    void writeToLog(unsigned int timeout = 0);

  private:
    std::string m_logFilePath;
    std::ofstream m_logFile;
    boost::lockfree::queue<std::string*, boost::lockfree::capacity<4096>> m_logMsgsQueue;
    std::atomic<bool> m_terminate;
    std::thread m_writerThread;
    LogLevel m_logLevel;
};

class Logger
{
  public:
    static Logger* instance();
    void log(const char* str, LogLevel logLevel);
    void setLogParams(const std::string& filePath, LogLevel logLevel, unsigned int timeout = 50);

  private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static std::unique_ptr<Logger> m_instance;
    static std::once_flag m_onceFlag;
    std::unique_ptr<LoggerPrivate> m_logger;
};

#endif // LOGGER_H
