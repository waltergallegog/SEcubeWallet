#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "myapplication.h"

int main(int argc, char *argv[]){
//    QApplication::setDesktopSettingsAware(false);
//    QApplication::setStyle("QtCurve");
    QApplication  a(argc, argv);//MyApplication a(argc, argv);

    MainWindow w;
    qWarning()<<"start the app";
    w.show();

    return a.exec();

//    try {
//        a.exec();
//    } catch (...) {
//        // clean up here, e.g. save the session
//        // and close all config files.
//        qDebug() <<"Something went wrong";
//        return 0; // exit the application
//    }

//    qDebug() <<"everything ok";
//    return 0;
}
