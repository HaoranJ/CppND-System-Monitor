#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  unsigned long long int user_time_;
  unsigned long long int nice_time_;
  unsigned long long int system_time_;
  unsigned long long int idle_time_;
  unsigned long long int io_wait_time_;
  unsigned long long int irq_time_;
  unsigned long long int soft_irq_time_;
  unsigned long long int steal_time_;
  unsigned long long int guest_time_;
  unsigned long long int guest_nice_time_;
};

#endif