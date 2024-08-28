#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>

#include <QSettings>


class Settings
{
public:
    Settings();

    std::string secretKey() const;
    void setSecretKey(const std::string&);

private:
    QSettings m_settings;
};

#endif // SETTINGS_HPP
