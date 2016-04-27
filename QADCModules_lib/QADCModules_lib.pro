#-------------------------------GPL-------------------------------------#
#
# QADCModules - A library for working with ADCIRC models
# Copyright (C) 2016  Zach Cobell
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#-----------------------------------------------------------------------#

QT       += gui

TARGET = QADCModules 
TEMPLATE = lib
VERSION = 0.1.0.0

win32 {
    #...Set extension for a windows dll (avoids tracking version numbering in filename)
    TARGET_EXT = .dll

    #...Include the netCDF header
    INCLUDEPATH += $$PWD/../thirdparty/netcdf/include

    #...Check for Visual C++ Compilers
    *msvc*{
        contains(QT_ARCH, i386){
            message("MSVC-32 Compiler in use")
            LIBS += -L$$PWD/../thirdparty/netcdf/libs_vc32 -lnetcdf -lhdf5 -lzlib -llibcurl_imp
        }else{
            message("MSVC-64 Compiler in use")
            LIBS += -L$$PWD/../thirdparty/netcdf/libs_vc64 -lnetcdf -lhdf5 -lzlib -llibcurl_imp
        }
    }

    #...Check for MinGW-32
    win32-g++ {
        LIBS += -L$$PWD/../thirdparty/netcdf/bin_32 -lnetcdf -lhdf5 -lz -lcurl
    }
}

#...For unix, we assume netCDF exists in the path if it is not provided on the command line
#   via qmake NETCDFHOME=$(NETCDFPATH)
unix:!macx{
    isEmpty(NETCDFHOME){
        LIBS += -lnetcdf
    }else{
        LIBS += -L$$NETCDFHOME/lib -lnetcdf
        INCLUDEPATH += $$NETCDFHOME/include 
    }
}

#...PROJ4 Library
PROJPATH      = $$PWD/../thirdparty/proj4/src

#...Boost Library
BOOSTPATH     = $$PWD/../thirdparty/boost-library

#...KDTREE
KDTREEPATH    = $$PWD/../thirdparty/kdtree

#...Shapelib Library
SHAPELIBPATH  = $$PWD/../thirdparty/shapelib

#...netCDF Library
NETCDFPATH    = $$PWD/../thirdparty/netcdf

