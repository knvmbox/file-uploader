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
#include "types/uploadparams.hpp"
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
    std::string bbcode;
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
    bool uploadImages(params::UploadParams);

public:
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
    std::vector<char> resizeImage(const void *data, size_t dataSize, size_t newSize);
    void uploadTask(params::UploadParams params, model::iterator, model::iterator);

    std::string uniqueFilename(const std::unordered_set<std::string>&, const std::string&);

private:
    const size_t m_maxThreads;

    std::shared_ptr<common::Logger> m_logger;
    std::vector<model::Item> m_items;

    size_t m_startedTasks;

    QIcon m_downIcon;
    QIcon m_upDownIcon;
    QIcon m_grayIcon;
};

Q_DECLARE_METATYPE(model::iterator)
Q_DECLARE_METATYPE(model::Item)
Q_DECLARE_METATYPE(model::ProcessType)
Q_DECLARE_METATYPE(imageban::image_t)

#endif // URLSMODEL_HPP
