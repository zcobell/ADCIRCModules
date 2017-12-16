#include "stringconversion.h"

StringConversion::StringConversion() {}

double StringConversion::stringToDouble(std::string a, bool &ok) {
  ok = true;
  try {
    return std::stod(a);
  } catch (...) {
    ok = false;
    return 0.0;
  }
}

float StringConversion::stringToFloat(std::string a, bool &ok) {
  ok = true;
  try {
    return std::stof(a);
  } catch (...) {
    ok = false;
    return 0.0f;
  }
}

int StringConversion::stringToInt(std::string a, bool &ok) {
  ok = true;
  try {
    return std::stoi(a);
  } catch (...) {
    ok = false;
    return 0;
  }
}
