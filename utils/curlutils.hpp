#ifndef CURLUTILS_HPP
#define CURLUTILS_HPP

#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

#include <curl/curl.h>


namespace curl {
namespace detail {

using curl_t = std::unique_ptr<CURL, decltype(&curl_easy_cleanup)>;

}

///////////////////////////////////////////////////////////////////////////////
static void curlInit(long flag = CURL_GLOBAL_DEFAULT) {
    curl_global_init(flag);
}

static void curlCleanup() {
    curl_global_cleanup();
}

///////////////////////////////////////////////////////////////////////////////
class curl_error : public std::runtime_error {
public:
    curl_error(const char *what) : std::runtime_error(what) {
    }

    curl_error(const std::string &what) : std::runtime_error(what) {
    }
};

///////////////////////////////////////////////////////////////////////////////
class CurlMime {
    using mime_t = std::unique_ptr<curl_mime, decltype(&curl_mime_free)>;

public:
    CurlMime() : m_mime{nullptr, curl_mime_free} {
    }

    CurlMime(detail::curl_t &curl) :
        m_mime{curl_mime_init(curl.get()), curl_mime_free} {
    }

    void addPart(const std::string &name, const std::string &data) {
        auto part = curl_mime_addpart(m_mime.get());
        curl_mime_name(part, name.c_str());
        curl_mime_data(part, data.c_str(), CURL_ZERO_TERMINATED);
    }

    void addFile(const std::string &name, const std::string &filename) {
        auto part = curl_mime_addpart(m_mime.get());
        curl_mime_name(part, name.c_str());
        curl_mime_filedata(part, filename.c_str());
    }

    curl_mime* get() {
        return m_mime.get();
    }

private:
    mime_t m_mime;
};

///////////////////////////////////////////////////////////////////////////////
class CurlStringList {
    using curl_slist_t = std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)>;

public:
    CurlStringList() : m_list{nullptr, curl_slist_free_all} {
    }

    void append(const char *str) {
        curl_slist *tmp = curl_slist_append(m_list.get(), str);
        if(!tmp) {
            throw curl_error("Can't append string to a list");
        }
        m_list.release();
        m_list.reset(tmp);
    }

    curl_slist* get() {
        return m_list.get();
    }

private:
    curl_slist_t m_list;
};

///////////////////////////////////////////////////////////////////////////////
class CurlDownloader {
public:
    CurlDownloader() :
        m_curl{curl_easy_init(), curl_easy_cleanup} {
        curl_easy_setopt(m_curl.get(), CURLOPT_WRITEFUNCTION, writeFunc);
        curl_easy_setopt(m_curl.get(), CURLOPT_WRITEDATA, this);
    }

    void appendHeader(const char *str) {
        m_headers.append(str);
    }

    const char* data() {
        return m_dataBuf.data();
    }

    void download(std::string url) {
        m_dataBuf.clear();

        curl_easy_setopt(m_curl.get(), CURLOPT_URL, url.c_str());
        if(m_headers.get()) {
             curl_easy_setopt(m_curl.get(), CURLOPT_HTTPHEADER, m_headers.get());
        }

        auto res = curl_easy_perform(m_curl.get());
        if(res != CURLE_OK) {
            throw curl_error(curl_easy_strerror(res));
        }
    }

    detail::curl_t& get() {
        return m_curl;
    }

    void setMime(CurlMime mime) {
        m_mime = std::move(mime);
        curl_easy_setopt(m_curl.get(), CURLOPT_MIMEPOST, m_mime.get());
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
    detail::curl_t m_curl;
    std::vector<char> m_dataBuf;
    CurlStringList m_headers;
    CurlMime m_mime;
};



};

#endif // CURLUTILS_HPP
