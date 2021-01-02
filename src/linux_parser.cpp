#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
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
  string line;
  string name, value, unit;
  float mem_total, mem_free;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      if (linestream >> name >> value >> unit) {
        if (name == "MemTotal:") {
          mem_total = stof(value);
        }
        if (name == "MemFree:") {
          mem_free = stof(value);
        }
      }
    }
  }

  float mem_util = (mem_total - mem_free) / mem_total;
  return mem_util;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string uptime, idle;
  long uptime_in_sec;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      if (linestream >> uptime >> idle) {
        uptime_in_sec = stol(uptime);
      }
    }
  }

  return uptime_in_sec;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string line;
  string total_uptime_in_sec, idle_uptime_in_sec;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      if (linestream >> total_uptime_in_sec >> idle_uptime_in_sec) {
        return std::stol(total_uptime_in_sec) * 100;
      }
    }
  }

  return 0; 
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
        user_time_jiffy = std::stol(cpu_stats[13]);
        kernel_time_jiffy = std::stol(cpu_stats[14]);
        break;
      }
    }
  }

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
        return (std::stol(total_uptime_in_sec) - std::stol(idle_uptime_in_sec)) * 100;
      }
    }
  }

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
        return std::stol(idle_uptime_in_sec) * 100;
      }
    }
  }

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

      if (cpu_stats[0] == "cpu") {
        cpu_stats.erase(cpu_stats.begin());
        return cpu_stats;
      }
    }
  }

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
  return cpu_usage;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string name, value;
  int process_total;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      if (linestream >> name >> value) {
        if (name == "processes") {
          process_total = stoi(value);
          break;
        }
      }
    }
  }

  return process_total;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string name, value;
  int process_total;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      if (linestream >> name >> value) {
        if (name == "procs_running") {
          process_total = stoi(value);
          break;
        }
      }
    }
  }

  return process_total;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line))
    {
      return line;
    }
  }
  return string(); 
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string name, ram_kb;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      if (linestream >> name >> ram_kb) {
        if (name == "VmSize:") {
          string ram_mb = std::to_string(std::stol(ram_kb) / 1000);
          return ram_mb;
        }
      }
    }
  }

  return string();
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string name, uid;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      if (linestream >> name >> uid) {
        if (name == "Uid:") {
          return uid;
        }
      }
    }
  }

  return string();
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      string delimiter = ":x:" + std::to_string(pid);
      std::size_t idx = line.find(delimiter);
      if (idx == std::string::npos) {
        continue;
      }

      string username = line.substr(0, idx);
      return username;
    }
  }

  return string();
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
        return 0;
      } 
      long start_time_in_sec = (std::stol(cpu_stats[21]))/sysconf(_SC_CLK_TCK);
      return start_time_in_sec;
    }
  }

  return 0; 
}