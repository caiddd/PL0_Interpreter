#ifndef UTIL_H
#define UTIL_H

#include <sstream>

namespace pl0 {

template<typename... Args>
inline std::string Concat(Args... args) {
  std::ostringstream oss;
  (oss << ... << args);
  return oss.str();
}

struct Location {
  int line = 1;
  int column = 1;

  std::string to_string() const {
    return std::to_string(line) + ':' + std::to_string(column);
  }
};

class BasicError {
 public:
  explicit BasicError(std::string message) : message_(std::move(message)) {}

  virtual const std::string &what() const { return message_; }

 private:
  std::string message_;
};

class GeneralError : BasicError {
 public:
  template<typename... Args>
  explicit GeneralError(Args... args) : BasicError(Concat(args...)) {}
};

class SyntaxError : BasicError {
 public:
  template<typename... Args>
  explicit SyntaxError(Location loc, Args... args)
      : loc_(loc), BasicError(concat(args...)) {}

 private:
  Location loc_;
};

}; // namespace pl0

#endif // UTIL_H