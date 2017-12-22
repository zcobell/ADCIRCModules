
/* Adcirc Interface File */
#if defined(SWIGPYTHON)
%module pyadcirc
#endif

#if defined(SWIGPERL)
%module perladcirc
#endif

%{
#define SWIG_FILE_WITH_INIT
#include "adcircmesh.h"
#include "adcircnode.h"
#include "adcircelement.h"
#include "adcircboundary.h"
#include "qkdtree2.h"
#include "qproj4.h"
#include "point.h"
%}

%include "adcircmesh.h"
%include "adcircnode.h"
%include "adcircelement.h"
%include "adcircboundary.h"
%include "qkdtree2.h"
%include "qproj4.h"
%include "point.h"
