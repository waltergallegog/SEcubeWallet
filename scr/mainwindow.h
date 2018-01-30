#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

//other windows, dialogs
#include "logindialog.h"
#include "securefiledialog.h"

//SEcure related
#include "SEfile.h"
#include "se3/L1.h"

//Sqlite related
//#include "dbmanager.h"

#include <QTableView>

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QFile>




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_NewWallet_clicked();

    void on_AddEntry_clicked();

    void on_CipherClose_clicked();

    void on_OpenCyphered_clicked();

private:
    Ui::MainWindow *ui;

    QString path, fileName;
    se3_session s; // session variable
    QSqlTableModel *model;

    SEFILE_FHANDLE sefile_file;
    se3_key *keyTable;
    se3_algo *algTable;

    QSqlDatabase db;


    //Methods
     void init(); //initialization. Call LoginDialog
     void CreateTable();
     void OpenDataBase();

};

#endif // MAINWINDOW_H
