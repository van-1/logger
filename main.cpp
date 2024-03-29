// STL
#include <iostream>
#include <thread>
#include <sstream>

// Local
#include <Logger.h>

void* producer(void* arg)
{
  std::stringstream ss;
  ss << std::this_thread::get_id();

  for (int i = 0; i < 1000000; i++)
  {
    std::string str = "producer " + std::to_string(i) + " " + ss.str();
    log_fatal(str.c_str(), LogLevel::DEBUG);
    log_error(str.c_str(), LogLevel::ERROR);
    log_warn(str.c_str(), LogLevel::INFO);
    log_info(str.c_str(), LogLevel::FATAL);
    log_debug(str.c_str(), LogLevel::WARN);
  }

  std::cout << ss.str() << " exited" << std::endl;

  return nullptr;
}


int main(int argc, char *argv[])
{
  Logger* logger  = Logger::instance();
  logger->setLogParams("log.txt", LogLevel::WARN);

  pthread_t a, b, c, d, e, f;

  pthread_create(&a, nullptr, producer, &logger);
  pthread_create(&b, nullptr, producer, &logger);
  pthread_create(&c, nullptr, producer, &logger);
  pthread_create(&d, nullptr, producer, &logger);
  pthread_create(&e, nullptr, producer, &logger);
  pthread_create(&f, nullptr, producer, &logger);

  pthread_join(a, nullptr);
  pthread_join(b, nullptr);
  pthread_join(c, nullptr);
  pthread_join(d, nullptr);
  pthread_join(e, nullptr);
  pthread_join(f, nullptr);

  return 0;
}
