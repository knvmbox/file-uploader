#ifndef IMAGEBANDEF_HPP
#define IMAGEBANDEF_HPP

#include <string>


namespace imageban {

///////////////////////////////////////////////////////////////////////////////
struct album_t {
    std::string id;
    std::string name;
};

///////////////////////////////////////////////////////////////////////////////
struct image_t {
    std::string id;
    std::string album_id;
    std::string name;
    std::string image_name;
    std::string link;
};

}

#endif // IMAGEBANDEF_HPP
