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
