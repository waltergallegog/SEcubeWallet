#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QUrl>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QDebug>
#include <QDate>
#include <QSqlDriver>
//#define DEV // uncomment this line to disable login dialog so you can test non-secube related code

#define IDENT_COL   0
#define USER_COL    1
#define DOM_COL     2
#define PASS_COL    3
#define DATE_COL    4
#define DESC_COL    5

#define CUSTOM      7

#define UNUSED(expr) (void)(expr)

#define DRIVER "QSQLITE"

Q_DECLARE_OPAQUE_POINTER(sqlite3*)
Q_DECLARE_METATYPE(sqlite3*)

static int c_callback_createTableList(void *mainwindowP, int argc, char **argv, char **azColName){ //create Table List
    MainWindow* main = reinterpret_cast<MainWindow*>(mainwindowP);
    return main->callback_createTableList(argc, argv, azColName);
}

static int c_callback_populateTable(void *mainwindowP, int argc, char **argv, char **azColName){ //Populate Table
    MainWindow* main = reinterpret_cast<MainWindow*>(mainwindowP);
    return main->callback_populateTable(argc, argv, azColName);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow ){
    init();
}

MainWindow::~MainWindow()//Destructor
{
    sqlite3_os_end(); //Release the data structure made up by pointers to the rest of VFS interfaces that has been associated with common I/O operations.
    //    if (ui->CipherClose->isEnabled()) // if there is an opened database the button is enabled
    //        on_CipherClose_clicked(); //Call cipher before closing
    secure_finit(); /*Once the user has finished all the operations it is strictly required to call the secure_finit() to avoid memory leakege*/

    if(dbMem.open()){ // close any existent connections and database
        model->clear();
        proxyModel->clear();
        dbMem.close();
        QSqlDatabase::removeDatabase(DRIVER);
        dbMem = QSqlDatabase();
    }

    if(s.logged_in)
        L1_logout(&s);
    delete ui;
}

void MainWindow::init()
{
    // Configure UI and initial state of buttons as not clickable because there is no wallet to edit yet
    ui->setupUi(this);
    ui->AddEntry->setEnabled(false);
    ui->EditEntry->setEnabled(false);
    ui->DeleteEntry->setEnabled(false);
    ui->LaunchEntry->setEnabled(false);
    ui->CipherClose->setEnabled(false);
    ui->Showpass->setEnabled(false);
    ui->DomainFilter->setEnabled(false);
    ui->UserFilter->setEnabled(false);
    ui->DescFilter->setEnabled(false);
    ui->olderText->setEnabled(false);
    ui->Months->setEnabled(false);
    ui->CustomMonths->setEnabled(false);
    ui->NewTable->setEnabled(false);
    ui->WalletList->setEnabled(false);
    ui->DeleteWallet->setEnabled(false);
    ui->TableTitle->setVisible(false);
    ui->WalletView->hide();

    ui->WalletView->horizontalHeader()->setStretchLastSection(true);
    setWindowTitle(tr("SEcube Wallet"));

    help = new helpWindow;

#ifndef DEV
    //SEcube Password login dialog
    LoginDialog* loginDialog = new LoginDialog( this );
    loginDialog->exec();

    if(loginDialog->result() == QDialog::Rejected){
        qDebug() << "User aborted, terminating";
        exit(1);
    }

    // Get opened session in LoginDialog
    se3_session* tmp=loginDialog->getSession();
    memcpy(&s, tmp, sizeof(se3_session));
    if(L1_crypto_set_time(&s, (uint32_t)time(0))){
        qDebug () << "Error during L1_crypto_set_time, terminating";
        exit(1);
    }

    if(secure_init(&s, -1, SE3_ALGO_MAX+1)){
        qDebug () << "Error during initialization, terminating";
        exit(1);
        /*After the board is connected and the user is correctly logged in, the secure_init() should be issued.
         * The parameter se3_session *s contains all the information that let the system acknowledge which board
         * is connected and if the user has successfully logged in.*/
    }

    sqlite3_os_init(); // assign the data structure made up by pointers to the rest of VFS interfaces that has been associated with common I/O operations.
#endif
    return;
}


