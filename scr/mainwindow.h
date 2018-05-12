#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#define IDENT_COL   0
#define USER_COL    1
#define DOM_COL     2
#define PASS_COL    3
#define DATE_COL    4
#define DESC_COL    5

#include <QMainWindow>

//other windows, dialogs
#include "logindialog.h"
#include "securefiledialog.h"
#include "addentry.h"
#include "environmentdialog.h"
#include "deleteconfirmation.h"
#include "helpwindow.h"
#include "newtable.h"
#include "saveconfirmation.h"
#include "overwritedialog.h"


//SEcure related
#include "SEfile.h"
#include "se3/L1.h"

//Sqlite related
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>

#include <QSharedMemory>


#include "../securesqlite3/sqlite3.h"

//Miscellaneous
#include <QObject>
#include <QFile>
#include <QDebug>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QScrollBar>
#include <QCloseEvent>

#include "mysortfilterproxymodel.h"
#include "filtersaligned.h"
#include "passworditemdelegate.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int callback_createTableList(int argc, char **argv, char **azColName); //Build TableList from ciphered db
    int callback_populateTable(int argc, char **argv, char **azColName); //Build TableList from ciphered db

private slots:
    void on_action_New_Wallet_triggered();
    void on_action_Open_Wallet_triggered();
    bool on_action_Save_Wallet_triggered();
    void on_action_Delete_Wallet_triggered();
    void on_action_Close_Wallet_triggered();


    void on_action_Add_Table_triggered();
    void on_action_Delete_Table_triggered();

    void on_action_Add_Entry_triggered();
    void on_action_Edit_Entry_triggered();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_action_Delete_Entry_triggered();
    void on_action_Show_Passwords_toggled(bool show);
    void on_action_Fit_Table_triggered();
    void on_action_Launch_Domain_triggered();

    void on_action_Set_Environment_triggered();
    void on_action_About_triggered();

    void DomainFilter_textChanged(const QString &arg1);
    void UserFilter_textChanged(const QString &arg1);
    void PassFilter_textChanged(const QString &arg1);
    void DescFilter_textChanged(const QString &arg1);
    void dateUnit_currentIndexChanged(int index);
    void dateFilter_textChanged(const QString &arg1);

    void invalidateAlignedLayout();
    void tableList_currentIndexChanged(const QString &arg1);


    void on_action_Save_Wallet_As_triggered();

private:
    Ui::MainWindow *ui;

    // SeCube related
    se3_session* s;              // session variable
    SEFILE_FHANDLE sefile_file; // Encrypted file

    // Database related
    QSqlDatabase dbMem;        // The database
    QSqlQuery query;
    QSqlTableModel *model;  // Model to handle tables in the database easily
    MySortFilterProxyModel *proxyModel;
    QStringList tables;
    QString currentTable;
    QString path, fileName; // To store database filename
    QString saveAsAbort=NULL;
    QString walletName;

    sqlite3 *dbSec;

    //bool dirty; // true if in-memory database has been modified and needs to be saved.

    /// ****** GUI elements ****
    FiltersAligned *filters;
    QComboBox *tableList;
    QLabel* displayWalletName;
    PasswordItemDelegate * passDelegate=new PasswordItemDelegate(this);
    int widths [DESC_COL-USER_COL+1] = {0};

    //// ***** Methods
    void init();           //initialization. Call LoginDialog and configure UI
    bool OpenDataBase();   //Create/Open Data base and create table, connections
    void createTableView(const QString &tableName);//Create the table model and display the data in the UI.
    void UpdateTableView(const QString &tableName);
    void setAllEnabled(bool enabled);
    QSharedMemory shaMemSession, shaMemReq, shaMemRes, shaMemDevBuf;


protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
