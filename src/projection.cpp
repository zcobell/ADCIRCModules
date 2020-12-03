//------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2020 Zachary Cobell
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
//------------------------------------------------------------------------//
#include "projection.h"

#include <cmath>
#include <string>

#include "constants.h"
#include "sqlite3.h"

#ifdef USE_INTERNAL_PROJ
#define PROJ_RENAME_SYMBOLS
#endif
#include "proj.h"

#include "adcmod_proj_rename.h"

using namespace Adcirc;

bool Projection::containsEpsg(int epsg) {
  projection_epsg_result result = {false, 0, ""};
  return Projection::queryProjDatabase(epsg, result) == 0;
}

std::string Projection::epsgDescription(int epsg) {
  projection_epsg_result result = {false, 0, ""};
  int ierr = Projection::queryProjDatabase(epsg, result);
  if (ierr == 0) {
    return std::get<2>(result);
  } else {
    return std::string();
  }
}

static int projection_sqlite_callback(void *data, int argc, char **argv,
                                      char **azColName) {
  (void)(azColName);  //...Silence warning, unused
  auto *v = static_cast<Projection::projection_epsg_result *>(data);
  if (argc < 1) {
    std::get<0>(*(v)) = false;
    return 1;
  }
  std::get<0>(*(v)) = true;
  std::get<1>(*(v)) = std::stoi(argv[1]);
  std::get<2>(*(v)) = argv[2];
  return 0;
}

int Projection::queryProjDatabase(int epsg, projection_epsg_result &result) {
  sqlite3 *db;
  sqlite3_open(adcmod_proj_context_get_database_path(PJ_DEFAULT_CTX), &db);
  std::string queryString =
      "select distinct auth_name,code,name from crs_view where auth_name == "
      "'EPSG' and code == " +
      std::to_string(epsg) + ";";
  char *errString;
  sqlite3_exec(db, queryString.c_str(), projection_sqlite_callback, &result,
               &errString);
  sqlite3_close(db);
  if (std::get<0>(result)) return 0;
  return 1;
}

int Projection::transform(int epsgInput, int epsgOutput, double x, double y,
                          double &outx, double &outy, bool &isLatLon) {
  std::vector<double> xv = {x};
  std::vector<double> yv = {y};
  std::vector<double> outxv;
  std::vector<double> outyv;
  if (Projection::transform(epsgInput, epsgOutput, xv, yv, outxv, outyv,
                            isLatLon))
    return 1;
  outx = outxv[0];
  outy = outyv[0];
  return 0;
}

int Projection::transform(int epsgInput, int epsgOutput,
                          const std::vector<double> &x,
                          const std::vector<double> &y,
                          std::vector<double> &outx, std::vector<double> &outy,
                          bool &isLatLon) {
  isLatLon = false;
  if (x.size() != y.size()) return 1;
  if (x.empty()) return 1;

  if (!Projection::containsEpsg(epsgInput)) return 1;
  if (!Projection::containsEpsg(epsgOutput)) return 1;

  std::string p1 = "EPSG:" + std::to_string(epsgInput);
  std::string p2 = "EPSG:" + std::to_string(epsgOutput);
  PJ *pj1 = adcmod_proj_create_crs_to_crs(PJ_DEFAULT_CTX, p1.c_str(),
                                          p2.c_str(), NULL);
  if (pj1 == nullptr) return 1;
  PJ *pj2 = adcmod_proj_normalize_for_visualization(PJ_DEFAULT_CTX, pj1);
  if (pj2 == nullptr) return 1;
  adcmod_proj_destroy(pj1);

  outx.clear();
  outy.clear();
  outx.reserve(x.size());
  outy.reserve(y.size());

  for (size_t i = 0; i < x.size(); ++i) {
    PJ_COORD cin;
    if (adcmod_proj_angular_input(pj2, PJ_INV)) {
      cin.lp.lam = adcmod_proj_torad(x[i]);
      cin.lp.phi = adcmod_proj_torad(y[i]);
    } else {
      cin.xy.x = x[i];
      cin.xy.y = y[i];
    }

    PJ_COORD cout = adcmod_proj_trans(pj2, PJ_FWD, cin);

    if (adcmod_proj_angular_output(pj2, PJ_FWD)) {
      outx.push_back(adcmod_proj_todeg(cout.lp.lam));
      outy.push_back(adcmod_proj_todeg(cout.lp.phi));
      isLatLon = true;
    } else {
      outx.push_back(cout.xy.x);
      outy.push_back(cout.xy.y);
      isLatLon = false;
    }
  }
  adcmod_proj_destroy(pj2);
  return 0;
}

