#ifndef PNGUTILS_HPP
#define PNGUTILS_HPP

#include <stdint.h>

#include <stdexcept>
#include <string>
#include <vector>

#include <png.h>
#include <setjmp.h>         // note: this must be included *after* png.h
#include <webp/decode.h>


namespace {
///////////////////////////////////////////////////////////////////////////////
void PNGAPI error_func(png_structp png, png_const_charp) {
    longjmp(png_jmpbuf(png), 1);
}

///////////////////////////////////////////////////////////////////////////////
void write_func(png_structp png, png_bytep data, size_t size) {
    std::vector<uint8_t> *p = static_cast<std::vector<uint8_t>*>(png_get_io_ptr(png));
    p->insert(p->end(), data, data + size);
}

}

namespace png {

///////////////////////////////////////////////////////////////////////////////
class png_error : public std::runtime_error {
public:
    png_error(const char *what) : std::runtime_error(what){
    }

    png_error(const std::string &what) : std::runtime_error(what){
    }
};

///////////////////////////////////////////////////////////////////////////////
std::vector<uint8_t> decodeToMemory(const WebPDecBuffer * const buffer) {
    std::vector<uint8_t> out;

    png_structp png = png_create_write_struct(
        PNG_LIBPNG_VER_STRING, nullptr, error_func, nullptr
    );

    if(!png) {
        throw png_error("Error while calling 'png_create_write_struct'");
    }

    png_infop info = png_create_info_struct(png);
    if(!info) {
        png_destroy_write_struct(&png, nullptr);
        throw png_error("Error while colling 'png_create_info_struct'");
    }

    if(setjmp(png_jmpbuf(png)))
    {
        png_destroy_write_struct(&png, &info);
        throw png_error("Error while colling 'setjmp(png_jmpbuf(png)'");
    }

    png_set_write_fn(png, (void *)&out, write_func, nullptr);

    const uint32_t width = buffer->width;
    const uint32_t height = buffer->height;
    png_bytep row = buffer->u.RGBA.rgba;
    const int stride = buffer->u.RGBA.stride;
    const int has_alpha = WebPIsAlphaMode(buffer->colorspace);
    uint32_t y;

    png_set_IHDR(
        png, info,
        width, height,
        8,
        has_alpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(png, info);
    for (y = 0; y < height; ++y) {
        png_write_rows(png, &row, 1);
        row += stride;
    }

    png_write_end(png, info);
    png_destroy_write_struct(&png, &info);

    return out;
}

}

#endif // PNGUTILS_HPP