// New wallet button clicked: promt secureFileDialog and create database.
void MainWindow::on_NewWallet_clicked()
{
    SecureFileDialog *fileDialog = new SecureFileDialog( this, 1 ); //option 1 means we are creating new file
    fileDialog->exec();
    if(fileDialog->result()==QDialog::Rejected){
        return; // Error in Dialog or Cancel button, do nothing
    }
    fileName = fileDialog->getChosenFile();

    if(!fileName.isEmpty()){
        ui->TableTitle->setText(fileName); // To display name of wallet in UI.
        ui->TableTitle->setVisible(true);
        if (!OpenDataBase())
            return; //error opening database
        ui->NewTable->setEnabled(true);
        ui->DeleteWallet->setEnabled(true);
    }
    return;
}

// Create/Open sqlite dataBase
bool MainWindow::OpenDataBase (){

    if(!(QSqlDatabase::isDriverAvailable(DRIVER))) { //Check if sqlite is installed on OS
        qWarning() << "MainWindow::DatabaseConnect - ERROR: no driver " << DRIVER << " available";
        exit (1); // as the application does not work without Sqlite, exit.
    }

    if (dbMem.open()){ //close any prev. opened connections and database
        model->clear();
        proxyModel->clear();
        dbMem.close();
        QSqlDatabase::removeDatabase(DRIVER);
        dbMem = QSqlDatabase();

        //sqlite3_close(db);
    }

    dbMem = QSqlDatabase::addDatabase(DRIVER);
    dbMem.setDatabaseName(":memory:");
    if(!dbMem.open()){ //Check if it was possible to open the database
        qWarning() << "ERROR: " << dbMem.lastError();
        return false;
    }

    ui->WalletList->clear();

    QStringList CurrentTables = dbMem.tables(QSql::Tables);
    if (!CurrentTables.isEmpty()){
        ui->WalletList->setEnabled(true);
        ui->WalletList->addItems(CurrentTables);
    }

    ui->NewTable->setEnabled(true);
    ui->WalletList->setEnabled(true);
    ui->DeleteWallet->setEnabled(true);
    ui->TableTitle->setText(fileName); // To display name of wallet in UI.
    ui->TableTitle->setVisible(true);
    ui->OpenCyphered->setEnabled(false);
    ui->CipherClose->setEnabled(true);

    return true;

}

//After table is created in database, display it in a QTableView, and manage it using a Table Model
void MainWindow::CreateViewTable(const QString &WalletName ){

    // Create and configure model to access table easily
    model = new QSqlTableModel;
    model->setTable(WalletName);
    model->select();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);// as the table will be not edditable, the changes are updated by calling submitAll()

    //Create ProxyModel for filtering
    proxyModel = new MySortFilterProxyModel(this);
    proxyModel->setSourceModel(model); //connect proxyModel to Model

    //Connect the model to the view table: Sqltable -> Model -> ProxyModel ->  TableView
    ui->WalletView->setModel(proxyModel);
    //Configure the table
    //ui->WalletView->setSortingEnabled(true);//enable sorting
    ui->WalletView->setEditTriggers(QAbstractItemView::NoEditTriggers);// To make the table view not edditable
    ui->WalletView->setColumnHidden(IDENT_COL, true);//Hide Identification column, not important for user, only for sqlite.
    ui->WalletView->verticalHeader()->hide();
    ui->WalletView->setColumnHidden(PASS_COL, true);//Initially hide passwords column
    ui->WalletView->setSelectionBehavior( QAbstractItemView::SelectItems ); //To allow only one row selection...
    ui->WalletView->setSelectionMode( QAbstractItemView::SingleSelection ); //So we can edit one entry per time
    ui->WalletView->show();// show table, initially hidden


    //As we now have a wallet, we can enable the buttons
    ui->AddEntry->setEnabled(true);
    ui->EditEntry->setEnabled(true);
    ui->DeleteEntry->setEnabled(true);
    ui->LaunchEntry->setEnabled(true);
    ui->Showpass->setEnabled(true);
    ui->Showpass->setChecked(false);
    ui->CipherClose->setEnabled(true);
    ui->DomainFilter->setEnabled(true);
    ui->UserFilter->setEnabled(true);
    ui->DescFilter->setEnabled(true);
    ui->olderText->setEnabled(true);
    ui->Months->setEnabled(true);
    ui->CustomMonths->setEnabled(true);

    return;
}


