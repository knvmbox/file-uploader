#include <filesystem>

#include "commonutils.hpp"

#include <QDir>

#include <fmt/core.h>


//-----------------------------------------------------------------------------
namespace utils {

bool checkDir(const std::string &dir) {
    QDir workDir{dir.c_str()};

    bool isExist = workDir.exists();
    bool isEmpty = workDir.entryList(QDir::NoDotAndDotDot | QDir::Files).isEmpty();

    return (isExist && isEmpty);
}

//-----------------------------------------------------------------------------
std::string createBbCode(const std::string &link) {
    if(link.empty()) {
        return {};
    }

    return fmt::format("[URL=https://imageban.ru][IMG]{}[/IMG][/URL]", link);
}

//-----------------------------------------------------------------------------
bool isWebpImage(const std::string &filename) {
    std::filesystem::path path{filename};

    std::string stem = path.extension();
    std::transform(stem.begin(), stem.end(), stem.begin(), ::toupper);

    return (stem == ".WEBP");
}

//-----------------------------------------------------------------------------
std::string replaceExt(const std::string &filename, const std::string &ext) {
    std::filesystem::path path{filename};
    path.replace_extension("." + ext);
    return path;
}

}
