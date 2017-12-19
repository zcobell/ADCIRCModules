
/* PyAdcirc Interface File */
%module pyadcirc

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
