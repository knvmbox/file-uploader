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
