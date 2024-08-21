#ifndef CURLDOWNLOADER_HPP
#define CURLDOWNLOADER_HPP

#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

#include <curl/curl.h>


///////////////////////////////////////////////////////////////////////////////
class curl_exception : public std::runtime_error {
public:
    curl_exception(const char *what) : std::runtime_error(what) {
    }

    curl_exception(const std::string &what) : std::runtime_error(what) {
    }
};

///////////////////////////////////////////////////////////////////////////////
class CurlDownloader {
public:
    using curl_t = std::unique_ptr<CURL, decltype(&curl_easy_cleanup)>;

public:
    static void curlInit(long flag = CURL_GLOBAL_DEFAULT) {
        curl_global_init(flag);
    }

    static void curlCleanup() {
        curl_global_cleanup();
    }

public:
    CurlDownloader(std::string url) :
        m_curl{curl_easy_init(), curl_easy_cleanup},
        m_url{std::move(url)} {
        curl_easy_setopt(m_curl.get(), CURLOPT_URL, m_url.c_str());
        curl_easy_setopt(m_curl.get(), CURLOPT_WRITEFUNCTION, writeFunc);
        curl_easy_setopt(m_curl.get(), CURLOPT_WRITEDATA, this);
    }

    const char* data() {
        return m_dataBuf.data();
    }

    void download() {
        m_dataBuf.clear();

        auto res = curl_easy_perform(m_curl.get());
        if(res != CURLE_OK) {
            throw curl_exception(curl_easy_strerror(res));
        }
    }

    size_t save(const std::string &filename) {
        std::ofstream out(filename, std::ios::binary);
        out.write(m_dataBuf.data(), m_dataBuf.size());

        return m_dataBuf.size();
    }

    size_t size() {
        return m_dataBuf.size();
    }

private:
    static size_t writeFunc(char *data, size_t size, size_t nmemb, void *ptr) {
        size_t realsize = size*nmemb;

        auto d = static_cast<CurlDownloader*>(ptr);
        d->appendData(data, realsize);
        return realsize;
    }

private:
    void appendData(const char *data, size_t size) {
        std::copy_n(data, size, std::back_inserter(m_dataBuf));
    }

private:
    curl_t m_curl;
    std::string m_url;
    std::vector<char> m_dataBuf;
};


#endif // CURLDOWNLOADER_HPP
