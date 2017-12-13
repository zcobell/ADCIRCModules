#include "adcircmesh.h"
#include "benchmark/benchmark.h"
#include <QCoreApplication>
#include <QPointer>

static void bench_readmesh(benchmark::State &state) {
  AdcircMesh *mesh = new AdcircMesh(
      "/home/zcobell/Development/QADCModules/testing/test_files/ms-riv.grd");
  while (state.KeepRunning()) {
    mesh->read();
  }
  delete mesh;
}

BENCHMARK(bench_readmesh);

BENCHMARK_MAIN()
