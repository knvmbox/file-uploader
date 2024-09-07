#include <filesystem>
#include <iterator>

#include <QDebug>

#include <QColor>
#include <QIcon>
#include <QProcess>
#include <QRunnable>
#include <QTextStream>
#include <QThread>
#include <QThreadPool>
#include <QUrl>

#include <fmt/core.h>

#include "../settings.hpp"
#include "../utils/curlutils.hpp"
#include "../utils/imageban.hpp"
#include "../utils/webpdecoder.hpp"
#include "urlsmodel.hpp"


///////////////////////////////////////////////////////////////////////////////
static constexpr int statusRow = 0;
static constexpr int filenameRow = 1;
static constexpr int downUrlRow = 2;
static constexpr int upUrlRow = 3;

///////////////////////////////////////////////////////////////////////////////
template <typename F>
struct PoolJob : public QRunnable {
    PoolJob(F f) : m_f{std::move(f)} {
    }

    void run() override {
        std::invoke(m_f);
    }

    F m_f;
};

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
    addColumn("Uploadload URL");

    qRegisterMetaType<model::iterator>();
    qRegisterMetaType<model::Item>();
    qRegisterMetaType<model::ProcessType>();
    qRegisterMetaType<imageban::image_t>();

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
        if(line.isEmpty())
            continue;

        QUrl url{line};
        auto filename = url.fileName().toStdString();

        if(filenamesSet.count(filename)) {
            filename = uniqueFilename(filenamesSet, filename);
        }

        m_items.push_back(
            {filename, line.toStdString(), "", model::ItemStatus::NullStatus}
        );
        filenamesSet.insert(filename);
    }
    endResetModel();

    return true;
}

