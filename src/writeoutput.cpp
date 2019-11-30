/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2019 Zachary Cobell
//
// ADCIRCModules is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ADCIRCModules is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ADCIRCModules.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------*/

//...Hack for multiple definition
//   This seems to be an issue with the
//   build on some versions of windows
#if H5_SIZEOF_SSIZE_T == 0
#define H5_SIZEOF_SSIZE_T H5_SIZEOF_LONG_LONG
#endif

#include "writeoutput.h"

#include <cstring>

#include "adcirc_outputfiles.h"
#include "formatting.h"
#include "hdf5.h"
#include "logging.h"
#include "netcdf.h"

using namespace Adcirc::Output;

WriteOutput::WriteOutput(const std::string &filename,
                         Adcirc::Output::ReadOutput *dataContainer,
                         Adcirc::Geometry::Mesh *mesh)
    : m_dataContainer(dataContainer),
      m_mesh(mesh),
      m_filename(filename),
      m_recordsWritten(0) {
  this->m_format = Adcirc::Output::getOutputFormatFromExtension(filename);
  this->m_isOpen = false;
}

WriteOutput::~WriteOutput() {
  if (this->m_isOpen) this->close();
}

void WriteOutput::open() {
  if (this->m_format == Adcirc::Output::OutputAsciiFull ||
      this->m_format == Adcirc::Output::OutputAsciiSparse) {
    this->openFileAscii();
  } else if (this->m_format == Adcirc::Output::OutputNetcdf4) {
    this->openFileNetCDF();
  } else if (this->m_format == Adcirc::Output::OutputHdf5) {
    this->openFileHdf5();
  }
  this->m_isOpen = true;
}

void WriteOutput::close() {
  if (this->m_format == Adcirc::Output::OutputAsciiFull ||
      this->m_format == Adcirc::Output::OutputAsciiSparse) {
    if (this->m_fid.is_open()) this->m_fid.close();
  } else if (this->m_format == Adcirc::Output::OutputNetcdf4 ||
             this->m_format == Adcirc::Output::OutputNetcdf3) {
    nc_close(this->m_ncid);
  } else if (this->m_format == Adcirc::Output::OutputHdf5) {
    H5Fclose(this->m_h5fid);
  }
  this->m_isOpen = false;
}

void WriteOutput::writeSparseAscii(bool s) {
  if (this->m_format == Adcirc::Output::OutputAsciiFull ||
      this->m_format == Adcirc::Output::OutputAsciiSparse) {
    if (s) {
      this->m_format = Adcirc::Output::OutputAsciiSparse;
    } else {
      this->m_format = Adcirc::Output::OutputAsciiFull;
    }
  }
}

void WriteOutput::write(const OutputRecord *record,
                        const OutputRecord *record2) {
  if (!this->m_isOpen) {
    adcircmodules_throw_exception("WriteOutput: File has not been opened.");
  }
  if (this->m_format == Adcirc::Output::OutputAsciiFull) {
    this->writeRecordAsciiFull(record);
  } else if (this->m_format == Adcirc::Output::OutputAsciiSparse) {
    this->writeRecordAsciiSparse(record);
  } else if (this->m_format == Adcirc::Output::OutputNetcdf3 ||
             this->m_format == Adcirc::Output::OutputNetcdf4) {
    this->writeRecordNetCDF(record);
  } else if (this->m_format == Adcirc::Output::OutputHdf5) {
    this->writeRecordHdf5(record, record2);
  }
  this->m_recordsWritten++;
  return;
}

void WriteOutput::openFileAscii() {
  this->m_fid.open(this->filename(), std::ios::out);
  this->m_fid << this->m_dataContainer->header() << std::endl;
  this->m_fid << Adcirc::Output::Formatting::adcircFileHeader(
      this->m_dataContainer->numSnaps(), this->m_dataContainer->numNodes(),
      this->m_dataContainer->dt(), this->m_dataContainer->dIteration(),
      this->m_dataContainer->metadata()->dimension());
  return;
}

