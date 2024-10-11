#include <algorithm>

#include <QColor>

#include "../settings.hpp"
#include "imagebanconfigviewmodel.hpp"


//-----------------------------------------------------------------------------
ImagebanConfigViewModel::ImagebanConfigViewModel(QObject *parent)
    : AbstractModel{parent}
{
    addColumn("Название");
    addColumn("Secret Key");

    load();
}

//-----------------------------------------------------------------------------
ImagebanConfigViewModel::~ImagebanConfigViewModel() {
    save();
}

//-----------------------------------------------------------------------------
QVariant ImagebanConfigViewModel::displayData(const QModelIndex &idx) const {
    int row = idx.row();
    int col = idx.column();

    if(row == m_items.size()) {
        return {};
    }

    switch(col) {
    case 0: return m_items.at(row).title;
    case 1: return m_items.at(row).key;
    }

    return {};
}

//-----------------------------------------------------------------------------
QVariant ImagebanConfigViewModel::editData(const QModelIndex &idx) const {
    int row = idx.row();
    int col = idx.column();

    if(row == m_items.size()) {
        return {};
    }

    switch(col) {
    case 0: return m_items.at(row).title;
    case 1: return m_items.at(row).key;
    }

    return {};
}

//-----------------------------------------------------------------------------
QVariant ImagebanConfigViewModel::foregroundColor(const QModelIndex &idx) const {
    int row = idx.row();

    if(
        row < m_items.size()
        && (m_items.at(row).title.isEmpty() || m_items.at(row).key.isEmpty())
    ) {
        return QColor{200, 25, 25};
    }

    return {};
}

//-----------------------------------------------------------------------------
bool ImagebanConfigViewModel::setValueData(const QModelIndex &idx, const QVariant &v) {
    int row = idx.row();
    int col = idx.column();

    QString text = v.toString().trimmed();

    auto setValue = [](Item &item, const QString &val, int col) {
        switch(col) {
        case 0: item.title = val.trimmed(); break;
        case 1: item.key = val.trimmed(); break;
        }
    };

    if(row == m_items.size()) {
        Item item;
        if(text.isEmpty()) {
            return false;
        }
        setValue(item, text, col);

        beginInsertRows(QModelIndex{}, m_items.size(), m_items.size());
        m_items.push_back(std::move(item));
        endInsertRows();

        return true;
    }

    setValue(m_items[row], text, col);
    return true;
}

//-----------------------------------------------------------------------------
void ImagebanConfigViewModel::load() {
    Settings settings;

    auto secrets = settings.secrets();
    std::transform(secrets.begin(), secrets.end(), std::back_inserter(m_items), [](auto &s){
        Item item = {s.title.c_str(), s.key.c_str()};
        return item;
    });
}

//-----------------------------------------------------------------------------
void ImagebanConfigViewModel::save() const {
    QVector<Item> valid;
    valid.reserve(m_items.size());

    std::copy_if(m_items.begin(), m_items.end(), std::back_inserter(valid), [](const Item &i){
        return (!(i.title.isEmpty() || i.key.isEmpty()));
    });

    std::vector<Settings::Secret> secrets;
    secrets.reserve(valid.size());

    std::transform(valid.begin(), valid.end(), std::back_inserter(secrets), [](auto &s) {
        Settings::Secret secret = {s.title.toStdString(), s.key.toStdString()};
        return secret;
    });

    Settings settings;
    settings.setSecrets(secrets);
}
