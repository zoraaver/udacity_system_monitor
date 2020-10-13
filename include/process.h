#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
 public:
  Process(int pid, const std::string& user, const std::string& command);
  int Pid() const;                         
  std::string User() const;                      
  std::string Command() const;                   
  float CpuUtilization() const;                  
  void CpuUtilization(long activeTicks, long systemTicks);
  std::string Ram() const;                       
  long int UpTime() const;                       
  Process& operator=(Process&&) = default;
  Process(const Process&) = default;
  Process(Process&&) = default;
  
 private:
  int pid;
  std::string user;
  std::string command;
  long cachedActive{0}, cachedIdle{0}, cachedSystem{0};
  float cpuUtilization = 0.0f;
  std::string ram;
  long upTime = 0;
};

#endif