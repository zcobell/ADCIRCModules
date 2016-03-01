# QADCModules
Implementation of routines used for to manipulate ADCIRC model files in Qt

[![Build Status](https://travis-ci.org/zcobell/QADCModules.svg?branch=master)](https://travis-ci.org/zcobell/QADCModules)

###Including QADCModules in a Qt Project
1. Add the path to the Library to the .pro file. For windows this will be a DLL and Unix a .so

        LIBS+= -L/path/to/library -lQADCModules
        
2. Add the include path to the main header file to the .pro file

        INCLUDEPATH+= /path/to/QADCModules
        
3. Include the main header for the project in the source code

        #include "QADCModules.h"

## Examples (so far)

###ADCIRC Meshes (fort.14)
1. Create a mesh object outside of a QObject

        QPointer<adcirc_mesh> thisMesh = new adcirc_mesh();

2. Create a mesh object inside of a QObject

        adcirc_mesh *thisMesh = new adcirc_mesh(this);

3. Read an ADCIRC mesh

        thisMesh->read("/path/to/mesh/fort.14");

4. Write an ADCIRC mesh

        thisMesh->write("/path/to/output/fort.14");

5. Diagnose an error while working with a mesh object

        qDebug() << thisMesh->error->getErrorString();

###ADCIRC Nodal Attributes (fort.13)
1. Create a nodal attributes object outside of a QObject

        QPointer<adcirc_nodalattributes> thisAttributes = new adcirc_nodalattributes();
        
2. Create a nodal attributes object inside of a QObject

        adcirc_nodalattributes *thisAttributes = new adcirc_nodalattributes(this);
        
3. Read the nodal attributes file

        thisAttributes->read("/path/to/nodalattributes/fort.13");

4. Write the nodal attributes file

        thisAttributes->write("/path/to/output/fort.13");

5. Diagnose an error while working with a nodal attributes object

        qDebug() << thisAttributes->error->getErrorString();
