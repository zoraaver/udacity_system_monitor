#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long numberOfSeconds)
{
  
  string hours = belowOrAboveTen(numberOfSeconds / 3600);
  numberOfSeconds %= 3600;
  
  string minutes = belowOrAboveTen(numberOfSeconds / 60);
  numberOfSeconds %= 60;
  
  string seconds = belowOrAboveTen(numberOfSeconds);
  
  return hours + ':' + minutes + ':' + seconds;
}

std::string Format::belowOrAboveTen(long inputNumber)
{
  if (inputNumber < 10)
    return '0' + std::to_string(inputNumber);
  else
    return std::to_string(inputNumber);
}