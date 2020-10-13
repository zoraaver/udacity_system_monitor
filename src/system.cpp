#include <unistd.h>
#include <cstddef>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes()
{
  vector<int> pids = LinuxParser::Pids();
  processes_.clear();
  processes_.reserve(pids.size());
  
  for (const int& pid : pids)
  {
    processes_.emplace_back(pid, LinuxParser::User(pid), LinuxParser::Command(pid));
    processes_.back().CpuUtilization(LinuxParser::ActiveJiffies(pid), LinuxParser::Jiffies());
  }

  std::sort(processes_.begin(), processes_.end(),
            [] (const Process& a, const Process& b) -> bool
            {
              return a.CpuUtilization() > b.CpuUtilization();
            });
  return processes_;
}

std::string System::Kernel() const { return LinuxParser::Kernel(); }

float System::MemoryUtilization() const { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() const { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() const { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() const { return LinuxParser::TotalProcesses(); }

long int System::UpTime() const { return LinuxParser::UpTime(); }