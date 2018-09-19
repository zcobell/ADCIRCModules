
#include "adcirc.h"
#include "griddata.h"
#include <iostream>
#include "multithreading.h"

int main(){
    Adcirc::Geometry::Mesh *m = new Adcirc::Geometry::Mesh("/cygdrive/c/Users/zcobell/Documents/Codes/ADCModules/testing/test_files/ms-riv.grd");
    m->read();
    m->defineProjection(4326,true);
    m->reproject(26915);

    //Multithreading::disable();

    Griddata *g = new Griddata(m,"landcover_la_gap_usgs_1998_nad83.tif");
    g->setShowProgressBar(true);
    g->readLookupTable("/cygdrive/c/Users/zcobell/Documents/Codes/ADCModules/build/LA-GAP_20100906.table");
    g->setEpsg(26915);
    g->setRasterInMemory(true);
    std::vector<std::vector<double>> r = g->computeDirectionalWindReduction(true);

    for(size_t i=0;i<12;++i){
        for(size_t j=0;j<m->numNodes();++j){
            m->node(j)->setZ(r[j][i]);
        }
        m->write("griddata_"+to_string(i)+".grd");
    }

    delete m;
    delete g;

}
