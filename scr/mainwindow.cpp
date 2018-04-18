﻿#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QUrl>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QDebug>
#include <QDate>
#include <QSqlDriver>
#include <QComboBox>

#define IDENT_COL   0
#define USER_COL    1
#define DOM_COL     2
#define PASS_COL    3
#define DATE_COL    4
#define DESC_COL    5

#define UNUSED(expr) (void)(expr)
#define QD qDebug()


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

////  Destructor: ensure last changes were written, close database connections and SECube connection.
MainWindow::~MainWindow()
{
#ifdef SECUBE
    // TODO: prompt if user wants to cipher before closing
    // if (ui->CipherClose->isEnabled()) // if there is an opened database the button is enabled
    // on_CipherClose_clicked(); //Call cipher before closing

    sqlite3_os_end(); //Release the data structure made up by pointers to the rest of VFS interfaces that has been associated with common I/O operations.
    secure_finit(); /*Once the user has finished all the operations it is strictly required to call the secure_finit() to avoid memory leakege*/
    if(s.logged_in)
        L1_logout(&s);
    delete ui;
#endif

    if(dbMem.open()){ // close any existent connections and in memory database
        model->clear();
        proxyModel->clear();
        dbMem.close();
        QSqlDatabase::removeDatabase(DRIVER);
        dbMem = QSqlDatabase();
    }
}

