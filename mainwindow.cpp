#include <QFileDialog>

#include "mainwindow.hpp"
#include "./ui_mainwindow.h"


//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_model{new UrlsModel}
{
    ui->setupUi(this);
    ui->dirSelector->setMode(FileSelector::OpenDir);
    ui->urlsView->setModel(m_model.get());

    connect(ui->dirSelector, SIGNAL(fileSelected(const QString&)), this, SLOT(updateState()));
    connect(ui->openUrlsAction, SIGNAL(triggered(bool)), this, SLOT(openUrls()));
    connect(ui->downloadBtn, SIGNAL(clicked(bool)), this, SLOT(uploadFiles()));

    connect(m_model.get(), SIGNAL(complete(bool)), this, SLOT(downloadComplete(bool)));
    connect(m_model.get(), SIGNAL(started()), this, SLOT(downloadStarted()));
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
}

//-----------------------------------------------------------------------------
void MainWindow::downloadComplete(bool status) {
    if(status) {
        ui->loggerEdit->appendPlainText("Загрузка изображений успешно завершена");
    } else {
        ui->loggerEdit->appendPlainText("Не удалось загрузить файлы");
    }

    lockUi(false);
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

    m_model->loadFile(filename);
}

//-----------------------------------------------------------------------------
void MainWindow::updateState() {
    bool state = (m_model->rowCount() == 0) || ui->dirSelector->filename().isEmpty();
    ui->downloadBtn->setDisabled(state);
}

//-----------------------------------------------------------------------------
void MainWindow::uploadFiles() {
    m_model->uploadFiles(ui->dirSelector->filename());
}
