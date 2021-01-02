#include <string>
#include <chrono>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hh = seconds / 3600;
  long mm = (seconds % 3600) / 60;
  long ss = seconds % 60;
  return std::to_string(hh) + ":" + std::to_string(mm) + ":" + std::to_string(ss);
}