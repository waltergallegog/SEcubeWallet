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
 
    
    
    
//  out << tmp->device;
   out << tmp->token;
   out << tmp->key;
   out << tmp->buf;
   out << tmp->locked;
   out << tmp->logged_in;
   out << tmp->timeout;
//    out << tmp->hfile;
//    out << tmp->cryptoctx;
   out << tmp->cryptoctx_initialized;

//    out << tmp->device.info;
   out << tmp->device.request;
   out << tmp->device.response;
//    out << tmp->device.f;
   out << tmp->device.opened;

   out << tmp->device.info.path;
   out << tmp->device.info.serialno;
   out << tmp->device.info.hello_msg;
   out << tmp->device.info.status;

   out << tmp->device.f.fd;
   out << tmp->device.f.buf;
   out << tmp->device.f.locked;

   out << tmp->hfile.fd;
   out << tmp->hfile.buf;
   out << tmp->hfile.locked;

//    out << tmp->cryptoctx.aesenc;
//    out << tmp->cryptoctx.aesdec;
//    out << tmp->cryptoctx.hmac;
   out << tmp->cryptoctx.hmac_key;
   out << tmp->cryptoctx.auth;

   out << tmp->cryptoctx.aesenc.rk;
   out << tmp->cryptoctx.aesenc.Nr;
   out << tmp->cryptoctx.aesenc.InitVector;
   out << tmp->cryptoctx.aesenc.mode;
   out << tmp->cryptoctx.aesenc.Te0;
   out << tmp->cryptoctx.aesenc.Te1;
   out << tmp->cryptoctx.aesenc.Te2;
   out << tmp->cryptoctx.aesenc.Te3;
   out << tmp->cryptoctx.aesenc.Te4;
   out << tmp->cryptoctx.aesenc.Td0;
   out << tmp->cryptoctx.aesenc.Td1;
   out << tmp->cryptoctx.aesenc.Td2;
   out << tmp->cryptoctx.aesenc.Td3;
   out << tmp->cryptoctx.aesenc.Td4;

   out << tmp->cryptoctx.aesdec.rk;
   out << tmp->cryptoctx.aesdec.Nr;
   out << tmp->cryptoctx.aesdec.InitVector;
   out << tmp->cryptoctx.aesdec.mode;
   out << tmp->cryptoctx.aesdec.Te0;
   out << tmp->cryptoctx.aesdec.Te1;
   out << tmp->cryptoctx.aesdec.Te2;
   out << tmp->cryptoctx.aesdec.Te3;
   out << tmp->cryptoctx.aesdec.Te4;
   out << tmp->cryptoctx.aesdec.Td0;
   out << tmp->cryptoctx.aesdec.Td1;
   out << tmp->cryptoctx.aesdec.Td2;
   out << tmp->cryptoctx.aesdec.Td3;
   out << tmp->cryptoctx.aesdec.Td4;

