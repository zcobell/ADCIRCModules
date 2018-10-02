#include "adcirc.h"
#include <iostream>

int main(){
    Adcirc::Geometry::Mesh *m = new Adcirc::Geometry::Mesh("test_files/ms-riv.grd");
    m->read();
    m->defineProjection(4326,true);
    m->reproject(26915);

    Multithreading::disable();

    Griddata *g = new Griddata(m,"test_files/lulc_samplelulcraster.tif");
    g->setShowProgressBar(true);
    g->readLookupTable("test_files/sample_lookup.table");
    g->setEpsg(26915);
    g->setRasterInMemory(true);
    for(size_t i=0;i<m->numNodes();++i){
        g->setInterpolationFlag(i,i%6);
    }
    std::vector<double> r = g->computeValuesFromRaster(true);

    delete m;
    delete g;

}
