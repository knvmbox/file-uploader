#include <filesystem>
#include <iterator>

#include <QDebug>

#include <QColor>
#include <QIcon>
#include <QProcess>
#include <QTextStream>
#include <QThread>
#include <QThreadPool>
#include <QUrl>

#include "../utils/curlutils.hpp"
#include "../utils/imageban.hpp"
#include "urlsmodel.hpp"


///////////////////////////////////////////////////////////////////////////////
static constexpr int statusRow = 0;
static constexpr int filenameRow = 1;
static constexpr int downUrlRow = 2;

//-----------------------------------------------------------------------------
UrlsModel::UrlsModel(QObject *parent) :
    AbstractModel{parent},
    m_completedTasks{0},
    m_maxThreads{static_cast<size_t>(QThread::idealThreadCount())},
    m_downIcon{":/img/down.png"},
    m_upDownIcon{":/img/up-and-down.png"},
    m_grayIcon{":/img/gray-up-and-down.png"} {
    addColumn("");
    addColumn("Filename");
    addColumn("Download URL");

    qRegisterMetaType<iterator>();
    qRegisterMetaType<ItemStatus>();
    qRegisterMetaType<ProcessType>();

    connect(this, &UrlsModel::itemComplete, this, &UrlsModel::updateItemStatus);
    connect(this, &UrlsModel::taskComplete, this, &UrlsModel::updateTaskStatus);
}

//-----------------------------------------------------------------------------
bool UrlsModel::downloadImages(const QString &dir) {
    QDir workDir{dir};

    bool isExist = workDir.exists();
    bool isEmpty = workDir.entryList(QDir::NoDotAndDotDot | QDir::Files).isEmpty();
    if(!isExist || !isEmpty) {
        return false;
    }

    m_workDir = std::move(workDir);
    return startDownload();
}

