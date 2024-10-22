#ifndef WEBPDECODER_HPP
#define WEBPDECODER_HPP

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

#include <webp/config.h>
#include <imageio/image_enc.h>
#include <imageio/webpdec.h>

#include "pngutils.hpp"


///////////////////////////////////////////////////////////////////////////////
struct webp_error : public std::runtime_error {
    webp_error(const char *str) : std::runtime_error(str) {
    }

    webp_error(const std::string &str) : std::runtime_error(str) {
    }
};

///////////////////////////////////////////////////////////////////////////////
class WebpDecoder {
private:
    using outputBuffer_t = std::unique_ptr<WebPDecBuffer, decltype(&WebPFreeDecBuffer)>;
    using dataBuffer_t = std::unique_ptr<uint8_t, void(*)(const uint8_t *data)>;

public:
    WebpDecoder(std::string filename) : WebpDecoder() {
        WebPBitstreamFeatures* const bitstream = &m_config.input;
        const uint8_t* ptr = nullptr;
        size_t dataSize = 0;

        if (!LoadWebP(filename.c_str(), &ptr, &dataSize, bitstream)) {
            throw webp_error("Can't load image!");
        }

        dataBuffer_t data(const_cast<uint8_t*>(ptr), +[](const uint8_t *data){
            WebPFree((void*)data);
        });
        m_outBuffer->colorspace = bitstream->has_alpha ? MODE_RGBA : MODE_RGB;

        VP8StatusCode status = DecodeWebP(data.get(), dataSize, &m_config);
        if(status != VP8_STATUS_OK) {
            throw webp_error(decodeError(status));
        }
    }

    WebpDecoder(const uint8_t *data, size_t dataSize) : WebpDecoder() {
        WebPBitstreamFeatures* const bitstream = &m_config.input;

        auto status = WebPGetFeatures(data, dataSize, bitstream);
        if (status != VP8_STATUS_OK) {
            throw webp_error(decodeError(status));
        }

        status = DecodeWebP(data, dataSize, &m_config);
        if(status != VP8_STATUS_OK) {
            throw webp_error(decodeError(status));
        }
    }

    WebpDecoder(const std::vector<uint8_t> &data) : WebpDecoder(data.data(), data.size()) {
    }

    void save(std::string filename) {
        auto ok = WebPSaveImage(m_outBuffer.get(), PNG, filename.c_str());
        if(!ok) {
            throw webp_error("can't create file!");
        }
    }

    std::vector<uint8_t> decode() {
        return png::decodeToMemory(m_outBuffer.get());
    }

private:
    WebpDecoder() :
        m_outBuffer{nullptr, WebPFreeDecBuffer} {
        m_outBuffer.reset(&m_config.output);
        if (!WebPInitDecoderConfig(&m_config)) {
            throw webp_error("Library version mismatch!");
        }
    }

    std::string decodeError(VP8StatusCode status) {
        static const char* errors[] = {
            "Success decode image",
            "Out of memory",
            "Invalid parameters",
            "Bitstream error",
            "Unsupported feature",
            "Suspended",
            "User aborted",
            "Not enough data"
        };

        if (status >= VP8_STATUS_OK && status <= VP8_STATUS_NOT_ENOUGH_DATA) {
            return errors[status];
        }

        return std::string("Unknown error");
    }

private:
    WebPDecoderConfig m_config;
    outputBuffer_t m_outBuffer;
};


#endif // WEBPDECODER_HPP
