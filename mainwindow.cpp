#include <QFileDialog>

#include "mainwindow.hpp"
#include "./ui_mainwindow.h"


//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_urlsModel{new UrlsModel}
{
    ui->setupUi(this);

    ui->dirSelector->setMode(FileSelector::OpenDir);
    ui->urlsView->setModel(m_urlsModel.get());

    connect(ui->dirSelector, SIGNAL(fileSelected(const QString&)), this, SLOT(updateState()));
    connect(ui->openUrlsAction, SIGNAL(triggered(bool)), this, SLOT(openUrls()));
    connect(ui->downloadBtn, SIGNAL(clicked(bool)), this, SLOT(downloadFiles()));

    connect(m_urlsModel.get(), SIGNAL(complete(bool)), this, SLOT(downloadComplete(bool)));
    connect(m_urlsModel.get(), SIGNAL(started()), this, SLOT(downloadStarted()));
}

//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

//-----------------------------------------------------------------------------
void MainWindow::error(const QString &str) {
    QString text = QString("[ERROR] %1").arg(str);
    ui->loggerEdit->appendPlainText(text);
}

//-----------------------------------------------------------------------------
void MainWindow::lockUi(bool state) {
    ui->openUrlsAction->setDisabled(state);
    ui->downloadBtn->setDisabled(state);
    ui->dirSelector->setDisabled(state);
}

//-----------------------------------------------------------------------------
void MainWindow::downloadComplete(bool status) {
    if(status) {
        ui->loggerEdit->appendPlainText("Загрузка изображений успешно завершена");
    } else {
        error("Не удалось загрузить файлы");
    }

    lockUi(false);
}

//-----------------------------------------------------------------------------
void MainWindow::downloadFiles() {
    bool res = m_urlsModel->downloadImages(ui->dirSelector->filename());
    if(!res) {
        error("Ошибка при запуске загрузки файлов");
    }
}

//-----------------------------------------------------------------------------
void MainWindow::downloadStarted() {
    ui->loggerEdit->appendPlainText("Загрузка изображений запущена");
    lockUi(true);
}

//-----------------------------------------------------------------------------
void MainWindow::openUrls() {
    auto filename = QFileDialog::getOpenFileName(
        ui->centralwidget, windowTitle(), ".",
        "Text files (*.txt);;All files (*.*)"
    );

    m_urlsModel->openUrlsFile(filename);
}

//-----------------------------------------------------------------------------
void MainWindow::updateState() {
    bool state = (m_urlsModel->rowCount() == 0) || ui->dirSelector->filename().isEmpty();
    ui->downloadBtn->setDisabled(state);
}

