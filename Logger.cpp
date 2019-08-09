// Local
#include "Logger.h"

// STD
#include <ctime>
#include <iostream>

// C Lib
#include <time.h>


LoggerPrivate::LoggerPrivate(const std::string& filePath, LogLevel logLevel, unsigned int timeout)
  : m_logFilePath(filePath)
  , m_terminate(false)
  , m_logLevel(logLevel)
{
  m_logFile.open(filePath, std::ios::out | std::ios::app);
  if (!m_logFile.is_open())
  {
    std::cerr << "cannot open file " << filePath;
    return;
  }
  m_writerThread = std::thread(&LoggerPrivate::writeToLog, this, timeout);
}


LoggerPrivate::~LoggerPrivate()
{
  terminate();
  m_writerThread.join();
}


void LoggerPrivate::writeToLog(unsigned int timeout)
{
  std::string* strToLog = nullptr;
  while (!m_terminate)
  {
    while (m_logMsgsQueue.pop(strToLog))
    {
      if (m_logFile.is_open())
      {
        m_logFile << (*strToLog) << std::endl;
        m_logFile.flush();
      }
      delete strToLog;
    }
    if (timeout > 0)
      std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
  }

  while (m_logMsgsQueue.pop(strToLog))
  {
    if (m_logFile.is_open())
    {
      m_logFile << (*strToLog) << std::endl;
      m_logFile.flush();
    }
    delete strToLog;
  }
  m_logFile.close();
}


void LoggerPrivate::log(const char* str, LogLevel logLevel)
{
  if (logLevel > m_logLevel)
    return;

  time_t t = time(nullptr);
  tm buf;
  char strBuf[128] = {0};
  asctime_r(localtime_r(&t, &buf), strBuf);
  std::string stringTime(strBuf);
  stringTime.pop_back();
  std::string strToLog = logLevelToStr(logLevel) + stringTime +  ": " + std::string(str);
  std::string* msg = new std::string(strToLog);
  while (!m_logMsgsQueue.push(msg));
}


void LoggerPrivate::terminate()
{
  m_terminate = true;
}


std::string LoggerPrivate::logLevelToStr(LogLevel logLevel) const
{
  std::string logLevelSrt;
  switch (logLevel)
  {
  case LogLevel::FATAL:
    logLevelSrt = "FATAL: ";
    break;
  case LogLevel::ERROR:
    logLevelSrt = "ERROR: ";
    break;
  case LogLevel::WARN:
    logLevelSrt = "WARN: ";
    break;
  case LogLevel::INFO:
    logLevelSrt = "INFO: ";
    break;
  case LogLevel::DEBUG:
    logLevelSrt = "DEBUG: ";
    break;
  default:
    logLevelSrt = "INFO: ";
    break;
  }
  return logLevelSrt;
}


// -------------------------------------------

std::unique_ptr<Logger> Logger::m_instance;
std::once_flag Logger::m_onceFlag;


Logger* Logger::instance()
{
  std::call_once(Logger::m_onceFlag,[] (){
    m_instance.reset(new Logger);
  });
  return m_instance.get();
}


void Logger::log(const char* str, LogLevel logLevel)
{
  if (m_logger)
    m_logger->log(str, logLevel);
}


void Logger::setLogParams(const std::string& filePath, LogLevel logLevel, unsigned int timeout)
{
  m_logger.reset(new LoggerPrivate(filePath, logLevel, timeout));
}
