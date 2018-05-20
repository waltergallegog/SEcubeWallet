#include "mainwindow.h"
#include "sessionwrapper.h"
#include <QApplication>
#include <QDebug>
#include "myapplication.h"

int main(int argc, char *argv[]){
    QApplication::setDesktopSettingsAware(false);
    QApplication::setStyle("QtCurve");
    QApplication a(argc, argv);//    MyApplication a(argc, argv);



    int n = 0;
    if (argc > 1) n = QByteArray(argv[1]).toInt();

    MainWindow wallet;
    SessionWrapper session(n+1);


    if(n==0){
        session.init();
    }
    else if (n==1){
        wallet.show();
        wallet.init();
    }

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
