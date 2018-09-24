
#include "adcirc.h"
#include "griddata.h"
#include <iostream>
#include "multithreading.h"

int main(){
    Adcirc::Geometry::Mesh *m = new Adcirc::Geometry::Mesh("flagged_cpra2017_v15a_S01Y10G300_chk.grd");
    m->read();
    m->defineProjection(4326,true);
    m->reproject(26915);

    Multithreading::disable();

    Griddata *g = new Griddata(m,"MPM2017_S03_G304_C000_U00_V00_SLA_O_25_25_W_dem30.img");
    g->setShowProgressBar(true);
    //g->readLookupTable("/cygdrive/c/Users/zcobell/Documents/Codes/ADCModules/build/LA-GAP_20100906.table");
    g->setEpsg(26915);
    g->setRasterInMemory(true);
    std::vector<double> r = g->computeValuesFromRaster();

    m->setZ(r);
    m->write("output.grd");

    delete m;
    delete g;

}
