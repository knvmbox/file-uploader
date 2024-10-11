#include <algorithm>

#include <QStringList>

#include "settings.hpp"


//-----------------------------------------------------------------------------
Settings::Settings() :
    m_settings("Vega Soft", "file-uploader") {
}

//-----------------------------------------------------------------------------
size_t Settings::imageSize() const {
    return m_settings.value("image/size", 500).toUInt();
}

//-----------------------------------------------------------------------------
std::vector<Settings::Secret> Settings::secrets() const {
    auto keys = loadStringList("imageban/secretkeys");
    auto titles = loadStringList("imageban/secrettitles");

    auto minSize = std::min(keys.size(), titles.size());
    std::vector<Settings::Secret> v;

    v.reserve(minSize);

    for(size_t ii = 0; ii != minSize; ++ii) {
        v.push_back({titles[ii], keys[ii]});
    }

    return v;
}

//-----------------------------------------------------------------------------
void Settings::setImageSize(size_t size) {
    m_settings.setValue("image/size", static_cast<unsigned>(size));
}

//-----------------------------------------------------------------------------
void Settings::setSecrets(const std::vector<Settings::Secret> &v) {
    QStringList keys;
    QStringList titles;


    std::transform(v.begin(), v.end(), std::back_inserter(keys), [](const Secret& s){
        return QString::fromStdString(s.key);
    });
    std::transform(v.begin(), v.end(), std::back_inserter(titles), [](const Secret& s){
        return QString::fromStdString(s.title);
    });

    m_settings.setValue("imageban/secretkeys", keys);
    m_settings.setValue("imageban/secrettitles", titles);
}

//-----------------------------------------------------------------------------
std::vector<std::string> Settings::loadStringList(const char *secName) const {
    auto list = m_settings.value(secName).toStringList();
    std::vector<std::string> v;

    v.reserve(list.size());
    for(const auto &i : list) {
        v.push_back(i.toStdString());
    }

    return v;
}

//-----------------------------------------------------------------------------
void Settings::saveStringList(const char *secName, const std::vector<std::string> &v) {
    QStringList list;

    list.reserve(v.size());
    for(const auto &i : v) {
        list <<i.c_str();
    }

    m_settings.setValue(secName, list);
}
