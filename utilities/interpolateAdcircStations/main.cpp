#include <iostream>
#include <string>
#include "boost/program_options.hpp"
#include "interpolate.h"

int main(int argc, char *argv[]) {
  namespace po = boost::program_options;

  po::options_description desc("Options");

  Interpolate::InputOptions input;

      // clang-format off
  desc.add_options() ("help", "show this message")
                     ("mesh", po::value<std::string>(&input.mesh), "specify the ADCIRC geometry used in the global file")
                     ("global", po::value<std::string>(&input.globalfile),"specify the global ADCIRC output")
                     ("station", po::value<std::string>(&input.stationfile), "specify the station locations to be used")
                     ("output",po::value<std::string>(&input.outputfile), "specify the output file to be created")
                     ("magnitude", po::bool_switch(&input.magnitude)->default_value(false),"write out the magnitude of vector quantities")
                     ("direction",po::bool_switch(&input.direction),"write out the direction of vector quantities")
                     ("readimeds", po::bool_switch(&input.readimeds)->default_value(false),"read station data as IMEDS instead of xyz")
                     ("writeimeds", po::bool_switch(&input.writeimeds)->default_value(false),"write station data as IMEDS instead of fort.61")
                     ("readDFlow", po::bool_switch(&input.readdflow)->default_value(false),"read a DFlow-FM polyline file for station locations")
                     ("writeDFlow",po::bool_switch(&input.writedflow)->default_value(false),"write DFlow-FM boundary conditions files")
                     ("coldstart",po::value<std::string>(&input.coldstart) ,"date/time of the simulation cold start for time reference in IMEDS files. Ex: yyyymmddhhmmss (year-month-day-hour-minute-second)")
                     ("refdate", po::value<std::string>(&input.refdate),"date/time reference used when writing input files for DFlow-FM. Ex: yyyymmddhhmmss (year-month-day-hour-minute-second)")
                     ("multiplier", po::value<double>(&input.multiplier),"value multiplied to the output value")
                     ("start_snap",po::value<int>(&input.startsnap),"start reading netCDF output at specified snap")
                     ("end_snap", po::value<int>(&input.endsnap),"stop reading netCDF output at specified snap")
                     ("keepwet", po::bool_switch(&input.keepwet)->default_value(false),"If a point goes dry, use the previous wet value")
                     ("readAsciiMesh", po::bool_switch(&input.readasciimesh)->default_value(false),"Force the code to read an ascii meshfile instead of defaulting to the NetCDF ADCIRC output file data")
                     ("epsg_global",po::value<int>(&input.epsg_global),"Specify the coordinate system of the global time series file (default: 4326)")
                     ("epsg_station", po::value<int>(&input.epsg_station),"Specify the coordinate system of the input station locations (default: 4326)")
                     ("epsg_output",po::value<int>(&input.epsg_output),"Specify the coordinate system of the output data file (default: 4326)");
  // clang-format on

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (argc == 1) {
    std::cout << "Error: No options specified." << std::endl;
    std::cout << desc << std::endl;
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }

  Interpolate interp(input);
  interp.run();

  return 0;
}
