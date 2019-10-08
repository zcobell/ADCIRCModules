#include "interpolate.h"

Interpolate::Interpolate(const InputOptions &options)
    : m_inputOptions(options) {}

Adcirc::Geometry::Mesh Interpolate::readMesh(
    const Adcirc::Output::OutputFormat &filetype) {
  Adcirc::Geometry::Mesh mesh;
  if (filetype == Adcirc::Output::OutputNetcdf3 ||
      filetype == Adcirc::Output::OutputNetcdf4) {
    if (this->m_inputOptions.readasciimesh) {
      mesh.setFilename(this->m_inputOptions.mesh);
    } else {
      mesh.setFilename(this->m_inputOptions.globalfile);
    }
  } else {
    mesh.setFilename(this->m_inputOptions.mesh);
  }
  mesh.read();
  return mesh;
}

void Interpolate::run() {
  Hmdf stationData;
  Adcirc::Output::ReadOutput globalFile(this->m_inputOptions.globalfile);
  globalFile.open();
  Adcirc::Output::OutputFormat filetype = globalFile.filetype();
  Adcirc::Geometry::Mesh m = this->readMesh(filetype);

  for (size_t i = 0; i < globalFile.numSnaps(); ++i) {
    globalFile.read(i);
    std::cout << i << std::endl;

    globalFile.clearAt(0);
  }

  return;
}
