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
    void setThumbSize(size_t);
    void setThumbSecretKey(const std::string&);
    std::string thumbSecretKey() const;
    size_t thumbSize() const;

private:
    QSettings m_settings;
};

#endif // SETTINGS_HPP
