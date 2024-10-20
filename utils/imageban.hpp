#ifndef IMAGEBAN_HPP
#define IMAGEBAN_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "curlutils.hpp"
#include "types/imagebandef.hpp"


namespace imageban {

///////////////////////////////////////////////////////////////////////////////
struct imageban_error : public std::runtime_error {
    imageban_error(const char *what) : std::runtime_error(what) {
    }

    imageban_error(const std::string &what) : std::runtime_error(what) {
    }
};

///////////////////////////////////////////////////////////////////////////////
class ImageBan {
    using json = nlohmann::json;

public:
    ImageBan(std::string s) : m_secret{std::move(s)} {
    }

    std::vector<album_t> albumsList() {
        curl::CurlDownloader request;
        std::vector<album_t> result;

        addSecret(request);
        request.download("https://api.imageban.ru/v1/account/me/albums/ids");
        std::string response{request.data(), request.size()};

        json doc = json::parse(response);
        if(doc.contains("error")) {
            throw_error(std::move(doc));
        }

        json data = doc.at("data");
        result.reserve(data.size());
        for (auto& element : data) {
            album_t album = albumInfo(element);
            result.push_back(std::move(album));
        }

        return result;
    }

    album_t createAlbum(std::string name) {
        album_t album = {"", std::move(name)};

        curl::CurlDownloader request;
        curl::CurlMime mime{request.get()};
        mime.addPart("album_name", album.name);
        mime.addPart("album_desc", "Описание альбома");
        mime.addPart("album_type", "public");
        request.setMime(std::move(mime));

        addSecret(request);
        request.download("https://api.imageban.ru/v1/album");

        auto data = request.data();
        json doc = json::parse(data);
        if(doc.contains("error")) {
            throw_error(std::move(doc));
        }

        album.id = doc.at("data").at("id").get<std::string>();
        return album;
    }

    image_t uploadImage(
        const std::string &album, const std::string &filepath, std::vector<char> data
    ) {
        curl::CurlDownloader request;

        curl::CurlMime mime{request.get()};
        mime.addFile("image", filepath, data);
        mime.addPart("album", album);

        addSecret(request);
        request.setMime(std::move(mime));
        request.download("https://api.imageban.ru/v1");

        json doc = json::parse(std::string(request.data(), request.size()));
        if(doc.contains("error")) {
            throw_error(std::move(doc));
        }

        image_t image = {
            .id = doc.at("data").at("id").get<std::string>(),
            .album_id = doc.at("data").at("album").get<std::string>(),
            .name = doc.at("data").at("name").get<std::string>(),
            .image_name = doc.at("data").at("img_name").get<std::string>(),
            .link = doc.at("data").at("link").get<std::string>()
        };

        return image;
    }

private:
    void addSecret(curl::CurlDownloader &d) {
        auto header = std::string("Authorization: Bearer ") + m_secret;
        d.appendHeader(header.c_str());
    }

    album_t albumInfo(std::string id) {
        album_t album = {std::move(id), ""};

        std::string  url = std::string{"https://api.imageban.ru/v1/album/"} + album.id;

        curl::CurlDownloader request;
        addSecret(request);
        request.download(url);

        auto doc = json::parse(std::string(request.data(), request.size()));
        if(doc.contains("error")) {
            throw_error(std::move(doc));
        }

        album.name = doc.at("data").at("name").get<std::string>();
        return album;
    }

    void throw_error(json doc) {
        auto errCode = doc.at("error").at("code").get<std::string>();
        auto errText = doc.at("error").at("message").get<std::string>();

        throw imageban_error{errCode + ": " +errText};
    }

private:
    std::string m_secret;
};

}

#endif // IMAGEBAN_HPP
