#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid, std::string uid, std::string cmd, float cpu_util) : pid_{pid}, user_{uid}, cmd_{cmd}, cpu_util_{cpu_util} {};

// DONE: Return this process's ID
int Process::Pid() { return this->pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
  float cpu_usage = LinuxParser::CpuUtilization(this->pid_);
  this->cpu_util_ = cpu_usage;
  return cpu_usage;
}

// DONE: Return the command that generated this process
string Process::Command() {
  return LinuxParser::Command(this->pid_);
}

// DONE: Return this process's memory utilization
string Process::Ram() {
  return LinuxParser::Ram(this->pid_);
}

// DONE: Return the user (name) that generated this process
string Process::User() {
  return LinuxParser::User(this->pid_);
}

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(this->pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return this->cpu_util_ > a.cpu_util_;
}