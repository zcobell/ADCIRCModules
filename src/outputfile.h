#ifndef ADCMOD_OUTPUTFILE_H
#define ADCMOD_OUTPUTFILE_H

#include <string>
#include <vector>
#include "filetypes.h"
#include "outputmetadata.h"

namespace Adcirc {

namespace Output {

class OutputFile {
 public:
  OutputFile(const std::string &filename);
  virtual ~OutputFile();

  virtual void open() = 0;

  virtual void close() = 0;

  bool exists();

  bool isOpen();

  std::string header() const;
  void setHeader(const std::string &header);

  std::string filename() const;
  void setFilename(const std::string &filename);

  size_t numSnaps() const;
  void setNumSnaps(size_t numSnaps);

  size_t numNodes() const;
  void setNumNodes(size_t numNodes);

  double dt() const;
  void setDt(double dt);

  int dIteration() const;
  void setDiteration(int dit);

  Adcirc::Output::OutputFormat filetype() const;
  Adcirc::Output::OutputFormat setFiletype(
      Adcirc::Output::OutputFormat filetype);

  size_t currentSnap() const;
  void setCurrentSnap(const size_t &currentSnap);

  double defaultValue() const;
  void setDefaultValue(double defaultValue);

  static const std::vector<Adcirc::Output::OutputMetadata>
      *adcircFileMetadata();

  Adcirc::Output::OutputMetadata *metadata();
  void setMetadata(const Adcirc::Output::OutputMetadata &metadata);

  double modelDt() const;
  void setModelDt(double modelDt);


  void setOpen(bool open);

private:
  bool m_open;
  Adcirc::Output::OutputFormat m_filetype;
  size_t m_currentSnap;
  size_t m_numSnaps;
  size_t m_numNodes;
  double m_modelDt;
  double m_dt;
  double m_defaultValue;
  int m_dit;
  std::string m_filename;
  std::string m_units;
  std::string m_description;
  std::string m_name;
  std::string m_header;
  Adcirc::Output::OutputMetadata m_metadata;
};

}  // namespace Output
}  // namespace Adcirc

#endif  // ADCMOD_OUTPUTFILE_H