//-----------------------------------------------------------------------------
bool UrlsModel::openUrlsFile(const QString &filename) {
    QFile urlsFile(filename);
    if (!urlsFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    beginResetModel();
    clearModel();

    std::unordered_set<std::string> filenamesSet;

    QTextStream in(&urlsFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QUrl url{line};

        auto filename = url.fileName().toStdString();

        if(filenamesSet.count(filename)) {
            filename = uniqueFilename(filenamesSet, filename);
        }

        m_items.push_back(
            {filename, line.toStdString(), ItemStatus::NullStatus}
        );
        filenamesSet.insert(filename);
    }
    endResetModel();

    return true;
}

//-----------------------------------------------------------------------------
bool UrlsModel::uploadImages(const QString &album) {
    bool isExist = m_workDir.exists();
    auto files = m_workDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
    bool existAll = std::all_of(m_items.begin(), m_items.end(), [&files](const Item &file) {
        return files.contains(file.filename.c_str());
    });

    if(!isExist || files.isEmpty() || !existAll) {
        return false;
    }

    return startUpload(album);
}

//-----------------------------------------------------------------------------
QVariant UrlsModel::decorationData(const QModelIndex &index) const {
    if(
        static_cast<int>(m_items.size()) <= index.row()
        || index.column() != statusRow
    ) {
        return QVariant{};
    }

    switch(m_items[index.row()].status) {
    case ItemStatus::NullStatus: return m_grayIcon;
    case ItemStatus::DownloadedStatus: return m_downIcon;
    case ItemStatus::UploadedStatus: return m_upDownIcon;
    }

    return QVariant{};
}

//-----------------------------------------------------------------------------
QVariant UrlsModel::displayData(const QModelIndex &index) const {
    if(static_cast<int>(m_items.size()) <= index.row()) {
        return QVariant{};
    }

    switch(index.column()) {
    case filenameRow: return m_items[index.row()].filename.c_str();
    case downUrlRow: return m_items[index.row()].link.c_str();
    }

    return QVariant{};
}

//-----------------------------------------------------------------------------
void UrlsModel::updateItemStatus(iterator it, ItemStatus status) {
    int row = std::distance(m_items.begin(), it);
    it->status = status;

    emit dataChanged(index(row, 0), index(row, columnCount({})));
}

//-----------------------------------------------------------------------------
void UrlsModel::updateTaskStatus(ProcessType type) {
    ++m_completedTasks;    
    if(m_completedTasks == m_maxThreads) {
        auto status = (
            (type == ProcessType::DownloadProcess)
            ? ItemStatus::DownloadedStatus
            : ItemStatus::UploadedStatus
        );

        emit processComplete(
            type,
            std::all_of(m_items.begin(), m_items.end(), [status](const auto &item){
                return (item.status == status);
            })
        );
        m_completedTasks = 0;
    }
}

//-----------------------------------------------------------------------------
void UrlsModel::downloadTask(iterator begin, iterator end) {
    curl::CurlDownloader downloader;

    while(begin != end) {
        QString filename = m_workDir.filePath(begin->filename.c_str());

        downloader.download(begin->link);
        downloader.save(filename.toStdString());

        emit itemComplete(begin, ItemStatus::DownloadedStatus);
        std::advance(begin, 1);
    }

    emit taskComplete(ProcessType::DownloadProcess);
}

//-----------------------------------------------------------------------------
bool UrlsModel::startDownload() {
    auto pool = QThreadPool::globalInstance();

    m_completedTasks = 0;

    if((m_items.size() < m_maxThreads) || (m_maxThreads == 1)) {
        pool->start([this](){
            downloadTask(m_items.begin(), m_items.end());
        });
    } else {
        size_t count = m_items.size()/m_maxThreads;
        iterator cur = m_items.begin();
        for(int ii = 0; ii < (m_maxThreads - 1); ++ii) {
            pool->start([this, cur, count]() {
                downloadTask(cur, std::next(cur, count));
            });
            std::advance(cur, count);
        }
        pool->start([this, cur](){
            downloadTask(cur, m_items.end());
        });
    }

    emit processStart(ProcessType::DownloadProcess);
    return true;
}

//-----------------------------------------------------------------------------
bool UrlsModel::startUpload(const QString &album) {
    auto pool = QThreadPool::globalInstance();

    m_completedTasks = 0;

    if((m_items.size() < m_maxThreads) || (m_maxThreads == 1)) {
        pool->start([this, album](){
            uploadTask(album, m_items.begin(), m_items.end());
        });
    } else {
        size_t count = m_items.size()/m_maxThreads;
        iterator cur = m_items.begin();
        for(int ii = 0; ii < (m_maxThreads - 1); ++ii) {
            pool->start([this, album, cur, count]() {
                uploadTask(album, cur, std::next(cur, count));
            });
            std::advance(cur, count);
        }
        pool->start([this, album, cur](){
            uploadTask(album, cur, m_items.end());
        });
    }

    emit processStart(ProcessType::UploadProcess);
    return true;
}

//-----------------------------------------------------------------------------
void UrlsModel::uploadTask(const QString &album, iterator begin, iterator end) {
    ImageBan imageBan{"cvqx64FaleJ7dYclfNATYfWDWI2bQ0AqT2i"};

    while(begin != end) {
        QString filename = m_workDir.filePath(begin->filename.c_str());

        imageBan.uploadImage(album.toStdString(), filename.toStdString());

        emit itemComplete(begin, ItemStatus::UploadedStatus);
        std::advance(begin, 1);
    }

    emit taskComplete(ProcessType::UploadProcess);
}

//-----------------------------------------------------------------------------
std::string UrlsModel::uniqueFilename(
    const std::unordered_set<std::string> &filenamesSet,
    const std::string &filename
) {
    if(!filenamesSet.count(filename)) {
        return filename;
    }

    std::filesystem::path path{filename};

    for(size_t ii = 1; ;++ii) {
        auto newFilename = QString("%1_(%2)%3")
            .arg(path.stem().string().c_str())
            .arg(ii)
            .arg(path.extension().string().c_str());
        if(!filenamesSet.count(newFilename.toStdString())) {
            return newFilename.toStdString();
        }
    }
}
