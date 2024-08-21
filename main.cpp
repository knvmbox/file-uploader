#include <QApplication>

#include "mainwindow.hpp"
#include "utils/curldownloader.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    CurlDownloader::curlInit();

    w.show();
    auto res = a.exec();

    CurlDownloader::curlCleanup();

    return res;
}
