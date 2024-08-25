#ifndef URLSMODEL_HPP
#define URLSMODEL_HPP

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

#include <QDir>
#include <QFile>
#include <QProcess>

#include "abstractmodel.hpp"


///////////////////////////////////////////////////////////////////////////////
struct Item {
    std::string filename;
    std::string link;
    bool status;
};

///////////////////////////////////////////////////////////////////////////////
using iterator = std::vector<Item>::iterator;

///////////////////////////////////////////////////////////////////////////////
class UrlsModel : public AbstractModel
{
    Q_OBJECT

public:
    explicit UrlsModel(QObject *parent = nullptr);

    QVariant displayData(const QModelIndex &index) const override;

public:
    bool downloadImages(const QString&);
    bool openUrlsFile(const QString&);
    bool uploadImages(const QString&);

public:
    bool canUpload() {
        return std::all_of(m_items.begin(), m_items.end(), [](const auto &item){
            return item.status;
        });
    }
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
    void uploadStart();

private:
    void clearModel() {
        m_items.clear();
    }

private:
    void downloadTask(iterator, iterator);
    bool startDownload();
    bool startUpload(const QString &album);
    void uploadTask(const QString&, iterator, iterator);

    std::string uniqueFilename(const std::unordered_set<std::string>&, const std::string&);

private:
    std::vector<Item> m_items;
    std::unique_ptr<QProcess> m_process;

    size_t m_completedTasks;
    size_t m_maxThreads;

    QDir m_workDir;
};

Q_DECLARE_METATYPE(iterator)

#endif // URLSMODEL_HPP
