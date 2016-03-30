/*-------------------------------GPL-------------------------------------//
//
// QADCModules - A library for working with ADCIRC models
// Copyright (C) 2016  Zach Cobell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------*/
#include "adcirc_global_output.h"
#include "netcdf.h"
#include "shapefil.h"

//-----------------------------------------------------------------------------------------//
// Constructor for an adcirc_global_output object
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_global_output::adcirc_global_output(QString filename, QObject *parent) : QObject(parent)
 * \brief Constructor for an adcirc_global_output object
 *
 * @param[in] filename filename of the ADCIRC output file to read
 * @param[in] parent   QObject reference
 *
 * Constructor for an adcirc_global_output object
 *
 */
//-----------------------------------------------------------------------------------------//
adcirc_global_output::adcirc_global_output(QString filename, QObject *parent) : QObject(parent)
{
    this->dontReadMesh = false;
    this->filename = filename;
    this->isMeshInitialized = false;
    this->initializeNetcdfVariables();
    this->error = new QADCModules_errors(this);
    this->outputData = NULL;
    this->mesh = NULL;
    this->lastRecordRead = 0;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Constructor for an adcirc_global_output object
//-----------------------------------------------------------------------------------------//
/**
 * \overload adcirc_global_output::adcirc_global_output(QString filename, QObject *parent) : QObject(parent)
 * \brief Constructor for an adcirc_global_output object
 *
 * @param[in] filename    filename of the ADCIRC output file to read
 * @param[in] ignoreMesh  Skip reading the adcirc mesh contained within the netCDF output file
 * @param[in] parent      QObject reference
 *
 * Constructor for an adcirc_global_output object
 *
 */
//-----------------------------------------------------------------------------------------//
adcirc_global_output::adcirc_global_output(QString filename, bool ignoreMesh, QObject *parent) : QObject(parent)
{
    this->dontReadMesh = ignoreMesh;
    this->filename = filename;
    this->isMeshInitialized = false;
    this->initializeNetcdfVariables();
    this->error = new QADCModules_errors(this);
    this->outputData = NULL;
    this->lastRecordRead = 0;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Destructor for an adcirc_global_output object
//-----------------------------------------------------------------------------------------//
/** \brief Destructor for an adcirc_global_output object
 *
 * Destructor for an adcirc_global_output object
 *
 */
//-----------------------------------------------------------------------------------------//
adcirc_global_output::~adcirc_global_output()
{
    if(this->readFid.isOpen())
        this->readFid.close();

    if(this->writeFid.isOpen())
        this->writeFid.close();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to read from an ascii ADCIRC output file
//-----------------------------------------------------------------------------------------//
/** \brief Function that reads the header of an ascii ADCIRC global output file
 *
 * Function that reads opens and reads the header of an ascii ADCIRC global output file.
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_global_output::readAscii()
{

    int ierr;

    //...Check that the filename is valid
    if(this->filename.isEmpty() || this->filename.isNull())
    {
        this->error->setError(ERROR_FILENOEXIST);
        return this->error->getError();
    }

    //...Check that the file exists
    QFile testFile(this->filename);
    if(!testFile.exists())
    {
        this->error->setError(ERROR_FILENOEXIST);
        return this->error->getError();
    }

    //...Read the ASCII header
    ierr = this->readAdcircGlobalOutputAsciiHeader();
    if(ierr!=ERROR_NOERROR)
    {
        this->error->setError(ierr);
        return this->error->getError();
    }

    //...Read the first record of the global output file
    ierr = this->readNextAdcircGlobalOutputAscii();
    if(ierr!=ERROR_NOERROR)
    {
        this->error->setError(ierr);
        return this->error->getError();
    }

    return ERROR_NOERROR;

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to read the next record from an adcirc ascii global file
//-----------------------------------------------------------------------------------------//
/** \brief Function to read the next record from the adcirc output file
 *
 * Function to read the next record from the adcirc output file
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_global_output::readNextAscii()
{
    int ierr = this->readNextAdcircGlobalOutputAscii();
    this->error->setError(ierr);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Public function to read an ADCIRC netCDF output file
//-----------------------------------------------------------------------------------------//
/** \brief Public function to read an ADCIRC output file
 *
 * @param[in] record Record number to read from the file. Note: first record is record 1, not 0.
 *
 * Public function to read an ADCIRC output file
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_global_output::readNetCDF(int record)
{
    int ierr,ncid;

    //...Check that the filename is valid
    if(this->filename.isEmpty() || this->filename.isNull())
    {
        this->error->setError(ERROR_FILENOEXIST);
        return this->error->getError();
    }

    //...Check that the file exists
    QFile testFile(this->filename);
    if(!testFile.exists())
    {
        this->error->setError(ERROR_FILENOEXIST);
        return this->error->getError();
    }

    //...Determine if the file is a netCDF file. Attempt to open if. If
    //   no error is returned, then we can assume that the file is netCDF
    //   formatted
    ierr = nc_open(this->filename.toStdString().c_str(),NC_NOWRITE,&ncid);

    if(ierr==NC_NOERR)
    {
        //...Close the dummy attempt to open the file
        ierr = nc_close(ncid);

        //...Call the read routine
        ierr = this->readAdcircGlobalOutputNetCDF(record);
    }
    else
    {
        this->error->setError(ERROR_ADCIRCOUTPUT_NOTNETCDF);
        return this->error->getError();
    }

    this->error->setError(ierr);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to set the list of netCDF variables in an ADCIRC output file
//-----------------------------------------------------------------------------------------//
/** \brief Function to set the list of netCDF variables in an ADCIRC output file
 *
 * Function to set the list of netCDF variables in an ADCIRC output file
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_global_output::initializeNetcdfVariables()
{
    this->netCDFVariables.clear();
    this->netCDFVariables.append("sigmat");
    this->netCDFVariables.append("salinity");
    this->netCDFVariables.append("temperature");
    this->netCDFVariables.append("u-vel3D");
    this->netCDFVariables.append("v-vel3D");
    this->netCDFVariables.append("w-vel3D");
    this->netCDFVariables.append("q20");
    this->netCDFVariables.append("l");
    this->netCDFVariables.append("ev");
    this->netCDFVariables.append("qsurfkp1");
    this->netCDFVariables.append("zeta");
    this->netCDFVariables.append("zeta_max");
    this->netCDFVariables.append("u-vel");
    this->netCDFVariables.append("v-vel");
    this->netCDFVariables.append("vel_max");
    this->netCDFVariables.append("pressure");
    this->netCDFVariables.append("pressure_min");
    this->netCDFVariables.append("windx");
    this->netCDFVariables.append("windy");
    this->netCDFVariables.append("wind_max");
    this->netCDFVariables.append("radstress_x");
    this->netCDFVariables.append("radstress_y");
    this->netCDFVariables.append("radstress_max");
    this->netCDFVariables.append("swan_HS");
    this->netCDFVariables.append("swan_HS_max");
    this->netCDFVariables.append("swan_DIR");
    this->netCDFVariables.append("swan_DIR_max");
    this->netCDFVariables.append("swan_TM01");
    this->netCDFVariables.append("swan_TM01_max");
    this->netCDFVariables.append("swan_TPS");
    this->netCDFVariables.append("swan_TPS_max");
    this->netCDFVariables.append("swan_windx");
    this->netCDFVariables.append("swan_windy");
    this->netCDFVariables.append("swan_wind_max");
    this->netCDFVariables.append("swan_TM02");
    this->netCDFVariables.append("swan_TM02_max");
    this->netCDFVariables.append("swan_TMM10");
    this->netCDFVariables.append("swan_TMM10_max");
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to find the netCDF variable in an ADCIRC output file
//-----------------------------------------------------------------------------------------//
/** \brief Function to find the netCDF variable in an ADCIRC output file
 *
 * @param[in]  ncid          netCDF file identifier
 * @param[out] numVariables  number of columns (scalar or vector) in an ADCIRC output file
 * @param[out] variable1     netCDF variable ID for first netCDF variable
 * @param[out] variable2     netCDF variable ID for the second netCDF variable
 *
 * Function to find the netCDF variable in an ADCIRC output file
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_global_output::findNetCDFVariables(int &ncid, QVector<int> &varIDs)
{
    QString varname;
    int i,ierr,varid;

    for(i=0;i<this->netCDFVariables.size();i++)
    {
        varname = this->netCDFVariables.at(i);
        ierr = nc_inq_varid(ncid,varname.toStdString().c_str(),&varid);
        if(ierr==NC_NOERR)
            varIDs.push_back(varid);
    }

    if(varIDs.size()==0)
        return ERROR_ADCIRCOUTPUT_NOVARIABLE;
    else if(varIDs.size()>2)
        return ERROR_ADCIRCOUTPUT_TOOMANYVARS;
    else
        return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to read from a netCDF ADCIRC output file
//-----------------------------------------------------------------------------------------//
/** \brief Function to read from a netCDF ADCIRC output file
 *
 * @param[in]  record  record to read from the netCDF file
 *
 * Function to read from a netCDF ADCIRC output file
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_global_output::readAdcircGlobalOutputNetCDF(int record)
{
    int i,ierr,numColumns;
    int ncid,dimid_time,dimid_node,varid_time;
    int start_int,count_int;
    double *timeList,*column1,*column2;
    double dtdp,fillValue;
    QVector<int> varIDs;
    size_t nSnaps,nNodes;

    //...Check if the file exists
    QFile thisRegFile(this->filename);
    if(!thisRegFile.exists())
    {
        this->error->setError(ERROR_FILENOEXIST);
        return this->error->getError();
    }

    //...First time here, we need to read the ADCIRC mesh data (nodes, elements only)
    if(!this->isMeshInitialized)
    {
        if(!this->dontReadMesh)
        {
            this->mesh = new adcirc_mesh(this);
            ierr = this->mesh->readNetCDF(this->filename);
            if(ierr == ERROR_NOERROR)
                this->isMeshInitialized = true;
            else
            {
                this->error->setError(ierr);
                return this->error->getError();
            }
        }
    }

    //...Open the netCDF file
    ierr = nc_open(this->filename.toStdString().c_str(),NC_NOWRITE,&ncid);

    //...Get the dimid for time
    ierr = nc_inq_dimid(ncid,"time",&dimid_time);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    //...Get the varid for time
    ierr = nc_inq_varid(ncid,"time",&varid_time);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    //...Check if the record requested is valid
    ierr = nc_inq_dimlen(ncid,dimid_time,&nSnaps);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    this->numRecords = static_cast<int>(nSnaps);
    if(record > this->numRecords)
    {
        this->error->setError(ERROR_ADCIRCOUTPUT_EXCEEDEDSDIM);
        return this->error->getError();
    }

    //...Get the number of nodes in the adcirc_global_output file
    ierr = nc_inq_dimid(ncid,"node",&dimid_node);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_inq_dimlen(ncid,dimid_node,&nNodes);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }
    this->numNodes = static_cast<int>(nNodes);

    //...Find the netCDF variable that is present in this file
    ierr = this->findNetCDFVariables(ncid,varIDs);
    if(ierr!=ERROR_NOERROR)
    {
        this->error->setError(ierr);
        return this->error->getError();
    }
    numColumns = varIDs.size();

    start_int             = (record-1)*this->numNodes;
    count_int             =  this->numNodes;
    static size_t start[] = {static_cast<size_t>(start_int)};
    static size_t count[] = {nNodes};

    //...Allocate memory
    timeList = (double*)malloc(sizeof(double)*nSnaps);
    column1  = (double*)malloc(sizeof(double)*nNodes);
    if(numColumns==2)
        column2 = (double*)malloc(sizeof(double)*nNodes);

    //...Read data from netCDF file
    ierr = nc_get_var(ncid,varid_time,timeList);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_get_vara_double(ncid,varIDs.at(0),start,count,column1);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    if(numColumns==2)
    {
        ierr = nc_get_vara_double(ncid,varIDs.at(1),start,count,column2);
        if(ierr!=NC_NOERR)
        {
            this->error->setCustomDescription(nc_strerror(ierr));
            this->error->setError(ERROR_NETCDF_GENERIC);
            return this->error->getError();
        }
    }

    //...Save the data into the output variable
    if(this->outputData!=NULL)
        delete this->outputData;
    this->outputData = new adcirc_output_record(this->numNodes,this);
    this->numColumns = numColumns;
    if(numColumns==1)
    {
        this->outputData->scalar.resize(this->numNodes);
        for(i=0;i<this->numNodes;i++)
            this->outputData->scalar[i] = column1[i];
    }
    else if(numColumns==2)
    {
        this->outputData->vector_u.resize(this->numNodes);
        this->outputData->vector_v.resize(this->numNodes);
        for(i=0;i<this->numNodes;i++)
        {
            this->outputData->vector_u[i] = column1[i];
            this->outputData->vector_v[i] = column2[i];
        }
    }

    //...Read a select set of the metadata
    ierr = nc_get_att(ncid,NC_GLOBAL,"dt",&dtdp);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }
    this->outputData->modelStep = qRound(dtdp*timeList[record-1]);
    this->outputData->modelTime = timeList[record-1];

    ierr = nc_get_att(ncid,varIDs.at(0),"_FillValue",&fillValue);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }
    this->defaultValue = fillValue;

    //...Save the last record read
    this->lastRecordRead = record;

    //...Close the file
    ierr = nc_close(ncid);

    //...Deallocate memory
    free(timeList);
    free(column1);
    if(numColumns==2)
        free(column2);

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function that reads the header from an ADCIRC global ASCII file
//-----------------------------------------------------------------------------------------//
/** \brief Function that reads the header from an ADCIRC global ASCII file
 *
 * Function that reads the header from an ADCIRC global ASCII file. Uses readFID to keep
 * the file open so that other routines can read subsequent records
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_global_output::readAdcircGlobalOutputAsciiHeader()
{
    QString fileData,tempString;
    QStringList fileDataList;
    bool err;

    this->readFid.setFileName(this->filename);

    if(!this->readFid.open(QIODevice::ReadOnly))
    {
        this->error->setError(ERROR_FILEOPENERR);
        return this->error->getError();
    }

    fileData    = this->readFid.readLine().simplified();
    this->title = fileData;

    fileData     = this->readFid.readLine().simplified();
    fileDataList = fileData.split(" ");

    tempString       = fileDataList.value(0);
    this->numRecords = tempString.toInt(&err);
    if(!err)
    {
        this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
        return this->error->getError();
    }

    tempString = fileDataList.value(1);
    this->numNodes = tempString.toInt(&err);
    if(!err)
    {
        this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
        return this->error->getError();
    }

    tempString = fileDataList.value(2);
    this->timeBetweenSnaps = tempString.toDouble(&err);
    if(!err)
    {
        this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
        return this->error->getError();
    }

    tempString = fileDataList.value(3);
    this->timestepsBetweenSnaps = tempString.toDouble(&err);
    if(!err)
    {
        this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
        return this->error->getError();
    }

    tempString = fileDataList.value(4);
    this->numColumns = tempString.toInt(&err);
    if(!err)
    {
        this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
        return this->error->getError();
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function that reads the next record from the adcirc output file
//-----------------------------------------------------------------------------------------//
/** \brief Function that reads the next record from the adcirc_global_output file
 *
 * Function that reads the next record from the adcirc_global_output file
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_global_output::readNextAdcircGlobalOutputAscii()
{
    QString fileData,tempString;
    QStringList fileDataList;
    double defaultValue,value1,value2;
    int i,numNonDefault,node;
    bool err,sparse;

    this->lastRecordRead = this->lastRecordRead + 1;

    //...Read the individual record header
    fileData     = this->readFid.readLine().simplified();
    fileDataList = fileData.split(" ");

    //...If necessary, create a new record object
    if(this->outputData==NULL)
        this->outputData = new adcirc_output_record(this->numNodes,this);

    //...Check if the file format is sparse or full
    if(fileDataList.size()==2)
        sparse = false;
    else
        sparse = true;

    //...Model time for this record
    tempString = fileDataList.value(0);
    this->outputData->modelTime = tempString.toDouble(&err);
    if(!err)
    {
        this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
        return this->error->getError();
    }

    //...Model time step for this record
    tempString = fileDataList.value(1);
    this->outputData->modelStep = tempString.toInt(&err);
    if(!err)
    {
        this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
        return this->error->getError();
    }

    //...Read the default value information for sparse formatted files
    if(sparse)
    {
        tempString = fileDataList.value(2);
        numNonDefault = tempString.toInt(&err);
        if(!err)
        {
            this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
            return this->error->getError();
        }
        tempString = fileDataList.value(3);
        defaultValue = tempString.toInt(&err);
        if(!err)
        {
            this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
            return this->error->getError();
        }
    }
    else
    {
        this->defaultValue = -99999.0;
        numNonDefault      = this->numNodes;
    }

    //...Allocations
    if(this->numColumns==1)
    {
        this->outputData->scalar.resize(this->numNodes);
        this->outputData->scalar.fill(this->defaultValue);
    }
    else
    {
        this->outputData->vector_u.resize(this->numNodes);
        this->outputData->vector_v.resize(this->numNodes);
        this->outputData->vector_u.fill(this->defaultValue);
        this->outputData->vector_v.fill(this->defaultValue);
    }

    //...Loop over the file
    for(i=0;i<numNonDefault;i++)
    {
        fileData     = this->readFid.readLine().simplified();
        fileDataList = fileData.split(" ");
        tempString   = fileDataList.value(0);
        node         = tempString.toInt(&err);
        if(!err)
        {
            this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
            return this->error->getError();
        }
        if(this->numColumns==1)
        {
            tempString = fileDataList.value(1);
            value1     = tempString.toDouble(&err);
            if(!err)
            {
                this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
                return this->error->getError();
            }
            this->outputData->scalar[node-1] = value1;
        }
        else
        {
            tempString = fileDataList.value(1);
            value1     = tempString.toDouble(&err);
            if(!err)
            {
                this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
                return this->error->getError();
            }
            this->outputData->vector_u[node-1] = value1;

            tempString = fileDataList.value(2);
            value2     = tempString.toDouble(&err);
            if(!err)
            {
                this->error->setError(ERROR_ADCIRCOUTPUT_ASCIIREADERROR);
                return this->error->getError();
            }
            this->outputData->vector_v[node-1] = value1;
        }
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that writes an adcirc_mesh object in shapefile format
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_global_output::toShapefile(QString outputFile)
 * \brief Function that writes an adcirc_global_output object in shapefile format
 *
 * @param[in]  outputFile  name of output file (.shp/.shx/.dbf) to write. Extension will be handled
 *                         automatically, so supplying a .shp extension is sufficient.
 *
 * Function that writes an adcirc_global_output object in shapefile format
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_global_output::toShapefile(QString outputFile)
{

    SHPHandle shpid;
    DBFHandle dbfid;
    SHPObject *shpobj;
    int i,shp_index,nodeid;
    double latitude,longitude,elevation,outputValue,vector_u,vector_v;

    if(this->mesh!=NULL)
    {

        shpid = SHPCreate(outputFile.toStdString().c_str(),SHPT_POINT);
        dbfid = DBFCreate(outputFile.toStdString().c_str());

        DBFAddField(dbfid,"nodeid",FTInteger,16,0);
        DBFAddField(dbfid,"longitude",FTDouble,16,8);
        DBFAddField(dbfid,"latitude",FTDouble,16,8);
        DBFAddField(dbfid,"elevation",FTDouble,16,4);

        if(this->numColumns==1)
            DBFAddField(dbfid,"outputValue",FTDouble,16,4);
        else
        {
            DBFAddField(dbfid,"vector_u",FTDouble,16,4);
            DBFAddField(dbfid,"vector_v",FTDouble,16,4);
        }


        for(i=0;i<this->numNodes;i++)
        {

            nodeid = this->mesh->nodes[i]->id;
            longitude = static_cast<double>(this->mesh->nodes[i]->position.x());
            latitude = static_cast<double>(this->mesh->nodes[i]->position.y());
            elevation = static_cast<double>(this->mesh->nodes[i]->position.z());
            if(this->numColumns==1)
                outputValue = this->outputData->scalar[i];
            else
            {
                vector_u = this->outputData->vector_u[i];
                vector_v = this->outputData->vector_v[i];
            }

            shpobj = SHPCreateSimpleObject(SHPT_POINT,1,&longitude,&latitude,&elevation);
            shp_index = SHPWriteObject(shpid,-1,shpobj);
            SHPDestroyObject(shpobj);

            DBFWriteIntegerAttribute(dbfid,shp_index,0,nodeid);
            DBFWriteDoubleAttribute(dbfid,shp_index,1,longitude);
            DBFWriteDoubleAttribute(dbfid,shp_index,2,latitude);
            DBFWriteDoubleAttribute(dbfid,shp_index,3,elevation);

            if(this->numColumns==1)
                DBFWriteDoubleAttribute(dbfid,shp_index,4,outputValue);
            else
            {
                DBFWriteDoubleAttribute(dbfid,shp_index,5,vector_u);
                DBFWriteDoubleAttribute(dbfid,shp_index,5,vector_v);
            }

        }

        SHPClose(shpid);

        return ERROR_NOERROR;
    }
    else
    {
        this->error->setError(ERROR_ADCIRCOUTPUT_MESHNOTINIT);
        return this->error->getError();
    }

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that writes an adcirc_mesh object in shapefile format
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_global_output::setMesh(adcirc_mesh *mesh)
 * \brief Function that sets the underlying mesh to use for functions in this class
 *
 * @param[in]  *mesh the underlying mesh to use for functions in this class
 *
 * Function that sets the underlying mesh to use for functions in this class if it is not
 * already defined by the netCDF read routines.
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_global_output::setMesh(adcirc_mesh *mesh)
{
    this->mesh = mesh;
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//