std::string Projection::projVersion() {
  return std::to_string(static_cast<unsigned long long>(PROJ_VERSION_MAJOR)) +
         "." +
         std::to_string(static_cast<unsigned long long>(PROJ_VERSION_MINOR)) +
         "." +
         std::to_string(static_cast<unsigned long long>(PROJ_VERSION_PATCH));
}

int Projection::cpp(double lambda0, double phi0, double xin, double yin,
                    double &xout, double &yout) {
  std::vector<double> vxin = {xin};
  std::vector<double> vyin = {yin};
  std::vector<double> vxout, vyout;
  int ierr = Projection::cpp(lambda0, phi0, vxin, vyin, vxout, vyout);
  if (ierr == 0) {
    xout = vxout[0];
    yout = vyout[0];
  }
  return ierr;
}

int Projection::cpp(double lambda0, double phi0, const std::vector<double> &xin,
                    const std::vector<double> &yin, std::vector<double> &xout,
                    std::vector<double> &yout) {
  if (xin.empty()) return 1;
  if (xin.size() != yin.size()) return 1;

  double slam0 = Adcirc::Constants::toRadians(lambda0);
  double sfea0 = Adcirc::Constants::toRadians(phi0);
  double r = Adcirc::Constants::radiusEarth(phi0);
  xout.clear();
  yout.clear();
  xout.reserve(xin.size());
  yout.reserve(yout.size());
  for (size_t i = 0; i < xin.size(); ++i) {
    xout.push_back(r * (Adcirc::Constants::toRadians(xin[i]) - slam0) *
                   std::cos(sfea0));
    yout.push_back(r * Adcirc::Constants::toRadians(yin[i]));
  }
  return 0;
}

int Projection::inverseCpp(double lambda0, double phi0, const double lambda,
                           const double phi, double &x, double &y) {
  std::vector<double> vlambda = {lambda};
  std::vector<double> vphi = {phi};
  std::vector<double> vx, vy;
  int ierr = Projection::inverseCpp(lambda0, phi0, vlambda, vphi, vx, vy);
  if (ierr == 0) {
    x = vx[0];
    y = vy[0];
  }
  return ierr;
}

int Projection::inverseCpp(double lambda0, double phi0,
                           const std::vector<double> &lambda,
                           const std::vector<double> &phi,
                           std::vector<double> &x, std::vector<double> &y) {
  if (lambda.empty()) return 1;
  if (lambda.size() != phi.size()) return 1;

  x.clear();
  y.clear();
  x.reserve(lambda.size());
  y.reserve(lambda.size());

  double slam0 = Adcirc::Constants::toRadians(lambda0);
  double sfea0 = Adcirc::Constants::toRadians(phi0);
  double r = Adcirc::Constants::radiusEarth(phi0);
  for (size_t i = 0; i < lambda.size(); ++i) {
    x.push_back(Adcirc::Constants::toDegrees(
        slam0 + lambda[i] / (r * std::cos(sfea0))));
    y.push_back(Adcirc::Constants::toDegrees(phi[i] / r));
  }
  return 0;
}

void Projection::setProjDatabaseLocation(const std::string &dblocation) {
  adcmod_proj_context_set_database_path(PJ_DEFAULT_CTX, dblocation.c_str(),
                                        nullptr, nullptr);
}
std::string Projection::projDatabaseLocation() {
  return adcmod_proj_context_get_database_path(PJ_DEFAULT_CTX);
}
