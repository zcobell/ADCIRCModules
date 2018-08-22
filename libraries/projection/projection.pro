##------------------------------GPL---------------------------------------##
## This file is part of ADCIRCModules.
##
## (c) 2015-2018 Zachary Cobell
##
## ADCIRCModules is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## ADCIRCModules is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with ADCIRCModules.  If not, see <http://www.gnu.org/licenses/>.
##------------------------------------------------------------------------##
TARGET = projection
TEMPLATE = lib
CONFIG += static
CONFIG -= qt
CONFIG += c++11

#...PROJ4 Library
PROJPATH      = $$PWD/../../thirdparty/proj4/src

#...PROJ Sources
SOURCES_PROJ4 =   $$PROJPATH/nad_init.c                \
                  $$PROJPATH/PJ_aea.c                  \
                  $$PROJPATH/PJ_aeqd.c                 \
                  $$PROJPATH/PJ_airy.c                 \
                  $$PROJPATH/PJ_aitoff.c               \
                  $$PROJPATH/PJ_august.c               \
                  $$PROJPATH/PJ_axisswap.c             \
                  $$PROJPATH/PJ_bacon.c                \
                  $$PROJPATH/PJ_bipc.c                 \
                  $$PROJPATH/PJ_boggs.c                \
                  $$PROJPATH/PJ_bonne.c                \
                  $$PROJPATH/PJ_calcofi.c              \
                  $$PROJPATH/PJ_cart.c                 \
                  $$PROJPATH/PJ_cass.c                 \
                  $$PROJPATH/PJ_cc.c                   \
                  $$PROJPATH/PJ_ccon.c                 \
                  $$PROJPATH/PJ_cea.c                  \
                  $$PROJPATH/PJ_chamb.c                \
                  $$PROJPATH/PJ_collg.c                \
                  $$PROJPATH/PJ_comill.c               \
                  $$PROJPATH/PJ_crast.c                \
                  $$PROJPATH/PJ_deformation.c          \
                  $$PROJPATH/PJ_denoy.c                \
                  $$PROJPATH/PJ_eck1.c                 \
                  $$PROJPATH/PJ_eck2.c                 \
                  $$PROJPATH/PJ_eck3.c                 \
                  $$PROJPATH/PJ_eck4.c                 \
                  $$PROJPATH/PJ_eck5.c                 \
                  $$PROJPATH/PJ_eqc.c                  \
                  $$PROJPATH/PJ_eqdc.c                 \
                  $$PROJPATH/PJ_fahey.c                \
                  $$PROJPATH/PJ_fouc_s.c               \
                  $$PROJPATH/PJ_gall.c                 \
                  $$PROJPATH/PJ_geoc.c                 \
                  $$PROJPATH/PJ_geos.c                 \
                  $$PROJPATH/PJ_gins8.c                \
                  $$PROJPATH/PJ_gnom.c                 \
                  $$PROJPATH/PJ_gn_sinu.c              \
                  $$PROJPATH/PJ_goode.c                \
                  $$PROJPATH/PJ_gstmerc.c              \
                  $$PROJPATH/PJ_hammer.c               \
                  $$PROJPATH/PJ_hatano.c               \
                  $$PROJPATH/PJ_helmert.c              \
                  $$PROJPATH/PJ_hgridshift.c           \
                  $$PROJPATH/PJ_horner.c               \
                  $$PROJPATH/PJ_igh.c                  \
                  $$PROJPATH/PJ_isea.c                 \
                  $$PROJPATH/PJ_imw_p.c                \
                  $$PROJPATH/PJ_krovak.c               \
                  $$PROJPATH/PJ_labrd.c                \
                  $$PROJPATH/PJ_laea.c                 \
                  $$PROJPATH/PJ_lagrng.c               \
                  $$PROJPATH/PJ_larr.c                 \
                  $$PROJPATH/PJ_lask.c                 \
                  $$PROJPATH/PJ_latlong.c              \
                  $$PROJPATH/PJ_lcca.c                 \
                  $$PROJPATH/PJ_lcc.c                  \
                  $$PROJPATH/PJ_loxim.c                \
                  $$PROJPATH/PJ_lsat.c                 \
                  $$PROJPATH/PJ_misrsom.c              \
                  $$PROJPATH/PJ_mbt_fps.c              \
                  $$PROJPATH/PJ_mbtfpp.c               \
                  $$PROJPATH/PJ_mbtfpq.c               \
                  $$PROJPATH/PJ_merc.c                 \
                  $$PROJPATH/PJ_mill.c                 \
                  $$PROJPATH/PJ_mod_ster.c             \
                  $$PROJPATH/PJ_moll.c                 \
                  $$PROJPATH/PJ_molodensky.c           \
                  $$PROJPATH/PJ_natearth.c             \
                  $$PROJPATH/PJ_natearth2.c            \
                  $$PROJPATH/PJ_nell.c                 \
                  $$PROJPATH/PJ_nell_h.c               \
                  $$PROJPATH/PJ_nocol.c                \
                  $$PROJPATH/PJ_nsper.c                \
                  $$PROJPATH/PJ_nzmg.c                 \
                  $$PROJPATH/PJ_ob_tran.c              \
                  $$PROJPATH/PJ_ocea.c                 \
                  $$PROJPATH/PJ_oea.c                  \
                  $$PROJPATH/PJ_omerc.c                \
                  $$PROJPATH/PJ_ortho.c                \
                  $$PROJPATH/PJ_patterson.c            \
                  $$PROJPATH/PJ_pipeline.c             \
                  $$PROJPATH/PJ_poly.c                 \
                  $$PROJPATH/PJ_putp2.c                \
                  $$PROJPATH/PJ_putp3.c                \
                  $$PROJPATH/PJ_putp4p.c               \
                  $$PROJPATH/PJ_putp5.c                \
                  $$PROJPATH/PJ_putp6.c                \
                  $$PROJPATH/PJ_qsc.c                  \
                  $$PROJPATH/PJ_robin.c                \
                  $$PROJPATH/PJ_rpoly.c                \
                  $$PROJPATH/PJ_sch.c                  \
                  $$PROJPATH/PJ_sconics.c              \
                  $$PROJPATH/PJ_somerc.c               \
                  $$PROJPATH/PJ_sterea.c               \
                  $$PROJPATH/PJ_stere.c                \
                  $$PROJPATH/PJ_sts.c                  \
                  $$PROJPATH/PJ_tcc.c                  \
                  $$PROJPATH/PJ_tcea.c                 \
                  $$PROJPATH/PJ_times.c                \
                  $$PROJPATH/PJ_tmerc.c                \
                  $$PROJPATH/PJ_tpeqd.c                \
                  $$PROJPATH/PJ_unitconvert.c          \
                  $$PROJPATH/PJ_urm5.c                 \
                  $$PROJPATH/PJ_urmfps.c               \
                  $$PROJPATH/PJ_vandg.c                \
                  $$PROJPATH/PJ_vandg2.c               \
                  $$PROJPATH/PJ_vandg4.c               \
                  $$PROJPATH/PJ_vgridshift.c           \
                  $$PROJPATH/PJ_wag2.c                 \
                  $$PROJPATH/PJ_wag3.c                 \
                  $$PROJPATH/PJ_wag7.c                 \
                  $$PROJPATH/PJ_wink1.c                \
                  $$PROJPATH/PJ_wink2.c                \
                  $$PROJPATH/proj_etmerc.c             \
                  $$PROJPATH/aasincos.c                \
                  $$PROJPATH/adjlon.c                  \
                  $$PROJPATH/bch2bps.c                 \
                  $$PROJPATH/bchgen.c                  \
                  $$PROJPATH/biveval.c                 \
                  $$PROJPATH/dmstor.c                  \
                  $$PROJPATH/emess.c                   \
                  $$PROJPATH/geocent.c                 \
                  $$PROJPATH/geodesic.c                \
                  $$PROJPATH/mk_cheby.c                \
                  $$PROJPATH/nad_cvt.c                 \
                  $$PROJPATH/nad_intr.c                \
                  $$PROJPATH/pj_apply_gridshift.c      \
                  $$PROJPATH/pj_apply_vgridshift.c     \
                  $$PROJPATH/pj_auth.c                 \
                  $$PROJPATH/pj_ctx.c                  \
                  $$PROJPATH/pj_fileapi.c              \
                  $$PROJPATH/pj_datum_set.c            \
                  $$PROJPATH/pj_datums.c               \
                  $$PROJPATH/pj_deriv.c                \
                  $$PROJPATH/pj_ell_set.c              \
                  $$PROJPATH/pj_ellps.c                \
                  $$PROJPATH/pj_errno.c                \
                  $$PROJPATH/pj_factors.c              \
                  $$PROJPATH/pj_fwd.c                  \
                  $$PROJPATH/pj_gauss.c                \
                  $$PROJPATH/pj_gc_reader.c            \
                  $$PROJPATH/pj_geocent.c              \
                  $$PROJPATH/pj_gridcatalog.c          \
                  $$PROJPATH/pj_gridinfo.c             \
                  $$PROJPATH/pj_gridlist.c             \
                  $$PROJPATH/PJ_healpix.c              \
                  $$PROJPATH/pj_init.c                 \
                  $$PROJPATH/pj_initcache.c            \
                  $$PROJPATH/pj_inv.c                  \
                  $$PROJPATH/pj_list.c                 \
                  $$PROJPATH/pj_log.c                  \
                  $$PROJPATH/pj_malloc.c               \
                  $$PROJPATH/pj_math.c                 \
                  $$PROJPATH/pj_mlfn.c                 \
                  $$PROJPATH/pj_msfn.c                 \
                  $$PROJPATH/pj_mutex.c                \
                  $$PROJPATH/proj_4D_api.c             \
                  $$PROJPATH/pj_internal.c             \
                  $$PROJPATH/pj_open_lib.c             \
                  $$PROJPATH/pj_param.c                \
                  $$PROJPATH/pj_phi2.c                 \
                  $$PROJPATH/pj_pr_list.c              \
                  $$PROJPATH/pj_qsfn.c                 \
                  $$PROJPATH/pj_release.c              \
                  $$PROJPATH/pj_strerrno.c             \
                  $$PROJPATH/pj_transform.c            \
                  $$PROJPATH/pj_tsfn.c                 \
                  $$PROJPATH/pj_units.c                \
                  $$PROJPATH/pj_utils.c                \
                  $$PROJPATH/pj_zpoly1.c               \
                  $$PROJPATH/proj_mdist.c              \
                  $$PROJPATH/proj_rouss.c              \
                  $$PROJPATH/rtodms.c                  \
                  $$PROJPATH/vector1.c                 \
                  $$PROJPATH/pj_strtod.c

HEADERS += epsg.h \
    projection.h

SOURCES += $$SOURCES_PROJ4 epsg.cpp \
    projection.cpp

INCLUDEPATH += $$PROJPATH $$PWD/../../thirdparty/proj4

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../point/release/ -lpoint
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../point/debug/ -lpoint
else:unix: LIBS += -L$$OUT_PWD/../point/ -lpoint

INCLUDEPATH += $$PWD/../point
DEPENDPATH += $$PWD/../point

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../point/release/libpoint.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../point/debug/libpoint.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../point/release/point.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../point/debug/point.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../point/libpoint.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../constants/release/ -lconstants
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../constants/debug/ -lconstants
else:unix: LIBS += -L$$OUT_PWD/../constants/ -lconstants

INCLUDEPATH += $$PWD/../constants
DEPENDPATH += $$PWD/../constants

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../constants/release/libconstants.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../constants/debug/libconstants.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../constants/release/constants.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../constants/debug/constants.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../constants/libconstants.a
