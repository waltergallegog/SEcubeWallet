#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow ){
    init();
}

MainWindow::~MainWindow()
{
    secure_finit(); /*Once the user has finished all the operations it is strictly required to call the secure_finit() to avoid memory leakege*/

    if(db.open()) // close anye existent database
        db.close();

    if(s.logged_in)
        L1_logout(&s);

    delete ui;
}

void MainWindow::init()
{

    ui->setupUi(this);
    ui->AddEntry->setEnabled(false);
    ui->EditEntry->setEnabled(false);
    ui->DeleteEntry->setEnabled(false);
    ui->CipherClose->setEnabled(false);
    ui->TableTitle->setVisible(false);
    ui->WalletView->hide();
    ui->WalletView->horizontalHeader()->setStretchLastSection(true);


    // Password login dialog
    LoginDialog* loginDialog = new LoginDialog( this );
    loginDialog->exec();

    if(loginDialog->result() == QDialog::Rejected){
        qDebug() << "User aborted, terminating";
        exit(1);
    }

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
         * is connected and if the user has successfully logged in. This function may set a default configuration
         * thanks to the L1 provided services: it will be used thefirst available key for encryption, and the
         * first available algorithm that can manage to encrypt and authenticate data at the same time. Since
         * keys must not be shared outside the device, from the host side, the user may just request to use a
         * key represented by a unique ID(uint32_t keyID).*/
    }

}


// New wallet button clicked: promt secureFileDialog and create database.
void MainWindow::on_NewWallet_clicked()
{
    SecureFileDialog *fileDialog = new SecureFileDialog( this, 1 ); //option 1 means we are creating new file
    fileDialog->exec();
    if(fileDialog->result()==QDialog::Rejected){
        return;
    }
    fileName = fileDialog->getChosenFile();

    if(!fileName.isEmpty()){
        ui->TableTitle->setText(fileName);
        ui->TableTitle->setVisible(true);
        OpenDataBase(); //
        CreateViewTable();
    }
}

void MainWindow::OpenDataBase (){
    qDebug() << "entered open data base";

    const QString DRIVER("QSQLITE");
    if(!(QSqlDatabase::isDriverAvailable(DRIVER))) { //Check if sqlite is installed on OS
        qWarning() << "MainWindow::DatabaseConnect - ERROR: no driver " << DRIVER << " available";
        exit (1);
    }
    if (db.open()){ //close any prev. opened database
        model->clear();
        db.close();
        QSqlDatabase::removeDatabase(DRIVER);
        db = QSqlDatabase();
    }
    db = QSqlDatabase::addDatabase(DRIVER);

    db.setDatabaseName(fileName);
    if(!db.open()){ //Check if it was possible to open the database
        qWarning() << "ERROR: " << db.lastError();
        exit (1);
    }

    QSqlQuery query;
    query.prepare("create table Wallet "
                  "(id integer primary key, "
                  "Username TEXT, "
                  "Domain TEXT, "
                  "Password TEXT )");

    if (!query.exec())
        qDebug() << "Couldn't create the table 'wallet': one might already exist.";

    return;

}

void MainWindow::CreateViewTable(){
    qDebug() << "entered CreateViewTable";
    model = new QSqlTableModel;


    model->setTable("Wallet");
    model->select();

    ui->WalletView->setModel(model);
    //ui->WalletView->setColumnHidden(0, true); //Hide Identification
    ui->WalletView->setColumnHidden(3, true);
    ui->WalletView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->WalletView->setSelectionBehavior( QAbstractItemView::SelectItems );
    ui->WalletView->setSelectionMode( QAbstractItemView::SingleSelection );



    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    ui->WalletView->show();
    ui->AddEntry->setEnabled(true);
    ui->CipherClose->setEnabled(true);
    ui->AddEntry->setEnabled(true);
    ui->EditEntry->setEnabled(true);
    ui->DeleteEntry->setEnabled(true);
    ui->CipherClose->setEnabled(true);
    ui->Showpass->setChecked(false);


    return;
}



void MainWindow::on_AddEntry_clicked(){

    AddEntry *add = new AddEntry(this);
    add->exec();
    if(add->result()==QDialog::Rejected){
        return;
    }

    QSqlRecord rec = model->record();
    rec.setGenerated("id", false);
    rec.setValue("Username", add->getUser());
    rec.setValue("Password",add->getPassword());
    rec.setValue("Domain",add->getDomain());

    int newRecNo = model->rowCount();
    if (model->insertRecord(newRecNo, rec))
        model->submitAll();


}

void MainWindow::on_CipherClose_clicked(){
    uint16_t ret = SE3_OK;
    char enc_filename[65];
    uint16_t enc_len = 0;

    memset(enc_filename, 0, 65);
    crypto_filename(fileName.toUtf8().data(), enc_filename, &enc_len );

    // Begin encryption of file
    QString *path_plainfile = new QString(fileName);
    if((ret = secure_open(path_plainfile->toUtf8().data(), &sefile_file, SEFILE_WRITE, SEFILE_NEWFILE))){
        if ( ret == SEFILE_SIGNATURE_MISMATCH ){   // Check if signature on header failed
            //            ui->plainName_lbl->setText("Signature Mismatch");
            //            ui->encName_lbl->setText("Signature Mismatch");
        }else{
            //            ui->plainName_lbl->setText("Error code: " + ret);
            //            ui->encName_lbl->setText("Error code: " + ret);
        }
        return; // todo error
    }
    QFile plain_file(fileName);

    plain_file.open(QIODevice::ReadOnly);
    qint64 size_len = plain_file.size();

    uint8_t *buffer = (uint8_t*) malloc((size_t)size_len*sizeof(uint8_t));
    memcpy(buffer, plain_file.readAll().data(), (size_t)size_len);
    if((ret = secure_write(&sefile_file, buffer, size_len))) {
        if ( ret == SEFILE_SIGNATURE_MISMATCH ){
            //            ui->plainName_lbl->setText("Signature Mismatch");
            //            ui->encName_lbl->setText("Signature Mismatch");
        }else{
            //            ui->plainName_lbl->setText("Error code: " + ret);
            //            ui->encName_lbl->setText("Error code: " + ret);
        }
    }

    free(buffer);
    secure_close(&sefile_file);
    plain_file.close();

    // clean
    QFile::remove(fileName);
    ui->WalletView->hide();
    ui->AddEntry->setEnabled(false);
    ui->EditEntry->setEnabled(false);
    ui->DeleteEntry->setEnabled(false);
    ui->CipherClose->setEnabled(false);
    ui->TableTitle->setVisible(false);
    return; // todo error

}

