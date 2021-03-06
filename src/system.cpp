#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  this->processes_.clear();
  vector<int> pids = LinuxParser::Pids();
  for (int pid : pids) {
    string uid = LinuxParser::Uid(pid);
    string cmd = LinuxParser::Command(pid);
    float cpu_util = LinuxParser::CpuUtilization(pid);
    Process proc(pid, uid, cmd, cpu_util);
    this->processes_.emplace_back(proc);
  }

  std::sort(this->processes_.begin(), this->processes_.end());
  return this->processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { 
  return LinuxParser::MemoryUtilization();
}

// DONE: Return the operating system name
std::string System::OperatingSystem() { 
  return LinuxParser::OperatingSystem();
}

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() {
  return LinuxParser::RunningProcesses();
}

// DONE: Return the total number of processes on the system
int System::TotalProcesses() {
  return LinuxParser::TotalProcesses();
}

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { 
  long int ans = LinuxParser::UpTime(); 
  return ans;
}