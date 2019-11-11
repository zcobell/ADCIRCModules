#!/bin/bash

#...Interpolate and write adcirc formatted data
../../build/interpolateAdcircStations --station locations.txt --mesh ../test_files/internal_overflow.grd --global ../test_files/fort.63 --output interp_wse.61
../../build/interpolateAdcircStations --station locations.txt --mesh ../test_files/internal_overflow.grd --global ../test_files/fort.64 --output interp_vel.62
../../build/interpolateAdcircStations --station locations.txt --global ../test_files/fort.63.nc --output interp_wsenc.61
../../build/interpolateAdcircStations --station locations.txt --global ../test_files/fort.64.nc --output interp_velnc.62
../../build/interpolateAdcircStations --station locations.txt --global ../test_files/fort.64.nc --output interp_magnc.61 --magnitude
../../build/interpolateAdcircStations --station locations.txt --global ../test_files/fort.64.nc --output interp_magdirnc.61 --magnitude --positive_direction 180
../../build/interpolateAdcircStations --station locations.txt --global ../test_files/fort.64.nc --output interp_dirnc.61 --direction

#...Interpolate and write imeds formatted data
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --mesh ../test_files/internal_overflow.grd --global ../test_files/fort.63 --output interp_wse.imeds
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --global ../test_files/fort.63.nc --output interp_wsenc.imeds
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --global ../test_files/fort.64.nc --output interp_velnc.imeds
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --global ../test_files/fort.64.nc --output interp_magnc.imeds --magnitude
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --global ../test_files/fort.64.nc --output interp_magdirnc.imeds --magnitude --positive_direction 180
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --global ../test_files/fort.64.nc --output interp_dirnc.imeds --direction

#...Interpolate and write imeds formatted data
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --mesh ../test_files/internal_overflow.grd --global ../test_files/fort.63 --output interp_wse.nc
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --global ../test_files/fort.63.nc --output interp_wsenc.nc
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --global ../test_files/fort.64.nc --output interp_velnc.nc
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --global ../test_files/fort.64.nc --output interp_magnc.nc --magnitude
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --global ../test_files/fort.64.nc --output interp_magdirnc.nc --magnitude --positive_direction 180
../../build/interpolateAdcircStations --station locations.txt --coldstart 20191110000000 --global ../test_files/fort.64.nc --output interp_dirnc.nc --direction

#...Read from imeds station
../../build/interpolateAdcircStations --station interp_wse.imeds --coldstart 20191110000000 --mesh ../test_files/internal_overflow.grd --global ../test_files/fort.63 --output interp_wse_fromimeds.nc

#...Read from netcdf station
../../build/interpolateAdcircStations --station interp_wsenc.nc --coldstart 20191110000000 --mesh ../test_files/internal_overflow.grd --global ../test_files/fort.63 --output interp_wse_fromnc.nc
