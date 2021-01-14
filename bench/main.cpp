//------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2019 Zachary Cobell
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
#include <QCoreApplication>
#include <QPointer>
#include "adcircmesh.h"
#include "benchmark/benchmark.h"

static void bench_readmesh(benchmark::State &state) {
  AdcircMesh *mesh = new AdcircMesh(std::string(
      "/home/zcobell/Development/QADCModules/testing/test_files/ms-riv.grd"));
  while (state.KeepRunning()) {
    mesh->read();
  }
  delete mesh;
}

BENCHMARK(bench_readmesh);

BENCHMARK_MAIN()
