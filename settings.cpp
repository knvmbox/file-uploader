#include "settings.hpp"


//-----------------------------------------------------------------------------
Settings::Settings() :
    m_settings("Vega Soft", "file-uploader") {
}

//-----------------------------------------------------------------------------
std::string Settings::secretKey() const {
    return m_settings.value("imageban/secretkey").toString().toStdString();
}

//-----------------------------------------------------------------------------
void Settings::setSecretKey(const std::string &secret) {
    m_settings.setValue("imageban/secretkey", secret.c_str());
}

//-----------------------------------------------------------------------------
void Settings::setThumbSize(size_t size) {
    m_settings.setValue("thumbs/size", static_cast<unsigned>(size));
}

//-----------------------------------------------------------------------------
void Settings::setThumbSecretKey(const std::string &key) {
    m_settings.setValue("imageban/thumbsecretkey", key.c_str());
}

//-----------------------------------------------------------------------------
std::string Settings::thumbSecretKey() const {
    return m_settings.value("imageban/thumbsecretkey").toString().toStdString();
}

//-----------------------------------------------------------------------------
size_t Settings::thumbSize() const {
    return m_settings.value("thumbs/size", 300).toUInt();
}