int WriteOutput::defineNetcdfVariable(int dimid_node, const int *dims,
                                      double fill, size_t index) {
  int varid_v, ierr = NC_NOERR;
  if (this->m_dataContainer->metadata()->isMax()) {
    ierr +=
        nc_def_var(this->m_ncid,
                   this->m_dataContainer->metadata()->variable(index).c_str(),
                   NC_DOUBLE, 1, &dimid_node, &varid_v);
  } else {
    ierr +=
        nc_def_var(this->m_ncid,
                   this->m_dataContainer->metadata()->variable(index).c_str(),
                   NC_DOUBLE, 2, dims, &varid_v);
  }
  ierr += nc_put_att_text(
      this->m_ncid, varid_v, "long_name",
      this->m_dataContainer->metadata()->longName(index).size(),
      this->m_dataContainer->metadata()->longName(index).c_str());
  ierr += nc_put_att_text(
      this->m_ncid, varid_v, "standard_name",
      this->m_dataContainer->metadata()->standardName(index).size(),
      this->m_dataContainer->metadata()->standardName(index).c_str());
  ierr +=
      nc_put_att_text(this->m_ncid, varid_v, "units",
                      this->m_dataContainer->metadata()->units(index).size(),
                      this->m_dataContainer->metadata()->units(index).c_str());
  ierr += nc_def_var_fill(this->m_ncid, varid_v, 0, &fill);
  ierr += nc_put_att_double(this->m_ncid, varid_v, "dry_value", NC_DOUBLE, 1,
                            &fill);
  ierr += nc_put_att_text(this->m_ncid, varid_v, "coordinates", 8, "time y x");
  ierr += nc_put_att_text(this->m_ncid, varid_v, "location", 4, "node");
  ierr += nc_put_att_text(this->m_ncid, varid_v, "mesh", 11, "adcirc_mesh");
  if (this->m_dataContainer->metadata()->convention(index) !=
      OutputMetadata::defaultConvention()) {
    ierr += nc_put_att_text(
        this->m_ncid, varid_v, "positive",
        this->m_dataContainer->metadata()->convention(index).size(),
        this->m_dataContainer->metadata()->convention(index).c_str());
  }

  if (this->m_format == Adcirc::Output::OutputNetcdf4) {
    ierr += nc_def_var_deflate(this->m_ncid, varid_v, 1, 1, 2);
  }
  return varid_v;
}

void WriteOutput::openFileNetCDF() {
  int ierr = nc_create(this->filename().c_str(), NC_NETCDF4, &this->m_ncid);
  int dimid_time, dimid_node, dimid_ele, dimid_nvertex, dimid_mesh;

  size_t nn, ne;
  if (this->m_mesh != nullptr) {
    nn = this->m_mesh->numNodes();
    ne = this->m_mesh->numElements();
  } else {
    nn = this->m_dataContainer->numNodes();
    ne = 0;
  }

  ierr += nc_def_dim(this->m_ncid, "time", NC_UNLIMITED, &dimid_time);
  ierr += nc_def_dim(this->m_ncid, "node", nn, &dimid_node);
  ierr += nc_def_dim(this->m_ncid, "ele", ne, &dimid_ele);
  ierr += nc_def_dim(this->m_ncid, "nvertex", 3, &dimid_nvertex);
  ierr += nc_def_dim(this->m_ncid, "mesh", 1, &dimid_mesh);

  const int dim_e[2] = {dimid_ele, dimid_nvertex};
  int varid_x, varid_y, varid_element, varid_mesh, varid_depth;

  ierr += nc_def_var(this->m_ncid, "time", NC_DOUBLE, 1, &dimid_time,
                     &this->m_varid_time);
  ierr += nc_def_var(this->m_ncid, "x", NC_DOUBLE, 1, &dimid_node, &varid_x);
  ierr += nc_def_var(this->m_ncid, "y", NC_DOUBLE, 1, &dimid_node, &varid_y);
  ierr += nc_def_var(this->m_ncid, "element", NC_INT, 2, dim_e, &varid_element);
  ierr += nc_def_var(this->m_ncid, "mesh", NC_INT, 1, &dimid_mesh, &varid_mesh);
  ierr += nc_def_var(this->m_ncid, "depth", NC_DOUBLE, 1, &dimid_node,
                     &varid_depth);

  if (this->m_format == Adcirc::Output::OutputNetcdf4) {
    ierr += nc_def_var_deflate(this->m_ncid, this->m_varid_time, 1, 1, 2);
    ierr += nc_def_var_deflate(this->m_ncid, varid_x, 1, 1, 2);
    ierr += nc_def_var_deflate(this->m_ncid, varid_y, 1, 1, 2);
    ierr += nc_def_var_deflate(this->m_ncid, varid_depth, 1, 1, 2);
    ierr += nc_def_var_deflate(this->m_ncid, varid_element, 1, 1, 2);
  }

  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception(
        "WritOutput: Error initializing netCDF output file.");
    return;
  }

  double fill = this->m_dataContainer->defaultValue();
  const int dims[2] = {dimid_time, dimid_node};

  //...Check if there are variables to define in the metadata. If not,
  //   need to assume some defaults so the code can function
  if (this->m_dataContainer->metadata()->dimension() == 1) {
    if (this->m_dataContainer->metadata()->variable(0) == std::string()) {
      this->m_dataContainer->setMetadata(c_outputMetadata[4]);
    }
  } else if (this->m_dataContainer->metadata()->dimension() == 2) {
    if (this->m_dataContainer->metadata()->variable(1) == std::string()) {
      this->m_dataContainer->setMetadata(c_outputMetadata[7]);
    }
  } else if (this->m_dataContainer->metadata()->dimension() == 3) {
    if (this->m_dataContainer->metadata()->variable(2) == std::string()) {
      this->m_dataContainer->setMetadata(c_outputMetadata[1]);
    }
  }

  for (size_t i = 0; i < this->m_dataContainer->metadata()->dimension(); ++i) {
    this->m_varid.push_back(
        this->defineNetcdfVariable(dimid_node, dims, fill, i));
  }

  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception(
        "WriteOutput: Error initializing variables in netCDF output file.");
    return;
  }

  double dt = this->m_dataContainer->modelDt();

  //...Global attribute section
  ierr += nc_put_att_double(this->m_ncid, NC_GLOBAL, "dt", NC_DOUBLE, 1, &dt);
  ierr += nc_put_att_double(this->m_ncid, NC_GLOBAL, "_FillValue", NC_DOUBLE, 1,
                            &fill);
  ierr += nc_put_att_text(this->m_ncid, NC_GLOBAL, "model", 6, "ADCIRC");
  ierr +=
      nc_put_att_text(this->m_ncid, NC_GLOBAL, "version", 13, "AdcircModules");
  ierr +=
      nc_put_att_text(this->m_ncid, NC_GLOBAL, "grid_type", 10, "triangular");
  ierr += nc_put_att_text(this->m_ncid, NC_GLOBAL, "conventions", 11,
                          "UGRID-0.9.0");
  ierr += nc_enddef(this->m_ncid);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("WriteOutput: Error defining attributes.");
    return;
  }

  if (this->m_mesh != nullptr) {
    ierr += nc_put_var(this->m_ncid, varid_x, &this->m_mesh->x()[0]);
    ierr += nc_put_var(this->m_ncid, varid_y, &this->m_mesh->y()[0]);
    ierr += nc_put_var(this->m_ncid, varid_depth, &this->m_mesh->z()[0]);
  }

  return;
}

