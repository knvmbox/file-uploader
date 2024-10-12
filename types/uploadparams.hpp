#ifndef UPLOADPARAMS_HPP
#define UPLOADPARAMS_HPP

#include <string>

#include "imagebandef.hpp"


namespace params {

struct UploadParams {
    std::string secretKey;
    imageban::album_t album;
    bool isSave;
    std::string dirPath;
};

}


#endif // UPLOADPARAMS_HPP
