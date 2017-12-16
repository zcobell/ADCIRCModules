#ifndef STRINGCONVERSION_H
#define STRINGCONVERSION_H

#include <string>

class StringConversion {
public:
  StringConversion();

  static int stringToInt(std::string a, bool &ok);
  static float stringToFloat(std::string a, bool &ok);
  static double stringToDouble(std::string a, bool &ok);
};

#endif // STRINGCONVERSION_H
