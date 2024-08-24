#include <filesystem>
#include <iterator>

#include <QDebug>

#include <QColor>
#include <QProcess>
#include <QTextStream>
#include <QThread>
#include <QThreadPool>
#include <QUrl>

#include "../utils/curlutils.hpp"
#include "urlsmodel.hpp"


//-----------------------------------------------------------------------------
UrlsModel::UrlsModel(QObject *parent) :
    AbstractModel{parent},
    m_completedTasks{0},
    m_maxThreads{static_cast<size_t>(QThread::idealThreadCount())} {
    addColumn("Filename");
    addColumn("Download URL");
    addColumn("Status");

    qRegisterMetaType<iterator>();

    connect(this, &UrlsModel::itemComplete, this, &UrlsModel::updateItemStatus);
    connect(this, &UrlsModel::taskComplete, this, &UrlsModel::updateTaskStatus);
}

//-----------------------------------------------------------------------------
QVariant UrlsModel::displayData(const QModelIndex &index) const {
    if(static_cast<int>(m_items.size()) <= index.row()) {
        return QVariant{};
    }

    switch(index.column()) {
    case 0: return m_items[index.row()].filename.c_str();
    case 1: return m_items[index.row()].link.c_str();
    case 2: return m_items[index.row()].status;
    }

    return QVariant{};
}

//-----------------------------------------------------------------------------
bool UrlsModel::downloadImages(const QString &dir) {
    QDir workDir{dir};

    bool isExist = workDir.exists();
    bool isEmpty = workDir.entryList(QDir::NoDotAndDotDot | QDir::Files).isEmpty();
    if(!isExist || !isEmpty) {
        return false;
    }

    return startDownload(dir);
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
            {filename, line.toStdString(), false}
        );
        filenamesSet.insert(filename);
    }
    endResetModel();

    return true;
}

//-----------------------------------------------------------------------------
void UrlsModel::updateItemStatus(iterator it, bool status) {
    int row = std::distance(m_items.begin(), it);
    it->status = status;

    emit dataChanged(index(row, 0), index(row, columnCount({})));
}

//-----------------------------------------------------------------------------
void UrlsModel::updateTaskStatus() {
    ++m_completedTasks;
    if(m_completedTasks == m_maxThreads) {
        emit downloadComplete(
            std::all_of(m_items.begin(), m_items.end(), [](const auto &item){
                return item.status;
            })
        );
        m_completedTasks = 0;
    }
}

//-----------------------------------------------------------------------------
void UrlsModel::downloadTask(const QString &path, iterator begin, iterator end) {
    curl::CurlDownloader downloader;

    while(begin != end) {
        QDir dir{path};
        QString filename = dir.filePath(begin->filename.c_str());

        downloader.download(begin->link);
        downloader.save(filename.toStdString());

        emit itemComplete(begin, true);
        std::advance(begin, 1);
    }

    emit taskComplete();
}

//-----------------------------------------------------------------------------
bool UrlsModel::startDownload(const QString &dir) {
    auto pool = QThreadPool::globalInstance();

    m_completedTasks = 0;

    if((m_items.size() < m_maxThreads) || (m_maxThreads == 1)) {
        pool->start([this, dir](){
            downloadTask(dir, m_items.begin(), m_items.end());
        });
    } else {
        size_t count = m_items.size()/m_maxThreads;
        iterator cur = m_items.begin();
        for(int ii = 0; ii < (m_maxThreads - 1); ++ii) {
            pool->start([this, dir, cur, count]() {
                downloadTask(dir, cur, std::next(cur, count));
            });
            std::advance(cur, count);
        }
        pool->start([this, dir, cur](){
            downloadTask(dir, cur, m_items.end());
        });
    }

    emit downloadStart();
    return true;
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
