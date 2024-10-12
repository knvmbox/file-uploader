#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>

#include <common/logger/logger.hpp>

#include "model/urlsmodel.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void lockUi(bool);

private slots:
    void doWork();
    void openParams();
    void openUrls();
    void processCompleted(model::ProcessType,bool);
    void processStarted(model::ProcessType);
    void saveUrls();
    void updateState();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<common::Logger> m_logger;
    std::unique_ptr<UrlsModel> m_urlsModel;
};
#endif // MAINWINDOW_HPP