//// init(): Configure GUI initial state
/// add elements not possible to add in qtcreator's design mode
/// initialiaze dialogs
/// launch login dialog
void MainWindow::init(){

    ui->setupUi(this);
    setWindowTitle(tr("SEcube Wallet"));

    // add table List to ToolBar before delete
    tableList = new QComboBox(this);
//    ui->tableTB->insertWidget(ui->action_Delete_Table, tableList);
    ui->entriesTB->insertWidget(ui->action_Add_Entry, tableList);
    connect(tableList, SIGNAL(currentIndexChanged(QString)), SLOT(tableList_currentIndexChanged(QString)));

    //State of some actions as not clickable because there is no wallet to edit yet
    ui->action_Save_Wallet->setEnabled(false);
    ui->action_Save_Wallet_As->setEnabled(false);
    ui->action_Delete_Wallet->setEnabled(false);

    ui->action_Add_Table->setEnabled(false);
    ui->action_Delete_Table->setEnabled(false);

    ui->action_Add_Entry->setEnabled(false);
    ui->action_Edit_Entry->setEnabled(false);
    ui->action_Delete_Entry->setEnabled(false);
    ui->action_Show_Passwords->setEnabled(false);
    ui->action_Launch_Domain->setEnabled(false);

    model=NULL;//only initi Table once
    //ui->WalletView->hide();

    help = new helpWindow;
#ifdef SECUBE
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



/// New wallet button clicked: start an in-memory database so the user can add tables/entries. Nothing to save in disk yet
void MainWindow::on_action_New_Wallet_triggered()
{
    //TODO: Before starting a new wallet check if there are not saved changes.

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
    dbMem.setDatabaseName(":memory:"); // :memory: allows to create in-memory databases
    if(!dbMem.open()){ //Check if it was possible to open the database
        qWarning() << "ERROR: " << dbMem.lastError();
        return;// Do nothing
    }

    // if everything whent ok, enable some actions
    ui->action_Add_Table->setEnabled(true);
    ui->action_Delete_Table->setEnabled(true);

    return;
}

//// Create a new table in the database, and add it to tableList

void MainWindow::on_action_Add_Table_triggered(){
    NewTable *newT = new NewTable(this);
    newT->exec();
    if(newT->result()==QDialog::Rejected){
        return; // Error or cancel, do nothing
    }
    QString tableName = newT->getName();

    //TODO: use global query instead of local?
    QSqlQuery query(dbMem);
    query.prepare("create table "+tableName+
                  "(id integer primary key, "
                  "Username TEXT, "
                  "Domain TEXT, "
                  "Password TEXT, "
                  "Date TEXT, "
                  "Description TEXT )"); //The table

    if (!query.exec()){
        //TODO: prompt error dialog name may be invalid. see more. and link
        qWarning() << "Couldn't create the table " << tableName << query.lastError();
        return;
    }

    query.finish();

    tableList->setEnabled(true);
    if(tableList->findText(tableName) == -1) //Check if item already in the list
        tableList->addItem(tableName); //if not, add it
    tableList->setCurrentText(tableName);

    //TODO: as there is a new table-> enable save

    // Finally, update the tableView to reflect the new/opened table
    if (model==NULL)
        createTableView(tableName);
    //    else{
    //        UpdateTableView(tableName);// not necessary, done by tableList combobox
    //    }

    //As we now have a table, we can enable the buttons
    ui->action_Add_Entry->setEnabled(true);
    ui->action_Edit_Entry->setEnabled(true);
    ui->action_Delete_Entry->setEnabled(true);
    ui->action_Launch_Domain->setEnabled(true);
    ui->action_Show_Passwords->setEnabled(true);
    ui->action_Show_Passwords->setChecked(false);
}



//// After table is created in database, display it in a QTableView, and manage it using a Table Model
void MainWindow::createTableView(const QString &tableName ){

    // Create and configure model to access table easily
    model = new QSqlTableModel;
    model->setTable(tableName);
    model->select();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);// as the table will be not edditable, the changes are updated by calling submitAll()

    //Create ProxyModel for filtering
    proxyModel = new MySortFilterProxyModel(this);
    proxyModel->setSourceModel(model); //connect proxyModel to Model

    //Connect the model to the view table: Sqltable -> Model -> ProxyModel ->  TableView
    ui->tableView->setModel(proxyModel);

    //Configure the table
    //ui->WalletView->setSortingEnabled(true);//enable sorting
    ui->tableView->verticalHeader()->hide();


    ui->tableView->setColumnHidden(IDENT_COL, true);//Hide Identification column, not important for user, only for sqlite.
    ui->tableView->setItemDelegateForColumn(PASS_COL, passDelegate);//Hide passwords initially

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);// To make the table view not edditable
    ui->tableView->setSelectionBehavior( QAbstractItemView::SelectItems ); //To allow only one row selection...
    ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection ); //So we can edit one entry per time
    ui->tableView->show();// show table, initially hidden

    // resize table columns to fill available space
    int col;
    int totalWidth=0;
    int tableWidth=ui->tableView->width();
    for (col=USER_COL;col<=DESC_COL; col++)//Get total space ocup. by columns
        totalWidth+= ui->tableView->horizontalHeader()->sectionSize(col);
    int diff = tableWidth-totalWidth;
    int div = diff / 5;
    int mod = diff % 5;

    //remainder too to the first column. (the -2 is for margins pixels(?))
    ui->tableView->horizontalHeader()->resizeSection(USER_COL, div + mod -2 + ui->tableView->horizontalHeader()->sectionSize(USER_COL));

    //add to each column the correspo. value.
    for (col=DOM_COL;col<=DESC_COL; col++)
        ui->tableView->horizontalHeader()->resizeSection(col, div + ui->tableView->horizontalHeader()->sectionSize(col));


    // Aligned Layout
    filters = new FiltersAligned();

    connect(filters->userFilter, SIGNAL(textChanged(QString)), SLOT(UserFilter_textChanged(QString)));
    connect(filters->domFilter,  SIGNAL(textChanged(QString)), SLOT(DomainFilter_textChanged(QString)));
    connect(filters->passFilter, SIGNAL(textChanged(QString)), SLOT(PassFilter_textChanged(QString)));
    connect(filters->descFilter, SIGNAL(textChanged(QString)), SLOT(DescFilter_textChanged(QString)));
    connect(filters->dateFilter, SIGNAL(textChanged(QString)), SLOT(dateFilter_textChanged(QString)));
    connect(filters->dateUnit,   SIGNAL(currentIndexChanged(int)), SLOT(dateUnit_currentIndexChanged(int)));

    //TODO: to have some spacing btwn filters, put each filter inside a widget with a layout, and put margins.

    ui->filtersWidget->setLayout(filters);
    filters->setTableColumnsToTrack(ui->tableView->horizontalHeader());
    filters->setParent(ui->filtersWidget);
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), SLOT(invalidateAlignedLayout()));
    connect(ui->tableView->horizontalScrollBar(), SIGNAL(valueChanged(int)), SLOT(invalidateAlignedLayout()));

    return;
}
////Used by columnAlignedLayout in createViewTable
void MainWindow::invalidateAlignedLayout(){
    filters->invalidate();
}


