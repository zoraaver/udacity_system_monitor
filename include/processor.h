#ifndef PROCESSOR_H
#define PROCESSOR_H



class Processor {
 public:
  float Utilization();
  
 private:
  long cachedActive = 0;
  long cachedIdle = 0;
};

#endif