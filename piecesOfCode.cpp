// have in mem datab and sec database linked
QVariant v = db.driver()->handle();
qDebug()<< v.isValid() << "  type  "<<v.typeName();
if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*")==0) {
    // v.data() returns a pointer to the handle
    db2 = *static_cast<sqlite3 **>(v.data());
    if (db2 == 0){  // check that it is not NULL
        int rc;
        sqlite3_os_init();

        rc = sqlite3_open(fileName.toUtf8(), &db2);

        if( rc )
            qDebug() << "Can't open database" << sqlite3_errmsg(db2);
        else
            qDebug() << "Opened database successfully";
        }
    else
        qDebug() << "not working";

}
else
    qDebug() << "not working 2";


//// *  Delete Secure database (if any) in disk first (only way to dump a data base)

//prepare vars
char enc_filename[65];
uint16_t enc_len = 0;
memset(enc_filename, 0, 65);

//Get name of file in disk (encrypted) and delete.
crypto_filename(fileName.toUtf8().data(), enc_filename, &enc_len );
QFile::remove(enc_filename);


//********* populte walletlist
QStringList CurrentTables = dbMem.tables(QSql::Tables);
if (!CurrentTables.isEmpty()){
//        ui->WalletList->setEnabled(true);
//        ui->WalletList->addItems(CurrentTables);
}


//**************delete widgets*******
NOTE: delete the previous (if any) filters, easier but ugly?
    if (dateWidget!=NULL){
        for (auto widget: dateWidget->findChildren<QWidget*> ())
            delete widget;
    }
    for (auto widget: ui->filtersWidget->findChildren<QWidget*> ())
        delete widget;

    delete ui->filtersWidget->layout();//and delete layout before puting it there again
    

    
    #define SQLITE_OK           0   /* Successful result */
/* beginning-of-error-codes */
#define SQLITE_ERROR        1   /* SQL error or missing database */
#define SQLITE_INTERNAL     2   /* Internal logic error in SQLite */
#define SQLITE_PERM         3   /* Access permission denied */
#define SQLITE_ABORT        4   /* Callback routine requested an abort */
#define SQLITE_BUSY         5   /* The database file is locked */
#define SQLITE_LOCKED       6   /* A table in the database is locked */
#define SQLITE_NOMEM        7   /* A malloc() failed */
#define SQLITE_READONLY     8   /* Attempt to write a readonly database */
#define SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
#define SQLITE_CORRUPT     11   /* The database disk image is malformed */
#define SQLITE_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
#define SQLITE_FULL        13   /* Insertion failed because database is full */
#define SQLITE_CANTOPEN    14   /* Unable to open the database file */
#define SQLITE_PROTOCOL    15   /* Database lock protocol error */
#define SQLITE_EMPTY       16   /* Database is empty */
#define SQLITE_SCHEMA      17   /* The database schema changed */
#define SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
#define SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
#define SQLITE_MISMATCH    20   /* Data type mismatch */
#define SQLITE_MISUSE      21   /* Library used incorrectly */
#define SQLITE_NOLFS       22   /* Uses OS features not supported on host */
#define SQLITE_AUTH        23   /* Authorization denied */
#define SQLITE_FORMAT      24   /* Auxiliary database format error */
#define SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define SQLITE_NOTADB      26   /* File opened that is not a database file */
#define SQLITE_NOTICE      27   /* Notifications from sqlite3_log() */
#define SQLITE_WARNING     28   /* Warnings from sqlite3_log() */
#define SQLITE_ROW         100  /* sqlite3_step() has another row ready */
#define SQLITE_DONE        101  /* sqlite3_step() has finished executing */

/** L1 errors */
enum {
    SE3_ERR_ACCESS = 100,  ///< insufficient privileges
    SE3_ERR_PIN = 101,  ///< pin rejected
    SE3_ERR_RESOURCE = 200,  ///< resource not found
    SE3_ERR_EXPIRED = 201,  ///< resource expired
    SE3_ERR_MEMORY = 400,  ///< no more space to allocate resource
    SE3_ERR_AUTH =  401    ///< SHA256HMAC Authentication failed
};

/** L0 error codes */
enum {
    SE3_OK = 0,  ///< success
    SE3_ERR_HW = 0xF001,  ///< hardware failure
    SE3_ERR_COMM = 0xF002,  ///< communication error
    SE3_ERR_BUSY = 0xF003,  ///< device locked by another process
    SE3_ERR_STATE = 0xF004,  ///< invalid state for this operation
    SE3_ERR_CMD = 0xF005,  ///< command does not exist
    SE3_ERR_PARAMS = 0xF006,  ///< parameters are not valid
};