//Call dialog and populate Table with the aqcuired data
void MainWindow::on_AddEntry_clicked(){

    AddEntry *add = new AddEntry(this);
    add->exec();
    if(add->result()==QDialog::Rejected){
        return; // Error or cancel, do nothing
    }

    QSqlRecord rec = model->record(); // Temp entry
    rec.setGenerated("id", false); // The ID column is managed automatically by the model, not by the user
    rec.setValue("Username", add->getUser());
    rec.setValue("Password",add->getPassword());
    rec.setValue("Domain",add->getDomain());
    rec.setValue("Description",add->getDescription());
    rec.setValue("Date", QDate::currentDate());

    int newRecNo = model->rowCount(); //Where to insert the new record?: at the end of the table
    if (model->insertRecord(newRecNo, rec))
        model->submitAll();// if insert ok, submit changes.

    return;
}

void MainWindow::on_WalletView_doubleClicked(const QModelIndex &index){
    MainWindow::on_EditEntry_clicked();
    UNUSED(index);
}

//Call dialog to allow user edit the data
void MainWindow::on_EditEntry_clicked()
{
    QModelIndexList selection = ui->WalletView->selectionModel()->selectedIndexes();//Get Items selected in table
    model->submitAll();//Submit any pending changes before continue. (cannot be done before select bcs it deletes it)

    // If Multiple rows can be selected (Disabled for edit. Could be good for delete)
    for(int i=0; i< selection.count(); i++){
        int row = selection.at(i).row();

        //Call add entry with second constructor, so we can pass it the values to edit
        AddEntry *add = new AddEntry(this, proxyModel->index(row,USER_COL).data().toString(),
                                     proxyModel->index(row,PASS_COL).data().toString(),
                                     proxyModel->index(row,DOM_COL).data().toString(),
                                     proxyModel->index(row,DESC_COL).data().toString());
        add->exec();

        if(add->result()==QDialog::Rejected)
            return; // If error or cancel, do nothing

        //Change the values in proxy
        proxyModel->setData(proxyModel->index(row,USER_COL), add->getUser());
        proxyModel->setData(proxyModel->index(row,PASS_COL), add->getPassword());
        proxyModel->setData(proxyModel->index(row,DOM_COL),  add->getDomain());
        proxyModel->setData(proxyModel->index(row,DESC_COL), add->getDescription());
        proxyModel->setData(proxyModel->index(row,DATE_COL), QDate::currentDate());

        if(!model->submitAll()) //Submit changes to database
            qDebug() << "Error: " << model->lastError();
    }
    return;
}

void MainWindow::on_DeleteEntry_clicked()
{
    QModelIndexList selection = ui->WalletView->selectionModel()->selectedIndexes();//Get Items selected in table
    model->submitAll();//Submit any pending changes before continue. (cannot be done before select bcs it deletes it)

    // If Multiple rows can be selected (disabled to allow edit)
    for(int i=0; i< selection.count(); i++){
        //Before continuing, promt confirmation dialog
        DeleteConfirmation * conf = new DeleteConfirmation;
        conf->exec();
        if(conf->result()==QDialog::Rejected)
            return;//if error or cancel, do nothing

        QModelIndex index = selection.at(i);
        proxyModel->removeRow(index.row());
        if(!model->submitAll()) //Submit changes if no errors
            qDebug() << "Error: " << model->lastError();
    }
    return;
}

