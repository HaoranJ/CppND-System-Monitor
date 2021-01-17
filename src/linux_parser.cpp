#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <unistd.h>
#include <sstream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

template <typename T>
T getValueOfFile(string const &filename) {
  string line;
  T value;
  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    if (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> value;
    }
  }

  stream.close();
  return value;
};

template <typename T>
T getValueByFilter(string const &filter, string const &filename) {
  string line;
  string name;
  T value;
  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      if (linestream >> name >> value) {
        if (name == filter) {
          stream.close();
          return value;
        }
      }
    }
  }

  stream.close();
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel >> version;
  }
  stream.close();
  return version;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float mem_total, mem_free;
  string mem_total_str = getValueByFilter<string>(filterMemTotalString, kMeminfoFilename);
  mem_total = mem_total_str.empty() ? 0.0 : stof(mem_total_str);

  string mem_free_str = getValueByFilter<string>(filterMemFreeString, kMeminfoFilename);
  mem_free = mem_free_str.empty() ? 0.0 : stof(mem_free_str);

  float mem_util = (mem_total - mem_free) / mem_total;
  return mem_util;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string uptime = getValueOfFile<string>(kUptimeFilename);
  long uptime_in_sec = uptime.empty() ? 0 : stol(uptime);

  return uptime_in_sec;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string uptime = getValueOfFile<string>(kUptimeFilename);
  long uptime_in_sec = uptime.empty() ? 0 : stol(uptime);

  return uptime_in_sec * 100;
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  long user_time_jiffy, kernel_time_jiffy;
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      vector<string> cpu_stats(std::istream_iterator<string>{linestream},
                                 std::istream_iterator<string>());

      if (cpu_stats[0] == std::to_string(pid)) {
        user_time_jiffy = cpu_stats[13].empty() ? 0 : std::stol(cpu_stats[13]);
        kernel_time_jiffy = cpu_stats[14].empty() ? 0 : std::stol(cpu_stats[14]);
        break;
      }
    }
  }

  stream.close();
  return user_time_jiffy + kernel_time_jiffy;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string line;
  string total_uptime_in_sec, idle_uptime_in_sec;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      if (linestream >> total_uptime_in_sec >> idle_uptime_in_sec) {
        long total_time = total_uptime_in_sec.empty() ? 0 : std::stol(total_uptime_in_sec);
        long idle_time = idle_uptime_in_sec.empty() ? 0 : std::stol(idle_uptime_in_sec);

        stream.close();
        return (total_time - idle_time) * 100;
      }
    }
  }

  stream.close();
  return 0; 
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line;
  string total_uptime_in_sec, idle_uptime_in_sec;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      if (linestream >> total_uptime_in_sec >> idle_uptime_in_sec) {
        long idle_time = idle_uptime_in_sec.empty() ? 0 : std::stol(idle_uptime_in_sec);

        stream.close();
        return idle_time * 100;
      }
    }
  }

  stream.close();
  return 0; 
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      vector<string> cpu_stats(std::istream_iterator<string>{linestream},
                                 std::istream_iterator<string>());

      if (cpu_stats[0] == filterCpu) {
        cpu_stats.erase(cpu_stats.begin());
        stream.close();
        return cpu_stats;
      }
    }
  }

  stream.close();
  return {};
}

float LinuxParser::CpuUtilization(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  long user_time, kernel_time, user_children_time, kernel_children_time;
  long start_time = UpTime(pid);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      vector<string> cpu_stats(std::istream_iterator<string>{linestream},
                                 std::istream_iterator<string>());

      if (cpu_stats[0] == std::to_string(pid)) {
        user_time = std::stol(cpu_stats[13]);
        kernel_time = std::stol(cpu_stats[14]);
        user_children_time = std::stol(cpu_stats[15]);
        kernel_children_time = std::stol(cpu_stats[16]);
        break;
      }
    }
  }

  long system_uptime = UpTime();
  long hertz = sysconf(_SC_CLK_TCK);
  long total_time = user_time + kernel_time + user_children_time + kernel_children_time;
  long elapsed_in_sec = system_uptime - (start_time/hertz);
  float cpu_usage = (static_cast<double>(total_time)/static_cast<double>(hertz))/static_cast<double>(elapsed_in_sec);

  stream.close();
  return cpu_usage;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string process_total_str = getValueByFilter<string>(filterProcesses, kStatFilename);
  int process_total = process_total_str.empty() ? 0 : stoi(process_total_str);

  return process_total;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string process_total_str = getValueByFilter<string>(filterRunningProcesses, kStatFilename);
  int process_total = process_total_str.empty() ? 0 : stoi(process_total_str);

  return process_total;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string filename = std::to_string(pid) + kCmdlineFilename;

  return getValueOfFile<string>(filename);
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string filename = std::to_string(pid) + kStatusFilename;
  string ram_kb = getValueByFilter<string>(filterProcMem, filename);
  long ram_kb_num = ram_kb.empty() ? 0 : std::stol(ram_kb);
  string ram_mb = std::to_string(ram_kb_num / 1000);
  return ram_mb;
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string filename = std::to_string(pid) + kStatusFilename;
  string uid = getValueByFilter<string>(filterUID, filename);
  return uid;
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      string uid = Uid(pid);
      string delimiter = ":x:" + uid;
      std::size_t idx = line.find(delimiter);
      if (idx == std::string::npos) {
        continue;
      }

      string username = line.substr(0, idx);
      stream.close();
      return username;
    }
  }

  stream.close();
  return string();
}

struct version {
  int major;
  int minor;
};

version getVersion(string kernel) {
  std::istringstream iss(kernel);
  std::vector<string> tokens;
  string tk;
  while (std::getline(iss, tk, '.')) {
    if (!tk.empty()) {
      tokens.push_back(tk);
    }
  }

  int major = std::stoi(tokens[0]);
  int minor = std::stoi(tokens[1]);
  struct version v = {major, minor};
  return v;
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      vector<string> cpu_stats(std::istream_iterator<string>{linestream},
                                 std::istream_iterator<string>());
      if (cpu_stats.size() < 22) {
        stream.close();
        return 0;
      } 

      long process_start_time = cpu_stats[21].empty() ? 0 : std::stol(cpu_stats[21]);

      string kernel = Kernel();
      auto version = getVersion(kernel);
      long uptime_pid;
      if (version.major <= 1 || (version.major == 2 && version.minor < 6)) {
        // jiffy before Linux 2.6
        uptime_pid = UpTime() - process_start_time/100; 
      } else {
        // clock tick after Linux 2.6
        uptime_pid = UpTime() - process_start_time/sysconf(_SC_CLK_TCK);
      }
       
      stream.close();
      return uptime_pid;
    }
  }

  stream.close(); 
  return 0; 
}