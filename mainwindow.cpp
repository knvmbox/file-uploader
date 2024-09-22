#include <QFileDialog>

#include <common/logger/loggerfactory.hpp>
#include <common/qtwidgets/plaintextlogger.hpp>

#include "mainwindow.hpp"
#include "parametersdlg.hpp"
#include "uploadimagesdlg.hpp"
#include "ui_mainwindow.h"


//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_logger{common::LoggerFactory::instance()}
    , m_urlsModel{new UrlsModel}
{
    ui->setupUi(this);

    common::LoggerFactory::setupLogger(new common::PlainTextLogger{ui->loggerEdit});

    ui->dirSelector->setMode(FileSelector::OpenDir);

    ui->urlsView->setModel(m_urlsModel.get());
    ui->urlsView->horizontalHeader()->resizeSection(0, 16);
    ui->urlsView->horizontalHeader()->resizeSection(1, 350);
    ui->urlsView->horizontalHeader()->resizeSection(2, 350);
    ui->urlsView->verticalHeader()->setDefaultSectionSize(24);

    connect(ui->openUrlsAction, SIGNAL(triggered(bool)), this, SLOT(openUrls()));
    connect(ui->exportUrlsAction, SIGNAL(triggered(bool)), this, SLOT(saveUrls()));
    connect(ui->paramsAction, SIGNAL(triggered(bool)), this, SLOT(openParams()));

    connect(ui->dirSelector, SIGNAL(fileSelected(const QString&)), this, SLOT(updateState()));
    connect(ui->downloadBtn, SIGNAL(clicked(bool)), this, SLOT(downloadFiles()));
    connect(ui->uploadBtn, SIGNAL(clicked(bool)), this, SLOT(uploadFiles()));

    connect(m_urlsModel.get(), &UrlsModel::processComplete, this, &MainWindow::processCompleted);
    connect(m_urlsModel.get(), &UrlsModel::processStart, this, &MainWindow::processStarted);
}

//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

//-----------------------------------------------------------------------------
void MainWindow::lockUi(bool state) {
    ui->openUrlsAction->setDisabled(state);
    ui->downloadBtn->setDisabled(state);
    ui->dirSelector->setDisabled(state);
    ui->uploadBtn->setDisabled(state);
}

//-----------------------------------------------------------------------------
void MainWindow::processCompleted(model::ProcessType type, bool status) {
    if(status) {
        auto text = (
            (type == model::ProcessType::DownloadProcess)
            ? "Загрузка изображений успешно завершена"
            : "Выгрузка изображений успешно завершена"
        );
        m_logger->info(text);
    } else {
        auto text = (
            (type == model::ProcessType::DownloadProcess)
            ? "Не удалось загрузить файлы"
            : "Не удалось выгрузить файлы"
        );
        m_logger->error(text);
    }

    lockUi(false);
}

//-----------------------------------------------------------------------------
void MainWindow::downloadFiles() {
    bool res = m_urlsModel->downloadImages(ui->dirSelector->filename());
    if(!res) {
        m_logger->error("Ошибка при запуске загрузки файлов");
    }
}

//-----------------------------------------------------------------------------
void MainWindow::openParams() {
    ParametersDlg dlg{this};
    dlg.exec();
}

//-----------------------------------------------------------------------------
void MainWindow::openUrls() {
    auto filename = QFileDialog::getOpenFileName(
        ui->centralwidget, windowTitle(), ".",
        "Text files (*.txt);;All files (*.*)"
    );

    m_urlsModel->openUrlsFile(filename);
    updateState();
}

//-----------------------------------------------------------------------------
void MainWindow::processStarted(model::ProcessType type) {
    QString text = (
        (type == model::ProcessType::DownloadProcess)
        ? "Загрузка изображений запущена"
        : "Выгрузка изображений запущена"
    );

    m_logger->info(text.toStdString());
    lockUi(true);
}

//-----------------------------------------------------------------------------
void MainWindow::saveUrls() {
    auto filename = QFileDialog::getSaveFileName(
        ui->centralwidget, windowTitle(), ".",
        "Text files (*.txt);;All files (*.*)"
    );

    m_urlsModel->saveUrlsFile(filename);
}

//-----------------------------------------------------------------------------
void MainWindow::updateState() {
    bool state = (m_urlsModel->rowCount() == 0) || ui->dirSelector->filename().isEmpty();
    ui->downloadBtn->setDisabled(state);
    ui->uploadBtn->setEnabled(m_urlsModel->canUpload());
}

//-----------------------------------------------------------------------------
void MainWindow::uploadFiles() {
    Settings settings;
    UploadImagesDlg dlg(settings.secretKey(), settings.thumbSecretKey(), this);

    auto res = dlg.exec();
    if(res == QDialog::Rejected) {
        return;
    }

    auto albums = dlg.album();
    auto isUploaded = m_urlsModel->uploadImages(
        albums.first.id.c_str(),
        albums.second.id.c_str()
    );
    if(!isUploaded) {
        m_logger->error("Ошибка при запуске выгрузки файлов");
    }
}
