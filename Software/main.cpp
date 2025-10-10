#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    QString strLanPath = QCoreApplication::applicationDirPath()  + "MDflasher_en.qm";
//    QTranslator trans;
//    trans.load(strLanPath);
//    qApp->installTranslator(&trans);
    w.show();
    return a.exec();
}
