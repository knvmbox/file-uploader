#include <filesystem>

#include <QColor>
#include <QProcess>
#include <QTextStream>
#include <QUrl>

#include "../utils/string_format.hpp"
#include "urlsmodel.hpp"


//-----------------------------------------------------------------------------
UrlsModel::UrlsModel(QObject *parent)
    : AbstractModel{parent} {
    addColumn("Filename");
    addColumn("URL");
    addColumn("Status");
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
    QFile tmpFile(workDir.filePath("imgs.url"));
    tmpFile.open(QIODevice::WriteOnly);

    saveUrls(tmpFile);

    QString filepath = tmpFile.fileName();
    tmpFile.close();

    createProcess(workDir, filepath);
    m_process->start();
    emit started();

    return true;
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
void UrlsModel::createProcess(const QDir &dir, const QString &filename) {
    QStringList args;
    args <<"-i" <<filename;

    m_process.reset(new QProcess);
    m_process->setProgram("wget");
    m_process->setWorkingDirectory(dir.path());
    m_process->setArguments(args);
    m_process->setReadChannel(QProcess::StandardError);

    connect(
        m_process.get(),
        QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus) {
            emit complete(exitStatus == QProcess::NormalExit);
            m_process.reset();

            QFile tmpFile(dir.filePath(filename));
            tmpFile.remove();
    });

}

//-----------------------------------------------------------------------------
void UrlsModel::saveUrls(QFile &file) {
    QTextStream stream{&file};

    for(const auto& item : m_items) {
        stream <<item.link.c_str() <<'\n';
    }
}

//-----------------------------------------------------------------------------
std::string UrlsModel::uniqueFilename(
    const std::unordered_set<std::string> &filenamesSet,
    const std::string &filename
) {
    using cdba::sec21::format;

    if(!filenamesSet.count(filename)) {
        return filename;
    }

    std::filesystem::path path{filename};

    for(size_t ii = 1; ;++ii) {
        auto newFilename = format("%1_(%2)%3")
            .arg(path.stem().string()).arg(ii).arg(path.extension().string());
        if(!filenamesSet.count(newFilename)) {
            return newFilename;
        }
    }
}
