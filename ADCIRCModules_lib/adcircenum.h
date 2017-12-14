#ifndef ADCIRCENUM_H
#define ADCIRCENUM_H

namespace Adcirc{

  enum _genericErrorCodes{
    NoError         = 0x0000000,
    MissingFile     = 0x0000001,
    FileOpenError   = 0x0000002,
    FileReadError   = 0x0000003,
    Proj4Error      = 0x0000004,
    KdtreeError     = 0x0000005
  };

  namespace Mesh{
    enum _meshErrorCodes{
      InvalidFormat = 0x0100001,
      UnknownError  = 0x0100002
    };
  }

  namespace Node{
    enum _nodeErrorCodes{
      NoError       = 0x0200000
    };
  }

  namespace Element{
    enum _elementErrorCodes{
      NoError       = 0x0300000
    };
  }

}

#endif // ADCIRCENUM_H