void MainWindow::on_LaunchEntry_clicked(){

    QModelIndexList selection = ui->WalletView->selectionModel()->selectedIndexes();//Get Items selected in table
    model->submitAll();//Submit any pending changes before continue. (cannot be done before select bcs it deletes it)

    // If Multiple rows can be selected (Disabled for edit. Could be good for delete)
    for(int i=0; i< selection.count(); i++){
        int row = selection.at(i).row();
        QString domain=proxyModel->index(row,DOM_COL).data().toString();
        QString domainaux=domain;
        if (!domain.startsWith("http://") && !domain.startsWith("https://"))
            domain = "http://" + domain;

        if (domain.contains("www.")){
            if (!domainaux.remove("www.").contains("."))
                domain = domain+".com";
        }else if (!domain.contains("."))
            domain = domain+".com";
        qDebug() <<domain;
        QDesktopServices::openUrl(QUrl(domain));
    }
}

//When user wants, make passwords visible
void MainWindow::on_Showpass_toggled(bool checked){
    ui->WalletView->setColumnHidden(PASS_COL, !checked);
    return;
}

//To change envirolment, key ID and encryption
void MainWindow::on_EnvironmentBut_clicked(){
    EnvironmentDialog *envDialog = new EnvironmentDialog(this, &s);
    envDialog->exec();
    return;
}

//// ************ if user wants to write to disk ***************
void MainWindow::on_CipherClose_clicked(){

    //    //// *  Delete Secure database (if any) in disk first (only way to dump a data base)

    //    //prepare vars
    //    char enc_filename[65];
    //    uint16_t enc_len = 0;
    //    memset(enc_filename, 0, 65);

    //    //Get name of file in disk (encrypted) and delete.
    //    crypto_filename(fileName.toUtf8().data(), enc_filename, &enc_len );
    //    QFile::remove(enc_filename);

    //// * Create a secure Sqlite using securesqlite3 functions
    sqlite3_os_init(); // assign the data structure made up by pointers to the rest of VFS interfaces that has been associated with common I/O operations.

    if( sqlite3_open(fileName.toUtf8(), &dbSec) ) //create a new database using the filename specified by the user before.
        qDebug() << "Can't open database" << sqlite3_errmsg(dbSec);
    else
        qDebug() << "Opened database successfully";

    //// * Dump everything from the in-memory database to the secure database
    char *zErrMsg = 0;

    QSqlQuery query(dbMem);

    QStringList values;
    QSqlRecord record;
    QString finalSql;
    static const QString insert = QStringLiteral("INSERT INTO %1 VALUES (%2); "); //Insert statement

    // Read from in mem database
    QStringList tableList = dbMem.tables(QSql::Tables); // Get a list of the tables in dbMem
    foreach (const QString table, tableList) {
        QString sql = "create table "+table+
                "(id integer primary key, "
                "Username TEXT, "
                "Domain TEXT, "
                "Password TEXT, "
                "Date TEXT, "
                "Description TEXT );"; //The table

        if ( sqlite3_exec(dbSec, sql.toUtf8(), NULL, 0, &zErrMsg) != SQLITE_OK ){//create the table
            qDebug() << "SQL error: %s\n"<< zErrMsg;
            sqlite3_free(zErrMsg);
        } else
            qDebug() << table << "created successfully";

        query.prepare(QString("SELECT * FROM [%1]").arg(table)); //prepare select
        query.exec();
        while (query.next()){
            values.clear();
            record = query.record();
            for (int i = 0; i < record.count(); i++)
                values << "'"+record.value(i).toString()+"'";//value needs to be inside ''
            finalSql += insert.arg(table).arg(values.join(", "));//create a single sql statement from all the inserts
        }
    }
    // write into sec database
    qDebug() << finalSql;

    if ( sqlite3_exec(dbSec, finalSql.toUtf8(), NULL, 0, &zErrMsg) != SQLITE_OK ){//execute the statement
        qDebug() << "SQL error "<< zErrMsg;
        sqlite3_free(zErrMsg);
    } else
        qDebug() <<"Saved successfully";

    //// Close secure database connection
    sqlite3_close (dbSec);
    sqlite3_os_end();

    //    if(dbMem.open()){ // close anye existent connections and database
    //        dbMem.close();
    //        QSqlDatabase::removeDatabase(DRIVER);
    //        dbMem = QSqlDatabase();
    //    }


    ui->WalletView->hide();

    ui->AddEntry->setEnabled(false);
    ui->EditEntry->setEnabled(false);
    ui->DeleteEntry->setEnabled(false);
    ui->CipherClose->setEnabled(false);
    ui->LaunchEntry->setEnabled(false);
    ui->TableTitle->setVisible(false);
    ui->Showpass->setEnabled(false);
    ui->DomainFilter->setEnabled(false);
    ui->UserFilter->setEnabled(false);
    ui->DescFilter->setEnabled(false);
    ui->olderText->setEnabled(false);
    ui->Months->setEnabled(false);
    ui->CustomMonths->setEnabled(false);
    ui->NewTable->setEnabled(false);
    ui->WalletList->setEnabled(false);
    ui->DeleteWallet->setEnabled(false);
    ui->OpenCyphered->setEnabled(true);


    return;
}