#...PROJ Sources
SOURCES_PROJ4 = \
    $$PROJPATH/pj_list.h $$PROJPATH/PJ_aeqd.c $$PROJPATH/PJ_gnom.c $$PROJPATH/PJ_laea.c $$PROJPATH/PJ_mod_ster.c \
    $$PROJPATH/PJ_nsper.c $$PROJPATH/PJ_nzmg.c $$PROJPATH/PJ_ortho.c $$PROJPATH/PJ_stere.c $$PROJPATH/PJ_sterea.c \
    $$PROJPATH/PJ_aea.c $$PROJPATH/PJ_bipc.c $$PROJPATH/PJ_bonne.c $$PROJPATH/PJ_eqdc.c $$PROJPATH/PJ_isea.c \
    $$PROJPATH/PJ_imw_p.c $$PROJPATH/PJ_krovak.c $$PROJPATH/PJ_lcc.c $$PROJPATH/PJ_poly.c $$PROJPATH/PJ_rpoly.c \
    $$PROJPATH/PJ_sconics.c $$PROJPATH/proj_rouss.c $$PROJPATH/PJ_cass.c $$PROJPATH/PJ_cc.c $$PROJPATH/PJ_cea.c \
    $$PROJPATH/PJ_eqc.c $$PROJPATH/PJ_gall.c $$PROJPATH/PJ_labrd.c $$PROJPATH/PJ_lsat.c $$PROJPATH/PJ_merc.c \
    $$PROJPATH/PJ_mill.c $$PROJPATH/PJ_ocea.c $$PROJPATH/PJ_omerc.c $$PROJPATH/PJ_somerc.c $$PROJPATH/PJ_tcc.c \
    $$PROJPATH/PJ_tcea.c $$PROJPATH/PJ_tmerc.c $$PROJPATH/PJ_airy.c $$PROJPATH/PJ_aitoff.c $$PROJPATH/PJ_august.c \
    $$PROJPATH/PJ_bacon.c $$PROJPATH/PJ_chamb.c $$PROJPATH/PJ_hammer.c $$PROJPATH/PJ_lagrng.c $$PROJPATH/PJ_larr.c \
    $$PROJPATH/PJ_lask.c $$PROJPATH/PJ_nocol.c $$PROJPATH/PJ_ob_tran.c $$PROJPATH/PJ_oea.c $$PROJPATH/PJ_tpeqd.c \
    $$PROJPATH/PJ_vandg.c $$PROJPATH/PJ_vandg2.c $$PROJPATH/PJ_vandg4.c $$PROJPATH/PJ_wag7.c $$PROJPATH/PJ_lcca.c \
    $$PROJPATH/PJ_geos.c $$PROJPATH/proj_etmerc.c $$PROJPATH/PJ_boggs.c $$PROJPATH/PJ_collg.c $$PROJPATH/PJ_crast.c \
    $$PROJPATH/PJ_denoy.c $$PROJPATH/PJ_eck1.c $$PROJPATH/PJ_eck2.c $$PROJPATH/PJ_eck3.c $$PROJPATH/PJ_eck4.c \
    $$PROJPATH/PJ_eck5.c $$PROJPATH/PJ_fahey.c $$PROJPATH/PJ_fouc_s.c $$PROJPATH/PJ_gins8.c $$PROJPATH/PJ_gstmerc.c \
    $$PROJPATH/PJ_gn_sinu.c $$PROJPATH/PJ_goode.c $$PROJPATH/PJ_igh.c $$PROJPATH/PJ_hatano.c $$PROJPATH/PJ_loxim.c \
    $$PROJPATH/PJ_mbt_fps.c $$PROJPATH/PJ_mbtfpp.c $$PROJPATH/PJ_mbtfpq.c $$PROJPATH/PJ_moll.c $$PROJPATH/PJ_nell.c \
    $$PROJPATH/PJ_nell_h.c $$PROJPATH/PJ_putp2.c $$PROJPATH/PJ_putp3.c $$PROJPATH/PJ_putp4p.c $$PROJPATH/PJ_putp5.c \
    $$PROJPATH/PJ_putp6.c $$PROJPATH/PJ_qsc.c $$PROJPATH/PJ_robin.c $$PROJPATH/PJ_sts.c $$PROJPATH/PJ_urm5.c \
    $$PROJPATH/PJ_urmfps.c $$PROJPATH/PJ_wag2.c $$PROJPATH/PJ_wag3.c $$PROJPATH/PJ_wink1.c $$PROJPATH/PJ_wink2.c \
    $$PROJPATH/pj_latlong.c $$PROJPATH/pj_geocent.c $$PROJPATH/aasincos.c $$PROJPATH/adjlon.c $$PROJPATH/bch2bps.c \
    $$PROJPATH/bchgen.c $$PROJPATH/biveval.c $$PROJPATH/dmstor.c $$PROJPATH/mk_cheby.c $$PROJPATH/pj_auth.c \
    $$PROJPATH/pj_deriv.c $$PROJPATH/pj_ell_set.c $$PROJPATH/pj_ellps.c $$PROJPATH/pj_errno.c $$PROJPATH/pj_factors.c \
    $$PROJPATH/pj_fwd.c $$PROJPATH/pj_init.c $$PROJPATH/pj_inv.c $$PROJPATH/pj_list.c $$PROJPATH/pj_malloc.c \
    $$PROJPATH/pj_mlfn.c $$PROJPATH/pj_msfn.c $$PROJPATH/proj_mdist.c $$PROJPATH/pj_open_lib.c $$PROJPATH/pj_param.c \
    $$PROJPATH/pj_phi2.c $$PROJPATH/pj_pr_list.c $$PROJPATH/pj_qsfn.c $$PROJPATH/pj_strerrno.c $$PROJPATH/pj_tsfn.c \
    $$PROJPATH/pj_units.c $$PROJPATH/pj_ctx.c $$PROJPATH/pj_log.c $$PROJPATH/pj_zpoly1.c $$PROJPATH/rtodms.c \
    $$PROJPATH/vector1.c $$PROJPATH/pj_release.c $$PROJPATH/pj_gauss.c $$PROJPATH/PJ_healpix.c $$PROJPATH/PJ_natearth.c \
    $$PROJPATH/PJ_calcofi.c $$PROJPATH/pj_fileapi.c $$PROJPATH/pj_gc_reader.c $$PROJPATH/pj_gridcatalog.c \
    $$PROJPATH/nad_cvt.c $$PROJPATH/nad_init.c $$PROJPATH/nad_intr.c $$PROJPATH/emess.c $$PROJPATH/emess.h \
    $$PROJPATH/pj_apply_gridshift.c $$PROJPATH/pj_datums.c $$PROJPATH/pj_datum_set.c $$PROJPATH/pj_transform.c \
    $$PROJPATH/geocent.c $$PROJPATH/geocent.h $$PROJPATH/pj_utils.c $$PROJPATH/pj_gridinfo.c $$PROJPATH/pj_gridlist.c \
    $$PROJPATH/jniproj.c $$PROJPATH/pj_mutex.c $$PROJPATH/pj_initcache.c $$PROJPATH/pj_apply_vgridshift.c $$PROJPATH/geodesic.c \
    $$PROJPATH/pj_strtod.c

