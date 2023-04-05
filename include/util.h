#ifndef UTIL_H
#define UTIL_H

#include <sstream>

namespace pl0 {

struct Location {
  int line = 1;
  int column = 1;

  std::string to_string() const {
    return std::to_string(line) + ':' + std::to_string(column);
  }
};

}; // namespace pl0

#endif // UTIL_H