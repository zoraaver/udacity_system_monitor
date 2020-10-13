#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() 
{
  long active = LinuxParser::ActiveJiffies();
  long idle = LinuxParser::IdleJiffies();
  long activeDuration = active - cachedActive;
  long idleDuration = idle - cachedIdle;
  cachedActive = active;
  cachedIdle = idle;
  return float (activeDuration) / float (idleDuration);
}