int MainWindow::callback_createTableList(int argc, char **argv, char **azColName){ //Build TableList from ciphered db
    qDebug() << "in callback_createTableList";
    tableList << argv[0]; //only one arg, the table name.
    return 0;
}

int MainWindow::callback_populateTable(int argc, char **argv, char **azColName){ //Build TableList from ciphered db
    int i;
    qDebug() << "in callback_populateTable";

    static const QString insert = QStringLiteral("INSERT INTO %1 VALUES (%2);"); //Insert statement
    QStringList values;
    QString aux;

    for(i = 0; i<argc; i++){
        aux = argv[i];
        values << "'"+aux+"'";
    }

    query.prepare( insert.arg(currentTable).arg(values.join(", ")) );
    if (!query.exec()){
        qWarning() << "Couldn't insert values";
        qWarning() << "ERROR: " << query.lastError();
        return 0;
    }

    return 0;
}



////Open Cyphered database and display it
void MainWindow::on_OpenCyphered_clicked(){


    //// Open Secure database

    //call securefileDialog. Option 0 displays the cyphered files in the current directory
    SecureFileDialog *fileDialog = new SecureFileDialog( this, 0 );
    fileDialog->exec();
    if(fileDialog->result()==QDialog::Rejected){
        return ;
    }

    path = fileDialog->getChosenFile();
    fileName = QFileInfo(QFile(path)).fileName();

    sqlite3_os_init(); // assign the data structure made up by pointers to the rest of VFS interfaces that has been associated with common I/O operations.
    if( sqlite3_open_v2(fileName.toUtf8(), &dbSec, SQLITE_OPEN_READONLY, NULL) ) //open the database for read using the filename specified by the user before.
        qDebug() << "Can't open database" << sqlite3_errmsg(dbSec);
    else
        qDebug() << "Opened database successfully";

    QString tableNames="SELECT name FROM sqlite_master "
                       "WHERE type='table' "
                       "ORDER BY name;";
    char *zErrMsg = 0;
    tableList.clear();
    if ( sqlite3_exec(dbSec, tableNames.toUtf8(), c_callback_createTableList, this, &zErrMsg) != SQLITE_OK){
        qDebug() << "SQL error"<< zErrMsg;
        sqlite3_free(zErrMsg);
    } else
        qDebug() <<" Get table names successfully";


    if(!(QSqlDatabase::isDriverAvailable(DRIVER))) { //Check if sqlite is installed on OS
        qWarning() << "MainWindow::DatabaseConnect - ERROR: no driver " << DRIVER << " available";
        exit (1); // as the application does not work without Sqlite, exit.
    }

    if (dbMem.open()){ //close any prev. opened connections and database
        model->clear();
        proxyModel->clear();
        dbMem.close();
        QSqlDatabase::removeDatabase(DRIVER);
        dbMem = QSqlDatabase();
    }

    dbMem = QSqlDatabase::addDatabase(DRIVER);
    dbMem.setDatabaseName(":memory:");
    if(!dbMem.open()){ //Check if it was possible to open the database
        qWarning() << "ERROR: " << dbMem.lastError();
        return;
    }
    query = QSqlQuery(dbMem);

    foreach (const QString table, tableList){

        ui->WalletList->setEnabled(true);
        ui->WalletList->addItem(table);

        QString sql = "create table "+table+
                "(id integer primary key, "
                "Username TEXT, "
                "Domain TEXT, "
                "Password TEXT, "
                "Date TEXT, "
                "Description TEXT );"; //The table
        query.prepare(sql);
        if (!query.exec())
            qWarning() << "Couldn't create the table" << table <<" one might already exist";
        else
            qDebug() << table << "created successfully";
        currentTable = table;

        if ( sqlite3_exec(dbSec, QString("SELECT * FROM [%1]").arg(table).toUtf8(), c_callback_populateTable, this, &zErrMsg) != SQLITE_OK){
            qDebug() << "SQL error"<< zErrMsg;
            sqlite3_free(zErrMsg);
        } else
            qDebug() <<" Get all from " << table <<" successfully";
    }
    query.finish();


    //    //Re-Open data base and create table.
    //    if (!OpenDataBase())
    //        return; //error opening database
    //    //CreateViewTable();
    //    return;
}

