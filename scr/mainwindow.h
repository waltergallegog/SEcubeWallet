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

#include "../securesqlite3/sqlite3.h"


//Miscellaneous
#include <QTableView>
#include <QObject>
#include <QFile>
#include <QDebug>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QScrollBar>
#include "mysortfilterproxymodel.h"
#include "columnalignedlayout.h"

static int c_callback_createTableList(void *mainwindowP, int argc, char **argv, char **azColName);
static int c_callback_populateTable(void *mainwindowP, int argc, char **argv, char **azColName);

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

    void on_action_New_Wallet_triggered();
    void invalidateAlignedLayout();



    void on_action_Add_Folder_triggered();

private:
    Ui::MainWindow *ui;

    // SeCube related
    se3_session s;              // session variable
    SEFILE_FHANDLE sefile_file; // Encrypted file

    // Database related
    QSqlDatabase dbMem;        // The database
    QSqlTableModel *model;  // Model to handle tables in the database easily
    MySortFilterProxyModel *proxyModel;
    QString path, fileName; // To store database filename
    QStringList tableList;
    QString currentTable;
    QSqlQuery query;

    sqlite3 *dbSec;

    helpWindow *help;

    /// ****** GUI elements ****
    ColumnAlignedLayout *alignedLayout;
    QWidget* dateWidget;
    QHBoxLayout* dateLayout;
    QLineEdit *idFilter;
    QLineEdit *userFilter;
    QLineEdit *domFilter;
    QLineEdit *passFilter;
    QLineEdit *dateFilter;
    QComboBox *dateUnit;
    QLineEdit *descFilter;

    QComboBox *walletList;

    //// ***** Methods
    void init();           //initialization. Call LoginDialog and configure UI
    bool OpenDataBase();   //Create/Open Data base and create table, connections
    void CreateViewTable(const QString &WalletName);//Create the table model and display the data in the UI.

    int callback_createTableList(int argc, char **argv, char **azColName); //Build TableList from ciphered db
    int callback_populateTable(int argc, char **argv, char **azColName); //Build TableList from ciphered db
    friend int c_callback_populateTable(void *mainwindowP, int argc, char **argv, char **azColName);
    friend int c_callback_createTableList(void *mainwindowP, int argc, char **argv, char **azColName);
};

#endif // MAINWINDOW_H