void WriteOutput::h5_createPropertiesGroup(int64_t gid, bool isVector) {
  hid_t p_gid =
      H5Gcreate2(gid, "PROPERTIES", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  this->h5_defineStringAttribute(p_gid, "Grouptype", "PROPERTIES");

  char data[] = "MESH2DDATAOBJ";
  hsize_t dims[1] = {1};
  hid_t type = H5Tcopy(H5T_C_S1);
  size_t len = 14;
  herr_t status = H5Tset_size(type, len);
  hid_t ds = H5Screate_simple(1, dims, dims);
  hid_t did = H5Dcreate2(p_gid, "Object Type", type, ds, H5P_DEFAULT,
                         H5P_DEFAULT, H5P_DEFAULT);
  hid_t fs = H5Dget_space(did);
  H5Dwrite(did, type, ds, fs, H5P_DEFAULT, data);
  H5Sclose(ds);
  H5Dclose(did);

  if (!isVector) {
    float nullvalue[1] = {Adcirc::Output::defaultOutputValue()};
    hsize_t dims[1] = {1};
    hid_t ns = H5Screate_simple(1, dims, dims);
    hid_t did_ns = H5Dcreate2(p_gid, "nullvalue", H5T_IEEE_F32LE, ns,
                              H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    hid_t fs_ns = H5Dget_space(did_ns);
    H5Dwrite(did_ns, H5T_IEEE_F32LE, H5P_DEFAULT, fs_ns, H5P_DEFAULT,
             nullvalue);
    H5Sclose(ns);
    H5Sclose(fs_ns);
    H5Dclose(did_ns);
  }

  H5Gclose(p_gid);
  return;
}

void WriteOutput::h5_createDataset(const std::string &name, bool isVector) {
  //...Create metadata
  const hsize_t dims_tm[1] = {0};
  const hsize_t dims_val[3] = {0, this->m_dataContainer->numNodes(), 2};
  const hsize_t dims_tm_max[1] = {H5S_UNLIMITED};
  const hsize_t dims_val_max[3] = {H5S_UNLIMITED,
                                   this->m_dataContainer->numNodes(), 2};
  const hsize_t dims_tm_chunk[1] = {1};
  const hsize_t dims_val_chunk[3] = {1, this->m_dataContainer->numNodes(), 1};

  //...Storage chunking
  hid_t props_tm = H5Pcreate(H5P_DATASET_CREATE);
  hid_t props_vl = H5Pcreate(H5P_DATASET_CREATE);
  hid_t props_mx = H5Pcreate(H5P_DATASET_CREATE);
  hid_t props_mn = H5Pcreate(H5P_DATASET_CREATE);
  hid_t props_ac = H5Pcreate(H5P_DATASET_CREATE);

  //...Create group
  hid_t gid = H5Gcreate2(this->m_h5fid, name.c_str(), H5P_DEFAULT, H5P_DEFAULT,
                         H5P_DEFAULT);

  this->h5_defineIntegerAttribute(gid, "Data Type", 0);
  this->h5_defineIntegerAttribute(gid, "DatasetCompression", 1);
  this->h5_defineStringAttribute(gid, "DatasetUnits", "None");
  if (isVector) {
    this->h5_defineStringAttribute(gid, "Grouptype", "DATASET VECTOR");
  } else {
    this->h5_defineStringAttribute(gid, "Grouptype", "DATASET SCALAR");
  }
  this->h5_defineStringAttribute(gid, "TimeUnits", "Seconds");

  //...Create the properties group
  this->h5_createPropertiesGroup(gid, isVector);

  //...Create variable space
  hid_t space_tm = H5Screate_simple(1, dims_tm, dims_tm_max);
  hid_t space_mx = H5Screate_simple(1, dims_tm, dims_tm_max);
  hid_t space_mn = H5Screate_simple(1, dims_tm, dims_tm_max);
  H5Pset_chunk(props_tm, 1, dims_tm_chunk);
  H5Pset_chunk(props_mx, 1, dims_tm_chunk);
  H5Pset_chunk(props_mn, 1, dims_tm_chunk);

  hid_t space_val = 0;
  hid_t space_ac = 0;
  if (isVector) {
    space_val = H5Screate_simple(3, dims_val, dims_val_max);
    H5Pset_chunk(props_vl, 3, dims_val_chunk);
  } else {
    space_val = H5Screate_simple(2, dims_val, dims_val_max);
    space_ac = H5Screate_simple(2, dims_val, dims_val_max);
    H5Pset_chunk(props_vl, 2, dims_val_chunk);
    H5Pset_chunk(props_ac, 2, dims_val_chunk);
  }

  //...Compress storage
  H5Pset_deflate(props_tm, 2);
  H5Pset_deflate(props_vl, 2);
  H5Pset_deflate(props_mx, 2);
  H5Pset_deflate(props_mn, 2);
  if (!isVector) H5Pset_deflate(props_ac, 2);

  //...Set fillvalue
  float mxfill = Adcirc::Output::defaultOutputValue();
  H5Pset_fill_value(props_mx, H5T_IEEE_F32LE, &mxfill);
  H5Pset_fill_value(props_mn, H5T_IEEE_F32LE, &mxfill);
  if (isVector) {
    float fill = 0.0f;
    H5Pset_fill_value(props_vl, H5T_IEEE_F32LE, &fill);
  } else {
    float fill = Adcirc::Output::defaultOutputValue();
    long long fill_ac = 1;
    H5Pset_fill_value(props_vl, H5T_IEEE_F32LE, &fill);
    H5Pset_fill_value(props_ac, H5T_STD_U8LE, &fill_ac);
  }

  //...Create datasets
  hid_t did_ac = 0;
  if (!isVector)
    did_ac = H5Dcreate2(gid, "Active", H5T_STD_U8LE, space_ac, H5P_DEFAULT,
                        props_ac, H5P_DEFAULT);

  hid_t did_mx = H5Dcreate2(gid, "Maxs", H5T_IEEE_F32LE, space_mx, H5P_DEFAULT,
                            props_mx, H5P_DEFAULT);
  hid_t did_mn = H5Dcreate2(gid, "Mins", H5T_IEEE_F32LE, space_mn, H5P_DEFAULT,
                            props_mn, H5P_DEFAULT);
  hid_t did_tm = H5Dcreate2(gid, "Times", H5T_IEEE_F64LE, space_tm, H5P_DEFAULT,
                            props_tm, H5P_DEFAULT);
  hid_t did_val = H5Dcreate2(gid, "Values", H5T_IEEE_F32LE, space_val,
                             H5P_DEFAULT, props_vl, H5P_DEFAULT);

  //...Close
  if (!isVector) {
    H5Sclose(space_ac);
    H5Dclose(did_ac);
  }

  H5Sclose(space_tm);
  H5Sclose(space_val);
  H5Sclose(space_mx);
  H5Sclose(space_mn);
  H5Dclose(did_tm);
  H5Dclose(did_val);
  H5Dclose(did_mx);
  H5Dclose(did_mn);
  H5Gclose(gid);

  return;
}

void WriteOutput::h5_defineStringAttribute(int64_t id, const std::string &name,
                                           const std::string &value) {
  hsize_t dims[1] = {1};
  hid_t type = H5Tcopy(H5T_C_S1);
  size_t len = strlen(value.c_str()) + 1;
  herr_t status = H5Tset_size(type, len);
  hid_t ds = H5Screate_simple(1, dims, dims);
  hid_t att = H5Acreate2(id, name.c_str(), type, ds, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Awrite(att, type, value.c_str());
  status = H5Aclose(att);
  status = H5Tclose(type);
  status = H5Sclose(ds);
  return;
}

void WriteOutput::h5_defineIntegerAttribute(int64_t id, const std::string &name,
                                            int value) {
  hsize_t dims[1] = {1};
  hid_t ds = H5Screate_simple(1, dims, dims);
  hid_t att =
      H5Acreate2(id, name.c_str(), H5T_STD_I32LE, ds, H5P_DEFAULT, H5P_DEFAULT);
  herr_t status = H5Awrite(att, H5T_STD_I32LE, &value);
  status = H5Aclose(att);
  status = H5Sclose(ds);
  return;
}

void WriteOutput::h5_defineFloatAttribute(int64_t id, const std::string &name,
                                          float value) {
  hsize_t dims[1] = {1};
  hid_t ds = H5Screate_simple(1, dims, dims);
  hid_t att = H5Acreate2(id, name.c_str(), H5T_IEEE_F32LE, ds, H5P_DEFAULT,
                         H5P_DEFAULT);
  herr_t status = H5Awrite(att, H5T_IEEE_F32LE, &value);
  status = H5Aclose(att);
  status = H5Sclose(ds);
  return;
}

void WriteOutput::h5_defineDoubleAttribute(int64_t id, const std::string &name,
                                           double value) {
  hsize_t dims[1] = {1};
  hid_t ds = H5Screate_simple(1, dims, dims);
  hid_t att = H5Acreate2(id, name.c_str(), H5T_IEEE_F64LE, ds, H5P_DEFAULT,
                         H5P_DEFAULT);
  herr_t status = H5Awrite(att, H5T_IEEE_F64LE, &value);
  status = H5Aclose(att);
  status = H5Sclose(ds);
  return;
}

void WriteOutput::h5_defineFiletype() {
  char filetype[] = "Xmdf";
  float fileversion[1] = {99.99};

  hsize_t dims[1] = {1};
  hid_t ft_type = H5Tcopy(H5T_C_S1);
  size_t ft_len = strlen(filetype) + 1;
  H5Tset_size(ft_type, ft_len);
  hid_t ft_ds = H5Screate_simple(1, dims, dims);
  hid_t ft_did = H5Dcreate2(this->m_h5fid, "File Type", ft_type, ft_ds,
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  hid_t ft_fs = H5Dget_space(ft_did);
  H5Dwrite(ft_did, ft_type, ft_ds, ft_fs, H5P_DEFAULT, filetype);
  H5Sclose(ft_ds);
  H5Dclose(ft_did);

  hid_t fv_ds = H5Screate_simple(1, dims, dims);
  hid_t fv_did = H5Dcreate2(this->m_h5fid, "File Version", H5T_IEEE_F32LE,
                            fv_ds, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  hid_t fv_fs = H5Dget_space(fv_did);
  H5Dwrite(fv_did, H5T_IEEE_F32LE, fv_ds, fv_fs, H5P_DEFAULT, fileversion);
  H5Sclose(fv_ds);
  H5Dclose(fv_did);

  return;
}

void WriteOutput::openFileHdf5() {
  this->m_h5fid = H5Fcreate(this->m_filename.c_str(), H5F_ACC_TRUNC,
                            H5P_DEFAULT, H5P_DEFAULT);
  hid_t gid_dataset = H5Gcreate2(this->m_h5fid, "/Datasets", H5P_DEFAULT,
                                 H5P_DEFAULT, H5P_DEFAULT);

  this->h5_defineStringAttribute(gid_dataset, "Grouptype", "MULTI DATASETS");

  this->h5_createDataset("/Datasets/Depth-averaged Velocity (64)", true);
  this->h5_createDataset("/Datasets/Water Surface Elevation (63)", false);

  this->h5_defineFiletype();

  H5Gclose(gid_dataset);

  return;
}

void WriteOutput::setFilename(const std::string &filename) {
  this->m_filename = filename;
}

std::string WriteOutput::filename() const { return this->m_filename; }

void WriteOutput::writeAsciiNodeRecord(size_t i, const OutputRecord *record) {
  if (this->m_dataContainer->metadata()->dimension() == 1) {
    this->m_fid << Adcirc::Output::Formatting::adcircScalarLineFormat(
        i + 1, record->z(i));
  } else if (this->m_dataContainer->metadata()->dimension() == 2) {
    if (this->m_dataContainer->metadata()->isMax()) {
      this->m_fid << Adcirc::Output::Formatting::adcircScalarLineFormat(
          i + 1, record->z(i));
    } else {
      this->m_fid << Adcirc::Output::Formatting::adcircVectorLineFormat(
          i + 1, record->u(i), record->v(i));
    }
  } else if (this->m_dataContainer->metadata()->dimension() == 3) {
    this->m_fid << Adcirc::Output::Formatting::adcirc3dLineFormat(
        i + 1, record->u(i), record->v(i), record->w(i));
  }
  return;
}

void WriteOutput::writeRecordAsciiFull(const OutputRecord *record) {
  this->m_fid << Adcirc::Output::Formatting::adcircFullFormatRecordHeader(
      record->time(), record->iteration());
  for (size_t i = 0; i < record->numNodes(); ++i) {
    this->writeAsciiNodeRecord(i, record);
  }
  if (this->m_dataContainer->metadata()->isMax() &&
      this->m_dataContainer->metadata()->dimension() == 2) {
    for (size_t i = 0; i < record->numNodes(); ++i) {
      this->m_fid << Adcirc::Output::Formatting::adcircScalarLineFormat(
          i + 1, record->v(i));
    }
  }
  return;
}

void WriteOutput::writeRecordAsciiSparse(const OutputRecord *record) {
  this->m_fid << Adcirc::Output::Formatting::adcircSparseFormatRecordHeader(
      record->time(), record->iteration(), record->numNonDefault(),
      record->defaultValue());
  for (size_t i = 0; i < record->numNodes(); ++i) {
    if (!record->isDefault(i)) {
      this->writeAsciiNodeRecord(i, record);
    }
  }
  if (this->m_dataContainer->metadata()->isMax() &&
      this->m_dataContainer->metadata()->dimension() == 2) {
    for (size_t i = 0; i < record->numNodes(); ++i) {
      if (!record->isDefault(i)) {
        this->m_fid << Adcirc::Output::Formatting::adcircScalarLineFormat(
            i + 1, record->v(i));
      }
    }
  }
  return;
}

void WriteOutput::writeRecordNetCDF(const OutputRecord *record) {
  double t = record->time();
  nc_put_var1(this->m_ncid, this->m_varid_time, &this->m_recordsWritten, &t);

  const size_t start[2] = {this->m_recordsWritten, 0};
  const size_t count[2] = {1, record->numNodes()};

  if (this->m_dataContainer->metadata()->dimension() == 1) {
    std::unique_ptr<double[]> z(new double[record->numNodes()]);
    for (size_t i = 0; i < record->numNodes(); ++i) {
      z.get()[i] = record->z(i);
    }
    nc_put_vara(this->m_ncid, this->m_varid[0], start, count, z.get());
  } else if (this->m_dataContainer->metadata()->dimension() == 2) {
    std::unique_ptr<double[]> u(new double[record->numNodes()]);
    std::unique_ptr<double[]> v(new double[record->numNodes()]);
    for (size_t i = 0; i < record->numNodes(); ++i) {
      u.get()[i] = record->u(i);
      v.get()[i] = record->v(i);
    }
    nc_put_vara(this->m_ncid, this->m_varid[0], start, count, u.get());
    nc_put_vara(this->m_ncid, this->m_varid[1], start, count, v.get());
  } else if (this->m_dataContainer->metadata()->dimension() == 3) {
    std::unique_ptr<double[]> u(new double[record->numNodes()]);
    std::unique_ptr<double[]> v(new double[record->numNodes()]);
    std::unique_ptr<double[]> w(new double[record->numNodes()]);
    for (size_t i = 0; i < record->numNodes(); ++i) {
      u.get()[i] = record->u(i);
      v.get()[i] = record->v(i);
      w.get()[i] = record->w(i);
    }
    nc_put_vara(this->m_ncid, this->m_varid[0], start, count, u.get());
    nc_put_vara(this->m_ncid, this->m_varid[1], start, count, v.get());
    nc_put_vara(this->m_ncid, this->m_varid[2], start, count, w.get());
  }
  return;
}

void WriteOutput::h5_appendRecord(const std::string &name,
                                  const Adcirc::Output::OutputRecord *record,
                                  bool isVector) {
  //...Names
  const std::string name_time = name + "/Times";
  const std::string name_values = name + "/Values";
  const std::string name_max = name + "/Maxs";
  const std::string name_min = name + "/Mins";
  const std::string name_ac = name + "/Active";

  //...Get the dataset ids
  hid_t did_ac = 0;
  if (!isVector) did_ac = H5Dopen2(this->m_h5fid, name_ac.c_str(), H5P_DEFAULT);
  hid_t did_tm = H5Dopen2(this->m_h5fid, name_time.c_str(), H5P_DEFAULT);
  hid_t did_val = H5Dopen2(this->m_h5fid, name_values.c_str(), H5P_DEFAULT);
  hid_t did_mx = H5Dopen2(this->m_h5fid, name_max.c_str(), H5P_DEFAULT);
  hid_t did_mn = H5Dopen2(this->m_h5fid, name_min.c_str(), H5P_DEFAULT);

  //...Get data spaces
  hid_t sid_ac = 0;
  if (!isVector) sid_ac = H5Dget_space(did_ac);
  hid_t sid_tm = H5Dget_space(did_tm);
  hid_t sid_vl = H5Dget_space(did_val);
  hid_t sid_mx = H5Dget_space(did_mx);
  hid_t sid_mn = H5Dget_space(did_mn);

  //..Get extents
  hsize_t dimtm[1], dimval[3], dimmx[1], dimmn[1], dimac[2];
  if (!isVector) H5Sget_simple_extent_dims(sid_ac, dimac, NULL);
  H5Sget_simple_extent_dims(sid_tm, dimtm, NULL);
  H5Sget_simple_extent_dims(sid_vl, dimval, NULL);
  H5Sget_simple_extent_dims(sid_mx, dimmx, NULL);
  H5Sget_simple_extent_dims(sid_mn, dimmn, NULL);

  //...Create new extents
  hsize_t offset_tm[1] = {dimtm[0]};
  hsize_t offset_val[3] = {dimval[0], 0, 0};
  hsize_t offset_mx[1] = {dimmx[0]};
  hsize_t offset_mn[1] = {dimmn[0]};
  hsize_t offset_ac[2];
  hsize_t dimac_new[2];
  if (!isVector) {
    offset_ac[0] = dimac[0];
    offset_ac[1] = 0;
    dimac_new[0] = 1;
    dimac_new[1] = record->numNodes();
  }
  hsize_t dimtm_new[1] = {1};
  hsize_t dimval_new_sca[2] = {1, record->numNodes()};
  hsize_t dimval_new_vec[3] = {1, record->numNodes(), 2};
  hsize_t dimmx_new[1] = {1};
  hsize_t dimmn_new[1] = {1};
  dimtm[0]++;
  dimval[0]++;
  dimmx[0]++;
  dimmn[0]++;
  if (!isVector) dimac[0]++;

  //...Extend the datasets
  H5Dset_extent(did_tm, dimtm);
  H5Dset_extent(did_val, dimval);
  H5Dset_extent(did_mx, dimmx);
  H5Dset_extent(did_mn, dimmn);
  if (!isVector) H5Dset_extent(did_ac, dimac);

  //...Get a new hyperslab
  hid_t fs_tm = H5Dget_space(did_tm);
  hid_t fs_val = H5Dget_space(did_val);
  hid_t fs_mx = H5Dget_space(did_mx);
  hid_t fs_mn = H5Dget_space(did_mn);
  hid_t fs_ac = 0;

  H5Sselect_hyperslab(fs_tm, H5S_SELECT_SET, offset_tm, NULL, dimtm_new, NULL);
  H5Sselect_hyperslab(fs_mx, H5S_SELECT_SET, offset_mx, NULL, dimmx_new, NULL);
  H5Sselect_hyperslab(fs_mn, H5S_SELECT_SET, offset_mn, NULL, dimmn_new, NULL);
  if (isVector) {
    H5Sselect_hyperslab(fs_val, H5S_SELECT_SET, offset_val, NULL,
                        dimval_new_vec, NULL);
  } else {
    H5Sselect_hyperslab(fs_val, H5S_SELECT_SET, offset_val, NULL,
                        dimval_new_sca, NULL);
    fs_ac = H5Dget_space(did_ac);
    H5Sselect_hyperslab(fs_ac, H5S_SELECT_SET, offset_ac, NULL, dimac_new,
                        NULL);
  }

  //...Define the memory space
  hid_t ms_tm = H5Screate_simple(1, dimtm_new, NULL);
  hid_t ms_mx = H5Screate_simple(1, dimmx_new, NULL);
  hid_t ms_mn = H5Screate_simple(1, dimmn_new, NULL);
  hid_t ms_val = 0;
  hid_t ms_ac = 0;
  if (isVector) {
    ms_val = H5Screate_simple(3, dimval_new_vec, NULL);
  } else {
    ms_val = H5Screate_simple(2, dimval_new_sca, NULL);
    ms_ac = H5Screate_simple(2, dimac_new, NULL);
  }

  //...Write the new data
  double time[1] = {record->time()};
  H5Dwrite(did_tm, H5T_IEEE_F64LE, ms_tm, fs_tm, H5P_DEFAULT, time);

  float mx[1] = {-std::numeric_limits<float>::max()};
  float mn[1] = {std::numeric_limits<float>::max()};

  if (isVector) {
    size_t idx = 0;
    std::unique_ptr<float[]> uv(new float[record->numNodes() * 2]);
    for (size_t i = 0; i < record->numNodes(); ++i) {
      float u = static_cast<float>(record->u(i));
      float v = static_cast<float>(record->v(i));
      if (u < -9990) u = 0.0f;
      if (v < -9990) v = 0.0f;
      float m = 0.0;
      if (u != 0.0f || v != 0.0f) m = std::sqrt(u * u + v * v);
      if (m > mx[0]) mx[0] = m;
      if (m < mn[0]) mn[0] = m;
      uv.get()[idx] = u;
      idx++;
      uv.get()[idx] = v;
      idx++;
    }
    H5Dwrite(did_val, H5T_IEEE_F32LE, ms_val, fs_val, H5P_DEFAULT, uv.get());
  } else {
    std::unique_ptr<float[]> wse(new float[record->numNodes()]);
    std::unique_ptr<unsigned char[]> active(
        new unsigned char[record->numNodes()]);
    for (size_t i = 0; i < record->numNodes(); ++i) {
      wse.get()[i] = static_cast<float>(record->z(i));
      if (wse.get()[i] > mx[0]) mx[0] = wse.get()[i];
      if (wse.get()[i] < mn[0]) mn[0] = wse.get()[i];
      if (wse.get()[i] > -9990) {
        active.get()[i] = 1;
      } else {
        active.get()[i] = 0;
      }
    }
    H5Dwrite(did_val, H5T_IEEE_F32LE, ms_val, fs_val, H5P_DEFAULT, wse.get());
    H5Dwrite(did_ac, H5T_STD_U8LE, ms_ac, fs_ac, H5P_DEFAULT, active.get());
  }

  H5Dwrite(did_mx, H5T_IEEE_F32LE, ms_mx, fs_mx, H5P_DEFAULT, mx);
  H5Dwrite(did_mn, H5T_IEEE_F32LE, ms_mn, fs_mn, H5P_DEFAULT, mn);

  //...Close datasets
  H5Sclose(ms_tm);
  H5Sclose(ms_val);
  H5Sclose(ms_mx);
  H5Sclose(ms_mn);

  H5Sclose(fs_tm);
  H5Sclose(fs_val);
  H5Sclose(fs_mx);
  H5Sclose(fs_mn);

  H5Sclose(sid_tm);
  H5Sclose(sid_vl);
  H5Sclose(sid_mx);
  H5Sclose(sid_mn);

  H5Dclose(did_tm);
  H5Dclose(did_val);
  H5Dclose(did_mx);
  H5Dclose(did_mn);

  return;
}

void WriteOutput::writeRecordHdf5(
    const Adcirc::Output::OutputRecord *recordElevation,
    const Adcirc::Output::OutputRecord *recordVelocity) {
  this->h5_appendRecord("/Datasets/Water Surface Elevation (63)",
                        recordElevation, false);
  if (recordVelocity != nullptr) {
    this->h5_appendRecord("/Datasets/Depth-averaged Velocity (64)",
                          recordVelocity, true);
  }
  return;
}
