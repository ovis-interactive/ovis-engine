#pragma once

#include <functional>
#include <ostream>
#include <sstream>
#include <utility>
#include <vector>

#include <fmt/format.h>

namespace ovis {

enum class LogLevel { VERBOSE = 0, DEBUG, INFO, WARNING, ERROR };
using LogListener = std::function<void(LogLevel, const std::string&)>;

namespace detail {

static const char LOG_LEVEL_CHARS[] = {'V', 'D', 'I', 'W', 'E'};
static const char* LOG_LEVEL_STRINGS[] = {"verbose", "debug", "info", "warning", "error"};

}  // namespace detail

class Log {
 public:
  template <typename... T>
  static inline void Write(LogLevel level, std::string_view format_string, T&&... args) {
    std::ostringstream format_buffer;
    const char log_level_char = detail::LOG_LEVEL_CHARS[static_cast<int>(level)];
    fmt::format_to(std::ostream_iterator<char>(format_buffer), "{}: ", log_level_char);
    fmt::format_to(std::ostream_iterator<char>(format_buffer), fmt::runtime(format_string), std::forward<T>(args)...);

    std::string formatted_string = format_buffer.str();
    for (auto& listener : log_listeners_) {
      if (listener) {
        listener(level, formatted_string);
      }
    }
  }

  static size_t AddListener(LogListener listener);
  static void RemoveListener(size_t id);

 private:
  static std::vector<LogListener> log_listeners_;
};

void ConsoleLogger(LogLevel, const std::string& text);

template <typename... T>
inline void LogV(std::string_view format_string, T&&... args) {
  Log::Write(LogLevel::VERBOSE, format_string, std::forward<T>(args)...);
}

template <typename... T>
inline void LogD(std::string_view format_string, T&&... args) {
  Log::Write(LogLevel::DEBUG, format_string, std::forward<T>(args)...);
}

template <typename... T>
inline void LogI(std::string_view format_string, T&&... args) {
  Log::Write(LogLevel::INFO, format_string, std::forward<T>(args)...);
}

template <typename... T>
inline void LogW(std::string_view format_string, T&&... args) {
  Log::Write(LogLevel::WARNING, format_string, std::forward<T>(args)...);
}

template <typename... T>
inline void LogE(std::string_view format_string, T&&... args) {
  Log::Write(LogLevel::ERROR, format_string, std::forward<T>(args)...);
}

inline const char* GetLogLevelString(LogLevel level) {
  return detail::LOG_LEVEL_STRINGS[static_cast<int>(level)];
}

}  // namespace ovis
