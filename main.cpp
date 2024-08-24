#include <QApplication>

#include "mainwindow.hpp"
#include "utils/curlutils.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    curl::curlInit();

    w.show();
    auto res = a.exec();

    curl::curlCleanup();

    return res;
}
