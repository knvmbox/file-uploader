#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>
#include <vector>

#include <QSettings>


class Settings
{
public:
    struct Secret {
        std::string title;
        std::string key;
    };

public:
    Settings();

    size_t imageSize() const;
    std::vector<Secret> secrets() const;

    void setImageSize(size_t);
    void setSecrets(const std::vector<Secret>&);

private:
    std::vector<std::string> loadStringList(const char *secName) const;
    void saveStringList(const char *secName, const std::vector<std::string>&);

private:
    QSettings m_settings;
};

#endif // SETTINGS_HPP
