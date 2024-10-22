#include <filesystem>
#include <iterator>

#include <QDebug>

#include <QBuffer>
#include <QColor>
#include <QIcon>
#include <QImage>
#include <QImageReader>
#include <QProcess>
#include <QRunnable>
#include <QTextStream>
#include <QThread>
#include <QThreadPool>
#include <QUrl>

#include <common/logger/loggerfactory.hpp>
#include <common/logger/loggerutils.hpp>

#include "../settings.hpp"
#include "../utils/commonutils.hpp"
#include "../utils/curlutils.hpp"
#include "../utils/imageban.hpp"
#include "../utils/webpdecoder.hpp"
#include "urlsmodel.hpp"


///////////////////////////////////////////////////////////////////////////////
static constexpr int statusRow = 0;
static constexpr int filenameRow = 1;
static constexpr int downUrlRow = 2;
static constexpr int upUrlRow = 3;

static const char* THUMBS_DIR = "thumbs";


///////////////////////////////////////////////////////////////////////////////
template <typename F>
struct PoolJob : public QRunnable {
    PoolJob(F f) :
        m_f{std::move(f)},
        m_logger{common::LoggerFactory::instance()} {
    }

    void run() override {
        m_logger->info("Job started!");
        std::invoke(m_f);
    }

    F m_f;
    std::shared_ptr<common::Logger> m_logger;
};

//-----------------------------------------------------------------------------
UrlsModel::UrlsModel(QObject *parent) :
    AbstractModel{parent},
    m_logger{common::LoggerFactory::instance()},
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
            {filename, line.toStdString(), "", "", model::ItemStatus::NullStatus}
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
        in <<item.bbcode.c_str() <<"\n";
    }

    return true;
}

//-----------------------------------------------------------------------------
bool UrlsModel::uploadImages(params::UploadParams params) {
    if(params.isSave) {
        bool isOk = utils::checkDir(params.dirPath.c_str());
        if(!isOk) {
            return false;
        }
    }

    auto pool = QThreadPool::globalInstance();

    m_completedTasks = 0;

    if((m_items.size() < m_maxThreads) || (m_maxThreads == 1)) {
        m_maxThreads = 1;
        pool->start(new PoolJob([this, params](){
            uploadTask(1, std::move(params), m_items.begin(), m_items.end());
        }));
    } else {
        m_maxThreads = static_cast<size_t>(QThread::idealThreadCount());
        size_t count = m_items.size()/m_maxThreads;

        auto cur = m_items.begin();
        for(int ii = 0; ii < (m_maxThreads - 1); ++ii) {
            pool->start(new PoolJob([this, params, cur, count, ii]() {
                uploadTask(ii + 1, std::move(params), cur, std::next(cur, count));
            }));
            std::advance(cur, count);
        }
        pool->start(new PoolJob([this, params, cur](){
            uploadTask(m_maxThreads + 5, std::move(params), cur, m_items.end());
        }));
    }

    emit processStart(model::ProcessType::UploadProcess);
    return true;
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
    case upUrlRow: return m_items[index.row()].bbcode.c_str();
    }

    return QVariant{};
}

//-----------------------------------------------------------------------------
void UrlsModel::updateItemStatus(model::iterator it, model::Item item) {
    int row = std::distance(m_items.begin(), it);

    it->filename = std::move(item.filename);
    it->upLink = item.upLink;
    it->bbcode = utils::createBbCode(item.upLink);
    it->status = item.status;

    emit dataChanged(index(row, 0), index(row, columnCount({})));
}

//-----------------------------------------------------------------------------
void UrlsModel::updateTaskStatus(model::ProcessType type) {
    ++m_completedTasks;

    common::log::info(m_logger, "Завершена задача", m_completedTasks, "из", m_maxThreads);

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
std::vector<char> UrlsModel::resizeImage(
    const void *data, size_t dataSize, size_t newSize
) {
    QByteArray array = QByteArray::fromRawData(static_cast<const char*>(data), dataSize);
    QBuffer imgBuffer{&array};
    QImageReader imgReader{&imgBuffer};
    QByteArray format = imgReader.format();
    std::string imgFormat{format.data()};
    QImage image = imgReader.read();

    if(image.isNull()) {
        return {};
    }

    int size = static_cast<int>(newSize);
    int width = image.width();

    QImage scaledImage = (width > size) ? image.scaledToWidth(size) : std::move(image);

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    bool isOk = scaledImage.save(&buffer, imgFormat.c_str());
    if(!isOk) {
        return {};
    }

    std::vector<char> v(ba.size());
    memcpy(v.data(), ba.data(), ba.size());

    return v;
}

//-----------------------------------------------------------------------------
void UrlsModel::uploadTask(
    int id, params::UploadParams params, model::iterator begin, model::iterator end
) {
    Settings settings;
    curl::CurlDownloader downloader;
    imageban::ImageBan imageBan{std::move(params.secretKey)};

    try {
        while(begin != end) {
            auto item = *begin;

            downloader.download(item.downLink);
            item.status = model::ItemStatus::DownloadedStatus;

            std::vector<char> data;
            bool isWebP = utils::isWebpImage(item.filename);
            if(isWebP) {
                WebpDecoder decoder(
                    reinterpret_cast<const uint8_t*>(downloader.data()), downloader.size()
                );
                item.filename = utils::replaceExt(item.filename, "png");

                auto pngData = decoder.decode();
                data = resizeImage(pngData.data(), pngData.size(), settings.imageSize());

            } else {
                data = resizeImage(downloader.data(), downloader.size(), settings.imageSize());
            }

            if(data.size()) {
                auto image = imageBan.uploadImage(
                    params.album.id, item.filename, std::move(data)
                );

                item.status = model::ItemStatus::UploadedStatus;
                item.upLink = image.link;
            }

            emit itemComplete(begin, item);
            std::advance(begin, 1);
        }
    } catch(curl::curl_error &e) {
        m_logger->error(e.what());
    }

    emit taskComplete(model::ProcessType::UploadProcess);
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
