#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("SrShadowy");
    QCoreApplication::setApplicationName("ProtonRun");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
