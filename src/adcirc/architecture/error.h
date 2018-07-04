#ifndef ERROR_H
#define ERROR_H

#include <string>

namespace Adcirc {

enum _genericErrorCodes {
  NoError = 0x0000000,
  MissingFile = 0x0000001,
  FileOpenError = 0x0000002,
  FileReadError = 0x0000003,
  FileCloseError = 0x0000004,
  FileNotOpenError = 0x0000005,
  Proj4Error = 0x0000006,
  KdtreeError = 0x0000007,
  UnknownError = 0x9999999
};

namespace ModelParameters {
enum _modelParamErrorCodes { MeshMismatch = 0x0400001 };
}

namespace Geometry {
enum _GeometryErrorCodes {
  InvalidFormat = 0x0100001,
};
}

namespace Output {
enum _outputErrorCodes {
  OutputReadError = 0x0500001,
  NetcdfVarNotFound = 0x0500002,
  NetcdfTooManyVariablesFound = 0x0500003
};
}

std::string errorString(int code);
}  // namespace Adcirc

#endif  // ERROR_H