#...KDTREE2 Sources
SOURCES_KDTREE2 = $$KDTREEPATH/kdtree2.cpp

#...Shapelib Sources
SOURCES_SHAPELIB = $$SHAPELIBPATH/shpopen.c $$SHAPELIBPATH/dbfopen.c $$SHAPELIBPATH/safileio.c $$SHAPELIBPATH/shptree.c

INCLUDEPATH += include $$PROJPATH $$BOOSTPATH $$KDTREEPATH $$SHAPELIBPATH

DEFINES += QADCMODULES_LIBRARY

SOURCES += \
    $$SOURCES_PROJ4 $$SOURCES_KDTREE2 $$SOURCES_SHAPELIB \
    QADCModules.cpp \
    adcirc_node.cpp \
    adcirc_element.cpp \
    adcirc_boundary.cpp \
    adcirc_mesh.cpp \
    adcirc_nodalparameter.cpp \
    adcirc_nodalattributes.cpp \
    proj4.cpp \
    adcirc_node_table.cpp \
    qkdtree2.cpp \
    QADCModules_errors.cpp \
    adcirc_output_record.cpp \
    adcirc_global_output.cpp

HEADERS +=\
    QADCModules_global.h \
    QADCModules.h \
    adcirc_node.h \
    adcirc_element.h \
    adcirc_boundary.h \
    adcirc_mesh.h \
    adcirc_nodalparameter.h \
    adcirc_nodalattributes.h \
    proj4.h \
    adcirc_node_table.h \
    $$KDTREEPATH/kdtree2.hpp \
    qkdtree2.h \
    QADCModules_errors.h \
    adcirc_global_output.h \
    adcirc_output_record.h


win32{
    isEmpty(PREFIX) {
        PREFIX = $$OUT_PWD
    }
    target.path = $$PREFIX/bin
    header_files.files = $$HEADERS $$BOOSTPATH/boost $$NETCDFPATH/include/netcdf.h
    header_files.path = $$PREFIX/include
    INSTALLS += header_files target

}
unix{
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    target.path = $$PREFIX/lib
    header_files.files = $$HEADERS $$BOOSTPATH/boost $$NETCDFPATH/include/netcdf.h
    header_files.path = $$PREFIX/include
    INSTALLS += header_files target

}



DISTFILES +=

RESOURCES += \
    QADCModules_resources.qrc
