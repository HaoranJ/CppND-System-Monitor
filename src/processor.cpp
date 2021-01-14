#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::vector<std::string> cpu_stats = LinuxParser::CpuUtilization();
  unsigned long long int user_time, nice_time, system_time, idle_time;
  unsigned long long int io_wait, irq, soft_irq, steal, guest, guest_nice;
  std::string::size_type sz = 0;
  user_time = std::stoull(cpu_stats[0], &sz, 10);
  nice_time = std::stoull(cpu_stats[1], &sz, 10);
  system_time = std::stoull(cpu_stats[2], &sz, 10);
  idle_time = std::stoull(cpu_stats[3], &sz, 10);
  io_wait = std::stoull(cpu_stats[4], &sz, 10);
  irq = std::stoull(cpu_stats[5], &sz, 10);
  soft_irq = std::stoull(cpu_stats[6], &sz, 10);
  steal = std::stoull(cpu_stats[7], &sz, 10);
  guest = std::stoull(cpu_stats[8], &sz, 10);
  guest_nice = std::stoull(cpu_stats[9], &sz, 10);

  auto prev_idle = this->idle_time_ + this->io_wait_time_;
  auto cur_idle = idle_time + io_wait;
  auto prev_non_idle = this->user_time_ + this->nice_time_ + this->system_time_ + this->irq_time_ + this->soft_irq_time_ + this->steal_time_;
  auto cur_non_idle = user_time + nice_time + system_time + irq + soft_irq + steal;
  auto prev_total = prev_idle + prev_non_idle;
  auto cur_total = cur_idle + cur_non_idle;
  auto total_diff = cur_total > prev_total ? cur_total - prev_total : 0;
  auto idle_diff = cur_idle > prev_idle ? cur_idle - prev_idle : 0;
  double cpu_percentage = (static_cast<double>(total_diff) - static_cast<double>(idle_diff)) / static_cast<double>(total_diff);

  this->user_time_ = user_time;
  this->nice_time_ = nice_time;
  this->system_time_ = system_time;
  this->idle_time_ = idle_time;
  this->io_wait_time_ = io_wait;
  this->irq_time_ = irq;
  this->soft_irq_time_ = soft_irq;
  this->steal_time_ = steal;
  this->guest_time_ = guest;
  this->guest_nice_time_ = guest_nice;
  return cpu_percentage;
}