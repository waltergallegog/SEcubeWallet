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
#include <QObject>
#include <QFile>
#include <QDebug>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QScrollBar>
#include "mysortfilterproxymodel.h"
#include "columnalignedlayout.h"
#include "passworditemdelegate.h"

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
    void on_action_New_Wallet_triggered();
    void on_action_Add_Folder_triggered();
    void on_action_Add_Entry_triggered();
    void on_action_Show_Passwords_toggled(bool show);
    void on_action_Edit_Entry_triggered();
    void on_walletView_doubleClicked(const QModelIndex &index);
    void on_action_Delete_Entry_triggered();
    void on_action_Launch_Domain_triggered();
    void on_action_Set_Environment_triggered();
    void on_action_Save_Wallet_triggered();
    void on_action_Open_Wallet_triggered();
    void on_action_About_triggered();

    void DomainFilter_textChanged(const QString &arg1);
    void UserFilter_textChanged(const QString &arg1);
    void PassFilter_textChanged(const QString &arg1);
    void DescFilter_textChanged(const QString &arg1);
    void dateUnit_currentIndexChanged(int index);
    void dateFilter_textChanged(const QString &arg1);

    void invalidateAlignedLayout();
    void walletList_currentIndexChanged(const QString &arg1);

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

    PasswordItemDelegate * passDelegate=new PasswordItemDelegate(this);

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
