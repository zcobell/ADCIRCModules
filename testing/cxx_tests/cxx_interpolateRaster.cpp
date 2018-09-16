
#include "adcirc.h"
#include "griddata.h"
#include <iostream>

int main(){
    Adcirc::Geometry::Mesh *m = new Adcirc::Geometry::Mesh("/home/zcobell/Development/ADCIRCModules/build/cpra2017_v15a_S01Y10G300_chk.grd");
    m->read();
    m->defineProjection(4326,true);
    m->reproject(26715);

    Griddata *g = new Griddata(m,"/home/zcobell/Development/ADCIRCModules/build/landcover_la_gap_usgs_1998.tif");
    g->setShowProgressBar(true);
    g->readLookupTable("/home/zcobell/Development/ADCIRCModules/build/LA-GAP_20100906.table");
    g->setEpsg(26715);
    g->setRasterInMemory(true);
    std::vector<std::vector<double>> r = g->computeDirectionalWindReduction(true);

    for(size_t i=0;i<12;i++){
        m->setZ(r[i]);
        m->write("griddata_"+to_string(i)+".grd");
    }

    delete m;
    delete g;

}
