#include <QColor>
#include <QProcess>
#include <QTextStream>
#include <QUrl>

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
QVariant UrlsModel::foregroundColor(const QModelIndex &index) const {
    if(static_cast<int>(m_items.size()) <= index.row()) {
        return QVariant{};
    }

    return (
        m_duplicates.count(m_items[index.row()].filename)
        ? QColor(255,0,0)
        : QColor(0,0,0)
    );
}

//-----------------------------------------------------------------------------
bool UrlsModel::loadFile(const QString &filename) {
    QFile urlsFile(filename);
    if (!urlsFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    beginResetModel();
    clearModel();

    std::unordered_set<std::string> filenames;

    QTextStream in(&urlsFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QUrl url{line};

        auto filename = url.fileName().toStdString();

        m_items.push_back(
            {filename, line.toStdString(), false}
        );

        if(filenames.count(filename)) {
            m_duplicates.insert(filename);
        } else {
            filenames.insert(filename);
        }
    }
    endResetModel();

    return true;
}

//-----------------------------------------------------------------------------
bool UrlsModel::uploadFiles(const QString &dir) {
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
