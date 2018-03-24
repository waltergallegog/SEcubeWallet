#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

//other windows, dialogs
#include "logindialog.h"
#include "securefiledialog.h"
#include "addentry.h"
#include "environmentdialog.h"
#include "deleteconfirmation.h"
#include "helpwindow.h"
#include "newtable.h"

//SEcure related
#include "SEfile.h"
#include "se3/L1.h"

//Sqlite related
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>

//Miscellaneous
#include <QTableView>
#include <QObject>
#include <QFile>
#include <QDebug>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include "mysortfilterproxymodel.h"


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
    void on_DeleteEntry_clicked();
    void on_LaunchEntry_clicked();
    void on_EnvironmentBut_clicked();
    void on_EditEntry_clicked();
    void on_Showpass_toggled(bool checked);
    //void onTableClicked(const QModelIndex &index);
    void on_DomainFilter_textChanged(const QString &arg1);
    void on_UserFilter_textChanged(const QString &arg1);
    void on_DescFilter_textChanged(const QString &arg1);
    void on_WalletView_doubleClicked(const QModelIndex &index);
    void on_Months_currentIndexChanged(int index);
    void on_CustomMonths_textChanged(const QString &arg1);
    void on_Help_clicked();

    void on_NewTable_clicked();

    void on_WalletList_currentIndexChanged(const QString &arg1);

    void on_DeleteWallet_clicked();

private:
    Ui::MainWindow *ui;

    // SeCube related
    se3_session s;              // session variable
    SEFILE_FHANDLE sefile_file; // Encrypted file

    // Database related
    QSqlDatabase db;        // The database
    QSqlTableModel *model;  // Model to handle tables in the database easily
    MySortFilterProxyModel *proxyModel;
    QString path, fileName; // To store database filename

    helpWindow *help;

    //Methods
     void init();           //initialization. Call LoginDialog and configure UI
     bool OpenDataBase();   //Create/Open Data base and create table, connections
     void CreateViewTable(const QString &WalletName);//Create the table model and display the data in the UI.
};

#endif // MAINWINDOW_H
