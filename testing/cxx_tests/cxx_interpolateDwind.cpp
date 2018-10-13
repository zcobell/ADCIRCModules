#include "adcirc.h"
#include <iostream>

int main(){
    Adcirc::Geometry::Mesh *m = new Adcirc::Geometry::Mesh("test_files/ms-riv.grd");
    m->read();
    m->defineProjection(4326,true);
    m->reproject(26915);


    Griddata *g = new Griddata(m,"test_files/lulc_samplelulcraster.tif");
    //Multithreading::disable();
   
#if 1   
    for(size_t i=0;i<m->numNodes();++i){
        if(i<100){
            g->setInterpolationFlag(i,Griddata::Average);
        } else {
            g->setInterpolationFlag(i,Griddata::NoMethod);
        }
    }
#endif    

    g->setShowProgressBar(true);
    g->readLookupTable("test_files/sample_lookup.table");
    g->setEpsg(26915);
    g->setRasterInMemory(true);
    std::vector<std::vector<double>> r = g->computeDirectionalWindReduction(true);

    delete m;
    delete g;

}
