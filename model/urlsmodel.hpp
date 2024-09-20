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

#include <common/logger/logger.hpp>

#include "abstractmodel.hpp"
#include "utils/imageban.hpp"


namespace model {

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

}

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
            return (item.status != model::ItemStatus::NullStatus);
        });
    }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return static_cast<int>(m_items.size());
    }

protected:
    QVariant decorationData(const QModelIndex&) const override;
    QVariant displayData(const QModelIndex &index) const override;

private slots:
    void updateItemStatus(model::iterator, model::Item);
    void updateTaskStatus(model::ProcessType);

signals:
    void itemComplete(model::iterator, model::Item);
    void taskComplete(model::ProcessType);
    void processComplete(model::ProcessType, bool);
    void processStart(model::ProcessType);

private:
    void clearModel() {
        m_items.clear();
    }

private:
    std::string createBbCode(std::string);
    std::string createBbCodeAsText(std::string);
    void downloadTask(model::iterator, model::iterator);
    bool isWebpImage(const std::string &filename);
    bool makeThumb(const std::string &, size_t);
    std::string makeThrumbFilename(const std::string&, const std::string &thrumbDir);
    std::string replaceExt(const std::string &filename, const std::string &ext);
    bool startDownload();
    bool startUpload(const QString &album);
    void uploadTask(const QString&, model::iterator, model::iterator);

    std::string uniqueFilename(const std::unordered_set<std::string>&, const std::string&);

private:
    std::shared_ptr<common::Logger> m_logger;
    std::vector<model::Item> m_items;
    QDir m_workDir;
    QDir m_thumbsDir;

    size_t m_completedTasks;
    size_t m_maxThreads;

    QIcon m_downIcon;
    QIcon m_upDownIcon;
    QIcon m_grayIcon;
};

Q_DECLARE_METATYPE(model::iterator)
Q_DECLARE_METATYPE(model::Item)
Q_DECLARE_METATYPE(model::ProcessType)
Q_DECLARE_METATYPE(imageban::image_t)

#endif // URLSMODEL_HPP
