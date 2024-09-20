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
    void setThrumbSize(size_t);
    void setThrumbSecretKey(const std::string&);
    std::string thrumbSecretKey() const;
    size_t thumbSize() const;

private:
    QSettings m_settings;
};

#endif // SETTINGS_HPP