/// Update Table view. When user selects a different table

void MainWindow::UpdateTableView(const QString &tableName){

    int col;
    int widths [DESC_COL-USER_COL+1];
    for (col=USER_COL;col<=DESC_COL; col++)
        widths[col] =  ui->tableView->horizontalHeader()->sectionSize(col);

    model->setTable(tableName);
    model->select();
    for (col=USER_COL;col<=DESC_COL; col++)
        ui->tableView->horizontalHeader()->resizeSection(col, widths[col]);
    ui->tableView->setColumnHidden(IDENT_COL, true);
}

/// When user changes table we need to modify the tableview
void MainWindow::tableList_currentIndexChanged(const QString &arg1){ //just change the view to the selected table
    if(model!=NULL)
        UpdateTableView(arg1);

}

void MainWindow::on_action_Delete_Table_triggered(){
    QString tableName = tableList->currentText();
    if (!tableName.isEmpty()){
        DeleteConfirmation * conf = new DeleteConfirmation;
        conf->exec();
        if(conf->result()==QDialog::Rejected)
            return;//if error or cancel, do nothing

        QSqlQuery query(dbMem);

        query.prepare("DROP TABLE " + tableName);
        if (!query.exec()){
            qWarning() << "Couldn't delete table" << tableName;
            qWarning() << "ERROR: " << query.lastError();
            return;
        }
        query.finish();
        tableList->removeItem(tableList->currentIndex());
    }
}

///Call dialog and populate Table with the aqcuired data
void MainWindow::on_action_Add_Entry_triggered(){

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

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index){
    MainWindow::on_action_Edit_Entry_triggered();
    UNUSED(index);
}

///Call add entry dialog an pass values to constructior allowing users to edit the data
void MainWindow::on_action_Edit_Entry_triggered()
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedIndexes();//Get Items selected in table
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

