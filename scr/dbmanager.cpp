#include <dbmanager.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

DbManager::DbManager(const QString &fileName){
    const QString DRIVER("QSQLITE");

    if(!(QSqlDatabase::isDriverAvailable(DRIVER))) {
        qWarning() << "MainWindow::DatabaseConnect - ERROR: no driver " << DRIVER << " available";
        exit (1);
    }

    m_db = QSqlDatabase::addDatabase(DRIVER);

    m_db.setDatabaseName(fileName);
    if(!m_db.open()){
      qWarning() << "ERROR: " << m_db.lastError();
      exit (1);
     }
}

DbManager::~DbManager(){
    if (m_db.isOpen())
        m_db.close();
}

bool DbManager::isOpen() const{
    return m_db.isOpen();
}

bool DbManager::createTable(){
    bool success = false;
    QSqlQuery query;

    query.prepare("create table Wallet "
              "(id integer primary key, "
              "Username TEXT, "
              "Password TEXT, "
              "Domain TEXT )");

    if (!query.exec()){
        qDebug() << "Couldn't create the table 'wallet': one might already exist.";
        qWarning() << "MainWindow::DatabaseInit - ERROR: " << query.lastError().text();
        success = false;
    }
    return success;
}