void MainWindow::on_OpenCyphered_clicked()
{
    QByteArray plain_buffer;
    uint8_t *buffer;
    char enc_filename[65];
    uint16_t ret = SE3_OK;
    uint16_t enc_len = 0;
    uint32_t nBytesRead = 0, file_len = 0;

    SecureFileDialog *fileDialog = new SecureFileDialog( this, 0 );
    fileDialog->exec();
    if(fileDialog->result()==QDialog::Rejected){
        return ;
    }
    path = fileDialog->getChosenFile();
    fileName = QFileInfo(QFile(path)).fileName();

    if(fileName.isNull()) return;
    path = path.left(path.size() - fileName.size());

    QString *path_plainfile = new QString(path + fileName);

    if((ret = secure_open(path_plainfile->toUtf8().data(), &sefile_file, SEFILE_WRITE, SEFILE_OPEN))){
        if ( ret == SEFILE_SIGNATURE_MISMATCH ){   // Check if signature on header failed
            //            ui->plainName_lbl->setText("Signature Mismatch");
            //            ui->encName_lbl->setText("Signature Mismatch");
        }else{
            //            ui->plainName_lbl->setText("Error code: " + ret);
            //            ui->encName_lbl->setText("Error code: " + ret);
        }
        return; // todo error
    }

    if((ret = secure_getfilesize(path_plainfile->toUtf8().data(), &file_len))){
        if ( ret == SEFILE_SIGNATURE_MISMATCH ){
            //            ui->plainName_lbl->setText("Signature Mismatch");
            //            ui->encName_lbl->setText("Signature Mismatch");
            secure_close(&sefile_file);
            return;
        }else if (ret > 0){
            //            ui->plainName_lbl->setText("Error code: " + ret);
            //            ui->encName_lbl->setText("Error code: " + ret);
            secure_close(&sefile_file);
            return;
        }
    }
    buffer = (uint8_t *)calloc(file_len, sizeof(uint8_t));
    if((ret = secure_read(&sefile_file,  buffer, file_len, &nBytesRead))){
        if ( ret == SEFILE_SIGNATURE_MISMATCH ){
            //            ui->plainName_lbl->setText("Signature Mismatch");
            //            ui->encName_lbl->setText("Signature Mismatch");
            secure_close(&sefile_file);
            return;
        }else if (ret > 0){
            //            ui->plainName_lbl->setText("Error code: " + ret);
            //            ui->encName_lbl->setText("Error code: " + ret);
            secure_close(&sefile_file);
            return;
        }
    }
    plain_buffer = QByteArray((char *)buffer, (int)nBytesRead);

    QFile plain_file(fileName);
    plain_file.open(QIODevice::WriteOnly);
    plain_file.write(plain_buffer);
    plain_file.close();
    OpenDataBase();
    CreateViewTable();
}

void MainWindow::on_DeleteEntry_clicked()
{
    QModelIndexList selection = ui->WalletView->selectionModel()->selectedIndexes();
    model->submitAll();

    // Multiple rows can be selected (not anymore, disabled to allow edit)
    for(int i=0; i< selection.count(); i++){

        DeleteConfirmation * conf = new DeleteConfirmation;
        conf->exec();
        if(conf->result()==QDialog::Rejected)
            return;

        QModelIndex index = selection.at(i);
        qDebug() << index.row();
        model->removeRows(index.row(),1);
    }
    if(!model->submitAll())
        qDebug() << "Error: " << model->lastError();
}

void MainWindow::on_EnvironmentBut_clicked()
{
    EnvironmentDialog *envDialog = new EnvironmentDialog(this, &s);
    envDialog->exec();

}


void MainWindow::on_EditEntry_clicked()
{
    QModelIndexList selection = ui->WalletView->selectionModel()->selectedIndexes();
    model->submitAll();

    // Multiple rows can be selected
    for(int i=0; i< selection.count(); i++){
        QModelIndex index = selection.at(i);
        qDebug() << index.row();

        QSqlRecord rec = model->record(index.row());

        AddEntry *add = new AddEntry(this, rec.value("Username").toString(),rec.value("Password").toString(), rec.value("Domain").toString());
        add->exec();

        if(add->result()==QDialog::Rejected)
            return;

        rec.setGenerated("id", false);
        rec.setValue("Username", add->getUser());
        rec.setValue("Password",add->getPassword());
        rec.setValue("Domain",add->getDomain());

        if (model->setRecord(index.row(), rec));

    }
    if(!model->submitAll())
        qDebug() << "Error: " << model->lastError();

}

void MainWindow::on_Showpass_toggled(bool checked){
    ui->WalletView->setColumnHidden(3, !checked);
}