void MainWindow::on_DomainFilter_textChanged(const QString &arg1){ //update Domain filter
    proxyModel->setFilterDomain(arg1);
}

void MainWindow::on_UserFilter_textChanged(const QString &arg1){ //Update User filter
    proxyModel->setFilterUser(arg1);
}

void MainWindow::on_DescFilter_textChanged(const QString &arg1){
    proxyModel->setFilterDesc(arg1);
}

void MainWindow::on_Months_currentIndexChanged(int index){
    proxyModel->setFilterOlder(index, ui->CustomMonths->text());
}

void MainWindow::on_CustomMonths_textChanged(const QString &arg1){
    proxyModel->setFilterOlder(ui->Months->currentIndex(), arg1);
}

void MainWindow::on_Help_clicked(){
    help->show();
}

void MainWindow::on_NewTable_clicked(){

    NewTable *newT = new NewTable(this);
    newT->exec();
    if(newT->result()==QDialog::Rejected){
        return; // Error or cancel, do nothing
    }
    QString WalletName = newT->getName();

    QSqlQuery query(dbMem);
    query.prepare("create table "+WalletName+
                  "(id integer primary key, "
                  "Username TEXT, "
                  "Domain TEXT, "
                  "Password TEXT, "
                  "Date TEXT, "
                  "Description TEXT )"); //The table

    if (!query.exec())
        qWarning() << "Couldn't create the table" << WalletName <<" one might already exist";

    query.finish();

    ui->WalletList->setEnabled(true);
    if(ui->WalletList->findText(WalletName) == -1) //Check if item already in the list
        ui->WalletList->addItem(WalletName); //if not, add it
    ui->WalletList->setCurrentText(WalletName);

    CreateViewTable(WalletName);
}

void MainWindow::on_WalletList_currentIndexChanged(const QString &arg1){ //just change the view to the selected wallet
    CreateViewTable(arg1);
}

void MainWindow::on_DeleteWallet_clicked(){
    QString WalletName=ui->WalletList->currentText();
    if (!WalletName.isEmpty()){
        DeleteConfirmation * conf = new DeleteConfirmation;
        conf->exec();
        if(conf->result()==QDialog::Rejected)
            return;//if error or cancel, do nothing

        QSqlQuery query(dbMem);

        query.prepare("DROP TABLE "+WalletName);
        if (!query.exec()){
            qWarning() << "Couldn't delete table" << WalletName;
            qWarning() << "ERROR: " << query.lastError();
            return;
        }
        query.finish();
        ui->WalletList->removeItem(ui->WalletList->currentIndex());
    }
}
