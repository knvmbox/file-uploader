#include "commonutils.hpp"

#include <QDir>


//-----------------------------------------------------------------------------
namespace utils {

bool checkDir(const char *dir) {
    QDir workDir{dir};

    bool isExist = workDir.exists();
    bool isEmpty = workDir.entryList(QDir::NoDotAndDotDot | QDir::Files).isEmpty();

    return (isExist && isEmpty);
}

}
