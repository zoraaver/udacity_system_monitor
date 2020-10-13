#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

float LinuxParser::MemoryUtilization()
 { 
  float memTotal = 1.0f;
  float memFree = 0.0f;
  float buffers = 0.0f;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open())
  {
    string token;
    while (stream >> token)
    {
      if (token == "MemTotal:")
        stream >> memTotal;
      else if (token == "MemFree:")
        stream >> memFree;
      else if (token == "Buffers:")
      {
        stream >> buffers;
        return 1 - memFree / (memTotal - buffers);
      }  
    }
  }
  return 1.0f;
 }

long LinuxParser::UpTime()
{ 
  long upTime = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open())
  {
    std::string line;
  	std::getline(stream, line);
  	std::istringstream linestream(line);
  	linestream >> upTime;
  }
  return upTime;
}

long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK); }

long LinuxParser::ActiveJiffies(int pid) 
{
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open())
  {
    // ignore first 13 tokens in /proc/[pid]/stat file
    for (int i = 0; i < 13; i++)
      stream.ignore(256, ' ');
    long userTime{0}, kernelTime{0}, childrenUserTime{0}, childrenKernelTime{0};
    stream >> userTime >> kernelTime >> childrenUserTime >> childrenKernelTime;
    return userTime + kernelTime + childrenUserTime + childrenKernelTime;
  }
  return 0;
}

long LinuxParser::ActiveJiffies()
{
  vector<long> cpuUtilization = CpuUtilization();
  return cpuUtilization[CPUStates::kUser_] + cpuUtilization[CPUStates::kNice_] +
         cpuUtilization[CPUStates::kSystem_] + cpuUtilization[CPUStates::kIRQ_] +
          cpuUtilization[CPUStates::kSoftIRQ_] + cpuUtilization[CPUStates::kSteal_] +
          cpuUtilization[CPUStates::kGuest_] + cpuUtilization[CPUStates::kGuestNice_];
}

long LinuxParser::IdleJiffies()
{ 
  vector<long> cpuUtilization = CpuUtilization();
  return cpuUtilization[CPUStates::kIdle_] + cpuUtilization[CPUStates::kIOwait_];
}

vector<long> LinuxParser::CpuUtilization()
{
  vector<long> cpuUtilization;
  cpuUtilization.reserve(10);
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    long value;
    stream.ignore(256, ' '); // ignore first word 'cpu'
    while (stream >> value && stream.peek() != '\n')
      cpuUtilization.push_back(value);
  }
  return cpuUtilization;
}


int LinuxParser::TotalProcesses()
{
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    string token;
    int totalProcesses = 0;
    while (stream >> token)
    {
      if (token == "processes")
      {
        stream >> totalProcesses;
        return totalProcesses;
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses()
{
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    string token;
    int runningProcesses = 0;
    while (stream >> token)
    {
      if (token == "procs_running")
      {
        stream >> runningProcesses;
        return runningProcesses;
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid)
{
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string cmd;
  if (stream.is_open()) stream >> cmd;
  return cmd; 
}

string LinuxParser::Ram(int pid)
{
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open())
  {
    string token;
    while (stream >> token)
    {
      if (token == "VmSize:")
      {
        stream >> token;
        return std::to_string(stoi(token) / 1024);
      }
    }
  }
  return "";
}

string LinuxParser::Uid(int pid)
{
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open())
  {
    string token;
    while (stream >> token)
    {
      if (token == "Uid:")
      {
        stream >> token;
        return token;
      }
    }
  }
  return "";
}

string LinuxParser::User(int pid)
{
  string uid = Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open())
  {
    while (!stream.eof())
    {
      string user, userId;
      std::getline(stream, user, ':');
      stream.ignore(256, ':');
      std::getline(stream, userId, ':');
      if (userId == uid)
        return user;
      stream.ignore(256, '\n');
    }
  }
  return "";
}


long LinuxParser::UpTime(int pid)
{
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  long upTime = 0;
  if (stream.is_open())
  {
    // ignore first 21 tokens in /proc/[pid]/stat file
    for (int i = 0; i < 21; i++)
      stream.ignore(256, ' ');
    
    stream >> upTime;
    upTime /= sysconf(_SC_CLK_TCK);
  }
  return upTime;
}