///Call confirmation dialog, if confirmed, delete entry
void MainWindow::on_action_Delete_Entry_triggered(){
    //TODO: ask for password. change in settings.
    QModelIndexList selection = ui->tableView->selectionModel()->selectedIndexes();//Get Items selected in table
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

///Check url is writen correctly and call browser
void MainWindow::on_action_Launch_Domain_triggered(){
    //TODO: autofill forms. check again url
    QModelIndexList selection = ui->tableView->selectionModel()->selectedIndexes();//Get Items selected in table
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

////Make passwords Not/visible by setting or not a column delegator
void MainWindow::on_action_Show_Passwords_toggled(bool show){

    //TODO: confirmation dialog, maybe with password? configurable from settings.
    if (!show){
        ui->tableView->setItemDelegateForColumn(PASS_COL, passDelegate);
        filters->passFilter->clear();
        filters->passFilter->setEnabled(false);
    }
    else{
        ui->tableView->setItemDelegateForColumn(PASS_COL, 0);
        filters->passFilter->clear();
        filters->passFilter->setEnabled(true);
    }

}

///To change envirolment, key ID and encryption
void MainWindow::on_action_Set_Environment_triggered(){
    EnvironmentDialog *envDialog = new EnvironmentDialog(this, &s);
    envDialog->exec();
    return;
}

//// ************ if user wants to write to disk ***************
void MainWindow::on_action_Save_Wallet_triggered(){

    ////  ask for a filename to save changes

    SecureFileDialog *fileDialog = new SecureFileDialog( this, 1 ); //option 1 means we are creating new file
    fileDialog->exec();
    if(fileDialog->result()==QDialog::Rejected){
        return; // Error in Dialog or Cancel button, do nothing
    }
    fileName = fileDialog->getChosenFile();

    if(fileName.isEmpty()) // if no valid file name => do nothing
        return;

    //    ui->TableTitle->setText(fileName); // To display name of table in UI.
    //    ui->TableTitle->setVisible(true);


    //// Check if file already exists

    //prepare vars
    char enc_filename[65];
    uint16_t enc_len = 0;
    memset(enc_filename, 0, 65);

    //Get name of file in disk (encrypted) and delete.
    crypto_filename(fileName.toUtf8().data(), enc_filename, &enc_len );
    if (QFile::exists(enc_filename)){
        // prompt: file already exists
    }


    //// * Create a secure Sqlite using securesqlite3 functions
    //sqlite3_os_init(); // assign the data structure made up by pointers to the rest of VFS interfaces that has been associated with common I/O operations.

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
    QStringList tables = dbMem.tables(QSql::Tables); // Get a list of the tables in dbMem
    foreach (const QString table, tables) {
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

    ui->tableView->hide();

    //    ui->AddEntry->setEnabled(false);
    //    ui->EditEntry->setEnabled(false);
    //    ui->DeleteEntry->setEnabled(false);
    //    ui->CipherClose->setEnabled(false);
    //    ui->LaunchEntry->setEnabled(false);
    //    ui->TableTitle->setVisible(false);
    //    ui->Showpass->setEnabled(false);
    //    ui->DomainFilter->setEnabled(false);
    //    ui->UserFilter->setEnabled(false);
    //    ui->DescFilter->setEnabled(false);
    //    ui->olderText->setEnabled(false);
    //    ui->Months->setEnabled(false);
    //    ui->CustomMonths->setEnabled(false);
    //    ui->NewTable->setEnabled(false);
    //    ui->WalletList->setEnabled(false);
    //    ui->DeleteWallet->setEnabled(false);
    //    ui->OpenCyphered->setEnabled(true);


    return;
}

int MainWindow::callback_createTableList(int argc, char **argv, char **azColName){ //Build TableList from ciphered db
    qDebug() << "in callback_createTableList";
    tables << argv[0]; //only one arg, the table name.
    UNUSED(argc);
    UNUSED(azColName);
    return 0;
}

int MainWindow::callback_populateTable(int argc, char **argv, char **azColName){ //Build TableList from ciphered db
    int i;
    UNUSED(azColName);

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
void MainWindow::on_action_Open_Wallet_triggered(){

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
    tables.clear();
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

    foreach (const QString table, tables){

        //        ui->tableList->setEnabled(true);
        //        ui->tableList->addItem(table);

        QString sql = "create table "+table+
                "(id integer primary key, "
                "Username TEXT, "
                "Domain TEXT, "
                "Password TEXT, "
                "Date TEXT, "
                "Description TEXT );"; //The table
        query.prepare(sql);
        if (!query.exec()){
            qWarning() << "Couldn't create the table" << table <<" one might already exist";
            return;
        }
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
}

//// ****** Search/Filters  *****

void MainWindow::UserFilter_textChanged(const QString &arg1){ //Update User filter
    proxyModel->setFilterUser(arg1);
}

void MainWindow::DomainFilter_textChanged(const QString &arg1){ //update Domain filter
    proxyModel->setFilterDomain(arg1);
}

void MainWindow::PassFilter_textChanged(const QString &arg1){ //update Domain filter
    proxyModel->setFilterPass(arg1);
}

void MainWindow::DescFilter_textChanged(const QString &arg1){
    proxyModel->setFilterDesc(arg1);
}

void MainWindow::dateUnit_currentIndexChanged(int index){
    proxyModel->setFilterOlder(index, filters->dateFilter->text());
}

void MainWindow::dateFilter_textChanged(const QString &arg1){
    proxyModel->setFilterOlder(filters->dateUnit->currentIndex(), arg1);
}

//// ***** Help Menu *******
void MainWindow::on_action_About_triggered(){
    help->show();
}