//    out << tmp->cryptoctx.hmac.shaCtx;
   out << tmp->cryptoctx.hmac.iPad;
   out << tmp->cryptoctx.hmac.oPad;

   out << tmp->cryptoctx.hmac.shaCtx.total;
   out << tmp->cryptoctx.hmac.shaCtx.state;
   out << tmp->cryptoctx.hmac.shaCtx.buffer;
   out << tmp->cryptoctx.hmac.shaCtx.W;

   typedef struct
   {
       uint32_t   total[2];
       uint32_t   state[8];
       uint8_t    buffer[64];
       uint32_t   W[64];
   } B5_tSha256Ctx;

   typedef struct
   {
      B5_tSha256Ctx        shaCtx;
      uint8_t            iPad[64];
      uint8_t            oPad[64];
   } B5_tHmacSha256Ctx;


   typedef struct {
       uint32_t rk[4*(14 + 1)];       /**< Precomputed round keys */
       uint8_t  Nr;                   /**< Number of rounds */
       uint8_t  InitVector[16];       /**< IV for OFB, CBC, CTR */
       uint8_t  mode;                 /**< Active mode */
       uint32_t const *Te0;
       uint32_t const *Te1;
       uint32_t const *Te2;
       uint32_t const *Te3;
       uint32_t const *Te4;
       uint32_t const *Td0;
       uint32_t const *Td1;
       uint32_t const *Td2;
       uint32_t const *Td3;
       uint32_t const *Td4;
   } B5_tAesCtx;

   typedef struct se3_payload_cryptoctx_ {
       B5_tAesCtx aesenc;
       B5_tAesCtx aesdec;
       B5_tHmacSha256Ctx hmac;
       uint8_t hmac_key[B5_AES_256];
       uint8_t auth[B5_SHA256_DIGEST_SIZE];
   } se3_payload_cryptoctx;

   typedef struct {
       int fd;
       void* buf; //[512] memalign
       bool locked;
   } se3_file;


   /** \brief SEcube Device Information structure */
   typedef struct se3_device_info_ {
       se3_char path[SE3_MAX_PATH];
       uint8_t serialno[SE3_SN_SIZE];
       uint8_t hello_msg[SE3_HELLO_SIZE];
       uint16_t status;
   } se3_device_info;



   /** \brief SEcube Device structure */
   typedef struct se3_device_ {
       se3_device_info info;
       uint8_t* request;
       uint8_t* response;
       se3_file f;
       bool opened;
   } se3_device;

   /* struct */
   /** \brief SEcube Communication session structure */
   typedef struct se3_session_ {
       se3_device device;
       uint8_t token[SE3_L1_TOKEN_SIZE];
       uint8_t key[SE3_L1_KEY_SIZE];
       uint8_t buf[SE3_COMM_N * SE3_COMM_BLOCK];
       bool locked;
       bool logged_in;
       uint32_t timeout;
       se3_file hfile;
       se3_payload_cryptoctx cryptoctx;
       bool cryptoctx_initialized;
       // TODO: Add flag for type of user logged (see set_{admin,user}_PIN) or change type for logged_in
   } se3_session;
   
   
    //  in >> tmp->device;
    in >> tmp->token;
    in >> t;
    in >> tmp->key;
    in >> tmp->buf;
    in >> tmp->locked;
    in >> tmp->logged_in;
    in >> tmp->timeout;
    //    in >> tmp->hfile;
    //    in >> tmp->cryptoctx;
    in >> tmp->cryptoctx_initialized;

    //    in >> tmp->device.info;
    in >> tmp->device.request;
    in >> tmp->device.response;
    //    in >> tmp->device.f;
    in >> tmp->device.opened;

    in >> tmp->device.info.path;
    in >> tmp->device.info.serialno;
    in >> tmp->device.info.hello_msg;
    in >> tmp->device.info.status;

    in >> tmp->device.f.fd;
    in >> tmp->device.f.buf;
    in >> tmp->device.f.locked;

    in >> tmp->hfile.fd;
    in >> tmp->hfile.buf;
    in >> tmp->hfile.locked;

    //    in >> tmp->cryptoctx.aesenc;
    //    in >> tmp->cryptoctx.aesdec;
    //    in >> tmp->cryptoctx.hmac;
    in >> tmp->cryptoctx.hmac_key;
    in >> tmp->cryptoctx.auth;

    in >> tmp->cryptoctx.aesenc.rk;
    in >> tmp->cryptoctx.aesenc.Nr;
    in >> tmp->cryptoctx.aesenc.InitVector;
    in >> tmp->cryptoctx.aesenc.mode;
    in >> tmp->cryptoctx.aesenc.Te0;
    in >> tmp->cryptoctx.aesenc.Te1;
    in >> tmp->cryptoctx.aesenc.Te2;
    in >> tmp->cryptoctx.aesenc.Te3;
    in >> tmp->cryptoctx.aesenc.Te4;
    in >> tmp->cryptoctx.aesenc.Td0;
    in >> tmp->cryptoctx.aesenc.Td1;
    in >> tmp->cryptoctx.aesenc.Td2;
    in >> tmp->cryptoctx.aesenc.Td3;
    in >> tmp->cryptoctx.aesenc.Td4;

    in >> tmp->cryptoctx.aesdec.rk;
    in >> tmp->cryptoctx.aesdec.Nr;
    in >> tmp->cryptoctx.aesdec.InitVector;
    in >> tmp->cryptoctx.aesdec.mode;
    in >> tmp->cryptoctx.aesdec.Te0;
    in >> tmp->cryptoctx.aesdec.Te1;
    in >> tmp->cryptoctx.aesdec.Te2;
    in >> tmp->cryptoctx.aesdec.Te3;
    in >> tmp->cryptoctx.aesdec.Te4;
    in >> tmp->cryptoctx.aesdec.Td0;
    in >> tmp->cryptoctx.aesdec.Td1;
    in >> tmp->cryptoctx.aesdec.Td2;
    in >> tmp->cryptoctx.aesdec.Td3;
    in >> tmp->cryptoctx.aesdec.Td4;

    //    in >> tmp->cryptoctx.hmac.shaCtx;
    in >> tmp->cryptoctx.hmac.iPad;
    in >> tmp->cryptoctx.hmac.oPad;

    in >> tmp->cryptoctx.hmac.shaCtx.total;
    in >> tmp->cryptoctx.hmac.shaCtx.state;
    in >> tmp->cryptoctx.hmac.shaCtx.buffer;
    in >> tmp->cryptoctx.hmac.shaCtx.W;

    //    typedef struct
    //    {
    //        uint32_t   total[2];
    //        uint32_t   state[8];
    //        uint8_t    buffer[64];
    //        uint32_t   W[64];
    //    } B5_tSha256Ctx;

    //    typedef struct
    //    {
    //       B5_tSha256Ctx        shaCtx;
    //       uint8_t            iPad[64];
    //       uint8_t            oPad[64];
    //    } B5_tHmacSha256Ctx;


    //    typedef struct {
    //        uint32_t rk[4*(14 + 1)];       /**< Precomputed round keys */
    //        uint8_t  Nr;                   /**< Number of rounds */
    //        uint8_t  InitVector[16];       /**< IV for OFB, CBC, CTR */
    //        uint8_t  mode;                 /**< Active mode */
    //        uint32_t const *Te0;
    //        uint32_t const *Te1;
    //        uint32_t const *Te2;
    //        uint32_t const *Te3;
    //        uint32_t const *Te4;
    //        uint32_t const *Td0;
    //        uint32_t const *Td1;
    //        uint32_t const *Td2;
    //        uint32_t const *Td3;
    //        uint32_t const *Td4;
    //    } B5_tAesCtx;

    //    typedef struct se3_payload_cryptoctx_ {
    //        B5_tAesCtx aesenc;
    //        B5_tAesCtx aesdec;
    //        B5_tHmacSha256Ctx hmac;
    //        uint8_t hmac_key[B5_AES_256];
    //        uint8_t auth[B5_SHA256_DIGEST_SIZE];
    //    } se3_payload_cryptoctx;

    //    typedef struct {
    //        int fd;
    //        void* buf; //[512] memalign
    //        bool locked;
    //    } se3_file;


    //    /** \brief SEcube Device Information structure */
    //    typedef struct se3_device_info_ {
    //        se3_char path[SE3_MAX_PATH];
    //        uint8_t serialno[SE3_SN_SIZE];
    //        uint8_t hello_msg[SE3_HELLO_SIZE];
    //        uint16_t status;
    //    } se3_device_info;



    //    /** \brief SEcube Device structure */
    //    typedef struct se3_device_ {
    //        se3_device_info info;
    //        uint8_t* request;
    //        uint8_t* response;
    //        se3_file f;
    //        bool opened;
    //    } se3_device;

    //    /* struct */
    //    /** \brief SEcube Communication session structure */
    //    typedef struct se3_session_ {
    //        se3_device device;
    //        uint8_t token[SE3_L1_TOKEN_SIZE];
    //        uint8_t key[SE3_L1_KEY_SIZE];
    //        uint8_t buf[SE3_COMM_N * SE3_COMM_BLOCK];
    //        bool locked;
    //        bool logged_in;
    //        uint32_t timeout;
    //        se3_file hfile;
    //        se3_payload_cryptoctx cryptoctx;
    //        bool cryptoctx_initialized;
    //        // TODO: Add flag for type of user logged (see set_{admin,user}_PIN) or change type for logged_in
    //    } se3_session;


    
        QD2 (s);

    //    QD2 (s->device);
    QD2 (s->token);
    QD2 (s->key);
    QD2 (s->buf);
    QD2 (s->locked);
    QD2 (s->logged_in);
    QD2 (s->timeout);
    //    QD2 (s->hfile);
    //    QD2 (s->cryptoctx);
    QD2 (s->cryptoctx_initialized);

    //    QD2 (s->device.info);
    QD2 (s->device.request);
    QD2 (s->device.response);
    //    QD2 (s->device.f);
    QD2 (s->device.opened);

    QD2 (s->device.info.path);
    QD2 (s->device.info.serialno);
    QD2 (s->device.info.hello_msg);
    QD2 (s->device.info.status);

    QD2 (s->device.f.fd);
    QD2 (s->device.f.buf);
    QD2 (s->device.f.locked);

    QD2 (s->hfile.fd);
    QD2 (s->hfile.buf);
    QD2 (s->hfile.locked);

    //    QD2 (s->cryptoctx.aesenc);
    //    QD2 (s->cryptoctx.aesdec);
    //    QD2 (s->cryptoctx.hmac);
    QD2 (s->cryptoctx.hmac_key);
    QD2 (s->cryptoctx.auth);

    QD2 (s->cryptoctx.aesenc.rk);
    QD2 (s->cryptoctx.aesenc.Nr);
    QD2 (s->cryptoctx.aesenc.InitVector);
    QD2 (s->cryptoctx.aesenc.mode);
    QD2 (s->cryptoctx.aesenc.Te0);
    QD2 (s->cryptoctx.aesenc.Te1);
    QD2 (s->cryptoctx.aesenc.Te2);
    QD2 (s->cryptoctx.aesenc.Te3);
    QD2 (s->cryptoctx.aesenc.Te4);
    QD2 (s->cryptoctx.aesenc.Td0);
    QD2 (s->cryptoctx.aesenc.Td1);
    QD2 (s->cryptoctx.aesenc.Td2);
    QD2 (s->cryptoctx.aesenc.Td3);
    QD2 (s->cryptoctx.aesenc.Td4);

    QD2 (s->cryptoctx.aesdec.rk);
    QD2 (s->cryptoctx.aesdec.Nr);
    QD2 (s->cryptoctx.aesdec.InitVector);
    QD2 (s->cryptoctx.aesdec.mode);
    QD2 (s->cryptoctx.aesdec.Te0);
    QD2 (s->cryptoctx.aesdec.Te1);
    QD2 (s->cryptoctx.aesdec.Te2);
    QD2 (s->cryptoctx.aesdec.Te3);
    QD2 (s->cryptoctx.aesdec.Te4);
    QD2 (s->cryptoctx.aesdec.Td0);
    QD2 (s->cryptoctx.aesdec.Td1);
    QD2 (s->cryptoctx.aesdec.Td2);
    QD2 (s->cryptoctx.aesdec.Td3);
    QD2 (s->cryptoctx.aesdec.Td4);

    //    QD2 (s->cryptoctx.hmac.shaCtx);
    QD2 (s->cryptoctx.hmac.iPad);
    QD2 (s->cryptoctx.hmac.oPad);

    QD2 (s->cryptoctx.hmac.shaCtx.total);
    QD2 (s->cryptoctx.hmac.shaCtx.state);
    QD2 (s->cryptoctx.hmac.shaCtx.buffer);
    QD2 (s->cryptoctx.hmac.shaCtx.W);
    
        QD2 (s);

