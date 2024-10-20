#ifndef COMMONUTILS_HPP
#define COMMONUTILS_HPP

#include <string>

namespace utils {

bool checkDir(const std::string&);
std::string createBbCode(const std::string&);
bool isWebpImage(const std::string&);
std::string replaceExt(const std::string &filename, const std::string &ext);

}


#endif // COMMONUTILS_HPP
