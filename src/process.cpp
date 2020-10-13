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

Process::Process(int pid, const std::string& user, const std::string& command) :
	pid(pid), user(user), command(command) {}

int Process::Pid() const { return pid; }

void Process::CpuUtilization(long activeTicks, long systemTicks)
{
  long activeDuration = activeTicks - cachedActive;
  long duration = systemTicks - cachedSystem; 
  cpuUtilization = float (activeDuration) / float (duration);
  cachedActive = activeTicks;
  cachedSystem = systemTicks;
}

float Process::CpuUtilization() const
{
  return cpuUtilization;
}

string Process::Command() const { return command; }

string Process::Ram() const { return LinuxParser::Ram(pid); }

string Process::User() const { return user; }

long int Process::UpTime() const { return LinuxParser::UpTime(pid); }