//-----------------------------------------------------------------------------
bool UrlsModel::saveUrlsFile(const QString &filename) {
    QFile urlsFile(filename);
    if (!urlsFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream in(&urlsFile);
    for(const auto &item : m_items) {
        in <<item.upLink.c_str() <<"\n";
    }

    return true;
}

//-----------------------------------------------------------------------------
bool UrlsModel::uploadImages(const QString &album) {
    bool isExist = m_workDir.exists();
    auto files = m_workDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
    bool existAll = std::all_of(
        m_items.begin(), m_items.end(), [&files](const model::Item &file) {
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
    case model::ItemStatus::NullStatus: return m_grayIcon;
    case model::ItemStatus::DownloadedStatus: return m_downIcon;
    case model::ItemStatus::UploadedStatus: return m_upDownIcon;
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
    case downUrlRow: return m_items[index.row()].downLink.c_str();
    case upUrlRow: return m_items[index.row()].upLink.c_str();
    }

    return QVariant{};
}

//-----------------------------------------------------------------------------
void UrlsModel::updateItemStatus(model::iterator it, model::Item item) {
    int row = std::distance(m_items.begin(), it);

    it->filename = std::move(item.filename);
    it->upLink = createBbCodeAsText(std::move(item.upLink));
    it->status = item.status;

    emit dataChanged(index(row, 0), index(row, columnCount({})));
}

//-----------------------------------------------------------------------------
void UrlsModel::updateTaskStatus(model::ProcessType type, bool) {
    ++m_completedTasks;    
    if(m_completedTasks == m_maxThreads) {
        auto status = (
            (type == model::ProcessType::DownloadProcess)
            ? model::ItemStatus::DownloadedStatus
            : model::ItemStatus::UploadedStatus
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
std::string UrlsModel::createBbCode(std::string link) {
    constexpr const char* IMAGEBAN_URL = "imageban.ru";
    constexpr const char * const OUT_WORD = "out";
    constexpr const char * const THUMBS_WORD = "thumbs";
    constexpr size_t YEAR_COUNT = 4;
    constexpr size_t MONTH_COUNT = 2;


    std::string url{link};

    if(link.empty()) {
        return link;
    }

    auto pos = url.find(IMAGEBAN_URL);
    if(pos == std::string::npos) {
        throw std::out_of_range("Can't find 'imageban.ru' URL!");
    }
    url.erase(8, pos - 8);

    pos = url.find(OUT_WORD);
    if(pos == std::string::npos) {
        throw std::out_of_range("Can't find 'out' word!");
    }
    url.erase(pos, std::strlen(OUT_WORD));
    url.insert(pos, "show");

    pos = url.find_last_of(".");
    if(pos == std::string::npos) {
        throw std::out_of_range("Can't find extension dot!");
    }
    url.erase(pos, 1);
    url.insert(pos, "/");

    pos = link.find(OUT_WORD);
    if(pos == std::string::npos) {
        throw std::out_of_range("Can't find 'out' word!");
    }
    link.erase(pos, std::strlen(OUT_WORD));
    link.insert(pos, THUMBS_WORD);

    pos = link.find(THUMBS_WORD);
    if(pos == std::string::npos) {
        throw std::out_of_range("Can't find 'thumbs' word!");
    }

    pos += std::strlen(THUMBS_WORD) + 1 + YEAR_COUNT;
    if(pos >= link.size()) {
        throw std::out_of_range("Can't find year digits!");
    }
    link.erase(pos, 1);
    link.insert(pos, ".");

    pos += 1 + MONTH_COUNT;
    if(pos >= link.size()) {
        throw std::out_of_range("Can't find month digits!");
    }
    link.erase(pos, 1);
    link.insert(pos, ".");

    return fmt::format("[URL={0}][IMG]{1}[/IMG][/URL]", url, link);
}

//-----------------------------------------------------------------------------
std::string UrlsModel::createBbCodeAsText(std::string link) {
    if(link.empty()) {
        return {};
    }

    return fmt::format(
        "[URL=https://imageban.ru][IMG]{}[/IMG][/URL]", link
    );
}

//-----------------------------------------------------------------------------
void UrlsModel::downloadTask(model::iterator begin, model::iterator end) {
    curl::CurlDownloader downloader;

    try {
        while(begin != end) {
            auto item = *begin;
            QString filename = m_workDir.filePath(item.filename.c_str());

            downloader.download(item.downLink);
            item.status = model::ItemStatus::DownloadedStatus;

            if(isWebpImage(item.filename)) {
                WebpDecoder decoder(
                    reinterpret_cast<const uint8_t*>(downloader.data()), downloader.size()
                );
                decoder.save(replaceExt(filename.toStdString(), "png"));
                item.filename = replaceExt(item.filename, "png");
            } else {
                downloader.save(filename.toStdString());
            }

            emit itemComplete(begin, item);
            std::advance(begin, 1);
        }

        emit taskComplete(model::ProcessType::DownloadProcess, true);

    } catch(curl::curl_error &e) {
        emit taskComplete(model::ProcessType::DownloadProcess, false);
    }

}

//-----------------------------------------------------------------------------
bool UrlsModel::isWebpImage(const std::string &filename) {
    std::filesystem::path path{filename};

    std::string stem = path.extension();
    std::transform(stem.begin(), stem.end(), stem.begin(), ::toupper);

    return (stem == ".WEBP");
}

//-----------------------------------------------------------------------------
std::string UrlsModel::replaceExt(const std::string &filename, const std::string &ext) {
    std::filesystem::path path{filename};
    path.replace_extension("." + ext);
    return path;
}

//-----------------------------------------------------------------------------
bool UrlsModel::startDownload() {
    auto pool = QThreadPool::globalInstance();

    m_completedTasks = 0;

    if((m_items.size() < m_maxThreads) || (m_maxThreads == 1)) {
        pool->start(new PoolJob([this](){
            downloadTask(m_items.begin(), m_items.end());
        }));
    } else {
        size_t count = m_items.size()/m_maxThreads;
        model::iterator cur = m_items.begin();
        for(int ii = 0; ii < (m_maxThreads - 1); ++ii) {
            pool->start(new PoolJob([this, cur, count]() {
                downloadTask(cur, std::next(cur, count));
            }));
            std::advance(cur, count);
        }
        pool->start(new PoolJob([this, cur](){
            downloadTask(cur, m_items.end());
        }));
    }

    emit processStart(model::ProcessType::DownloadProcess);
    return true;
}

//-----------------------------------------------------------------------------
bool UrlsModel::startUpload(const QString &album) {
    auto pool = QThreadPool::globalInstance();

    m_completedTasks = 0;

    if((m_items.size() < m_maxThreads) || (m_maxThreads == 1)) {
        pool->start(new PoolJob([this, album](){
            uploadTask(album, m_items.begin(), m_items.end());
        }));
    } else {
        size_t count = m_items.size()/m_maxThreads;
        auto cur = m_items.begin();
        for(int ii = 0; ii < (m_maxThreads - 1); ++ii) {
            pool->start(new PoolJob([this, album, cur, count]() {
                uploadTask(album, cur, std::next(cur, count));
            }));
            std::advance(cur, count);
        }
        pool->start(new PoolJob([this, album, cur](){
            uploadTask(album, cur, m_items.end());
        }));
    }

    emit processStart(model::ProcessType::UploadProcess);
    return true;
}

//-----------------------------------------------------------------------------
void UrlsModel::uploadTask(const QString &album, model::iterator begin, model::iterator end) {
    Settings settings;
    imageban::ImageBan imageBan{settings.secretKey()};

    try {
        while(begin != end) {
            auto item = *begin;
            QString filename = m_workDir.filePath(item.filename.c_str());

            auto image = imageBan.uploadImage(album.toStdString(), filename.toStdString());
            item.status = model::ItemStatus::UploadedStatus;
            item.upLink = image.link;

            emit itemComplete(begin, item);
            std::advance(begin, 1);
        }
    } catch(imageban::imageban_error &e) {
        emit taskComplete(model::ProcessType::UploadProcess, false);
    }

    emit taskComplete(model::ProcessType::UploadProcess, true);
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
