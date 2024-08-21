#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>

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
    void downloadComplete(bool);
    void downloadFiles();
    void downloadStarted();
    void openUrls();
    void updateState();

private:
    Ui::MainWindow *ui;

    std::unique_ptr<UrlsModel> m_urlsModel;
};
#endif // MAINWINDOW_HPP
