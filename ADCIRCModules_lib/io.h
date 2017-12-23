#ifndef IO_H
#define IO_H

#include <string>
#include <vector>

namespace FileIO {
enum _fileio {
  NoError,
  GenericFileReadError,
  FileReadErrorNode,
  FileReadErrorElement,
  FileReadErrorBoundary
};
};

class IO {
public:
  explicit IO();

  static int readFileData(std::string filename, std::vector<std::string> &data);

  static int splitString(std::string &data, std::vector<std::string> &result);

  static int splitStringNodeFormat(std::string &data, int &id, double &x,
                                   double &y, double &z);

  static int splitStringElemFormat(std::string &data, int &id, int &n1, int &n2,
                                   int &n3);

  static int splitStringBoundary0Format(std::string &data, int &node1);

  static int splitStringBoundary23Format(std::string &data, int &node1,
                                         double &crest, double &supercritical);

  static int splitStringBoundary24Format(std::string &data, int &node1,
                                         int &node2, double &crest,
                                         double &subcritical,
                                         double &supercritical);

  static int splitStringBoundary25Format(std::string &data, int &node1,
                                         int &node2, double &crest,
                                         double &subcritical,
                                         double &supercritical,
                                         double &pipeheight, double &pipecoef,
                                         double &pipediam);
};

#endif // IO_H
