#ifndef IO_H
#define IO_H

#include <vector>
#include <string>

namespace FileIO {
    enum _fileio{
        NoError,
        GenericFileReadError,
        FileReadErrorNode,
        FileReadErrorElement
    };
};

class IO
{
public: 
    explicit IO();

    static int readFileData(std::string filename, std::vector<std::string> &data);

    static int splitString(std::string &data,std::vector<std::string> &result);

    static int splitStringNodeFormat(std::string &data, int &id, double &x, double &y, double &z);
    
    static int splitStringElemFormat(std::string &data, int &id, int &n1, int &n2, int &n3);

};

#endif //IO_H
