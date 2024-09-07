#ifndef URLSMODEL_HPP
#define URLSMODEL_HPP

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

#include <QDir>
#include <QFile>
#include <QIcon>
#include <QProcess>

#include "abstractmodel.hpp"
#include "utils/imageban.hpp"


///////////////////////////////////////////////////////////////////////////////
enum class ItemStatus {
    NullStatus, DownloadedStatus, UploadedStatus
};

enum class ProcessType {
    DownloadProcess, UploadProcess
};

///////////////////////////////////////////////////////////////////////////////
struct Item {
    std::string filename;
    std::string downLink;
    std::string upLink;
    ItemStatus status;
};

///////////////////////////////////////////////////////////////////////////////
using iterator = std::vector<Item>::iterator;

///////////////////////////////////////////////////////////////////////////////
class UrlsModel : public AbstractModel
{
    Q_OBJECT

public:
    explicit UrlsModel(QObject *parent = nullptr);

public:
    bool downloadImages(const QString&);
    bool openUrlsFile(const QString&);
    bool saveUrlsFile(const QString&);
    bool uploadImages(const QString&);

public:
    bool canUpload() {
        return std::all_of(m_items.begin(), m_items.end(), [](const auto &item){
            return (item.status != ItemStatus::NullStatus);
        });
    }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return static_cast<int>(m_items.size());
    }

protected:
    QVariant decorationData(const QModelIndex&) const override;
    QVariant displayData(const QModelIndex &index) const override;

private slots:
    void updateItemStatus(iterator, Item);
    void updateTaskStatus(ProcessType);

signals:
    void itemComplete(iterator, Item);
    void taskComplete(ProcessType);
    void processComplete(ProcessType, bool);
    void processStart(ProcessType);

private:
    void clearModel() {
        m_items.clear();
    }

private:
    std::string createBbCode(std::string);
    std::string createBbCodeAsText(std::string);
    void downloadTask(iterator, iterator);
    bool isWebpImage(const std::string &filename);
    std::string replaceExt(const std::string &filename, const std::string &ext);
    bool startDownload();
    bool startUpload(const QString &album);
    void uploadTask(const QString&, iterator, iterator);

    std::string uniqueFilename(const std::unordered_set<std::string>&, const std::string&);

private:
    std::vector<Item> m_items;
    QDir m_workDir;

    size_t m_completedTasks;
    size_t m_maxThreads;

    QIcon m_downIcon;
    QIcon m_upDownIcon;
    QIcon m_grayIcon;
};

Q_DECLARE_METATYPE(iterator)
Q_DECLARE_METATYPE(Item)
Q_DECLARE_METATYPE(ProcessType)
Q_DECLARE_METATYPE(imageban::image_t)

#endif // URLSMODEL_HPP
