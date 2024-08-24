#ifndef URLSMODEL_HPP
#define URLSMODEL_HPP

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

#include <QDir>
#include <QFile>
#include <QProcess>

#include "abstractmodel.hpp"

struct Item {
    std::string filename;
    std::string link;
    bool status;
};

using iterator = std::vector<Item>::iterator;

class UrlsModel : public AbstractModel
{
    Q_OBJECT

public:
    explicit UrlsModel(QObject *parent = nullptr);

    QVariant displayData(const QModelIndex &index) const override;

public:
    bool downloadImages(const QString&);
    bool openUrlsFile(const QString&);

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return static_cast<int>(m_items.size());
    }

private slots:
    void updateItemStatus(iterator, bool);
    void updateTaskStatus();

signals:
    void downloadComplete(bool);
    void downloadStart();
    void itemComplete(iterator, bool);
    void taskComplete();

private:
    void clearModel() {
        m_items.clear();
    }

private:
    void downloadTask(const QString&, iterator, iterator);
    bool startDownload(const QString&);

    std::string uniqueFilename(const std::unordered_set<std::string>&, const std::string&);

private:
    std::vector<Item> m_items;
    std::unique_ptr<QProcess> m_process;

    size_t m_completedTasks;
    size_t m_maxThreads;
};

Q_DECLARE_METATYPE(iterator)

#endif // URLSMODEL_HPP
