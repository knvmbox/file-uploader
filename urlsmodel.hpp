#ifndef URLSMODEL_HPP
#define URLSMODEL_HPP

#include <memory>
#include <string>
#include <vector>

#include <QDir>
#include <QFile>
#include <QProcess>

#include "model/abstractmodel.hpp"


class UrlsModel : public AbstractModel
{
    Q_OBJECT
public:
    struct Item {
        std::string link;
        bool status;
    };

public:
    explicit UrlsModel(QObject *parent = nullptr);

    QVariant displayData(const QModelIndex &index) const override;
    bool loadFile(const QString&);
    bool uploadFiles(const QString&);

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return static_cast<int>(m_items.size());
    }

signals:
    void complete(bool);
    void started();

private:
    void createProcess(const QDir&, const QString&);
    void saveUrls(QFile &);

private:
    std::vector<Item> m_items;
    std::unique_ptr<QProcess> m_process;
};

#endif // URLSMODEL_HPP