//    QD2 (s->device);
    QD2 (s->token);
    QD2 (s->key);
    QD2 (s->buf);
    QD2 (s->locked);
    QD2 (s->logged_in);
    QD2 (s->timeout);
//    QD2 (s->hfile);
//    QD2 (s->cryptoctx);
    QD2 (s->cryptoctx_initialized);

//    QD2 (s->device.info);
    QD2 (s->device.request);
    QD2 (s->device.response);
//    QD2 (s->device.f);
    QD2 (s->device.opened);

    QD2 (s->device.info.path);
    QD2 (s->device.info.serialno);
    QD2 (s->device.info.hello_msg);
    QD2 (s->device.info.status);

    QD2 (s->device.f.fd);
    QD2 (s->device.f.buf);
    QD2 (s->device.f.locked);

    QD2 (s->hfile.fd);
    QD2 (s->hfile.buf);
    QD2 (s->hfile.locked);

//    QD2 (s->cryptoctx.aesenc);
//    QD2 (s->cryptoctx.aesdec);
//    QD2 (s->cryptoctx.hmac);
    QD2 (s->cryptoctx.hmac_key);
    QD2 (s->cryptoctx.auth);

    QD2 (s->cryptoctx.aesenc.rk);
    QD2 (s->cryptoctx.aesenc.Nr);
    QD2 (s->cryptoctx.aesenc.InitVector);
    QD2 (s->cryptoctx.aesenc.mode);
    QD2 (s->cryptoctx.aesenc.Te0);
    QD2 (s->cryptoctx.aesenc.Te1);
    QD2 (s->cryptoctx.aesenc.Te2);
    QD2 (s->cryptoctx.aesenc.Te3);
    QD2 (s->cryptoctx.aesenc.Te4);
    QD2 (s->cryptoctx.aesenc.Td0);
    QD2 (s->cryptoctx.aesenc.Td1);
    QD2 (s->cryptoctx.aesenc.Td2);
    QD2 (s->cryptoctx.aesenc.Td3);
    QD2 (s->cryptoctx.aesenc.Td4);

    QD2 (s->cryptoctx.aesdec.rk);
    QD2 (s->cryptoctx.aesdec.Nr);
    QD2 (s->cryptoctx.aesdec.InitVector);
    QD2 (s->cryptoctx.aesdec.mode);
    QD2 (s->cryptoctx.aesdec.Te0);
    QD2 (s->cryptoctx.aesdec.Te1);
    QD2 (s->cryptoctx.aesdec.Te2);
    QD2 (s->cryptoctx.aesdec.Te3);
    QD2 (s->cryptoctx.aesdec.Te4);
    QD2 (s->cryptoctx.aesdec.Td0);
    QD2 (s->cryptoctx.aesdec.Td1);
    QD2 (s->cryptoctx.aesdec.Td2);
    QD2 (s->cryptoctx.aesdec.Td3);
    QD2 (s->cryptoctx.aesdec.Td4);

//    QD2 (s->cryptoctx.hmac.shaCtx);
    QD2 (s->cryptoctx.hmac.iPad);
    QD2 (s->cryptoctx.hmac.oPad);

    QD2 (s->cryptoctx.hmac.shaCtx.total);
    QD2 (s->cryptoctx.hmac.shaCtx.state);
    QD2 (s->cryptoctx.hmac.shaCtx.buffer);
    QD2 (s->cryptoctx.hmac.shaCtx.W);