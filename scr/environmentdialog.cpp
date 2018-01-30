#include "environmentdialog.h"


EnvironmentDialog::EnvironmentDialog(QWidget *parent, se3_session *s) : QDialog(parent)
{
    memcpy(&session, s, sizeof(se3_session));
    setUpGUI();
    setWindowTitle( tr("Set Environment") );
    setModal( false );
}

//EnvironmentDialog::~EnvironmentDialog(){
//    delete formGridLayout;
//}

void EnvironmentDialog::setUpGUI(){
    // set up the layout
    //QGridLayout* formGridLayout = new QGridLayout( this );
    formGridLayout = new QGridLayout( this );
    chooseEncryption = new QComboBox ( this );
    chooseKeyId = new QComboBox ( this );
    labelEncryption = new QLabel ( this );
    labelKeyId = new QLabel ( this );
    labelEncryption->setText( tr("Encryption"));
    labelKeyId->setText( tr("KeyID"));
    labelEncryption->setBuddy( chooseEncryption );
    labelKeyId->setBuddy( chooseKeyId );

    refreshEnvironment();

    // initialize buttons
    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok );
    buttons->addButton( QDialogButtonBox::Cancel );
    buttons->addButton( QDialogButtonBox::Reset );
    buttons->button( QDialogButtonBox::Reset )->setText( tr("Refresh") );
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Ok") );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Abort") );

    // connects slots
    connect( buttons->button( QDialogButtonBox::Cancel ),
             SIGNAL (clicked()),
             this,
             SLOT (slotAbortEnvironment())
             );

    connect( buttons->button( QDialogButtonBox::Ok ),
             SIGNAL (clicked()),
             this,
             SLOT (slotAcceptEnvironment())
             );
    connect( buttons->button( QDialogButtonBox::Reset ),
             SIGNAL (clicked()),
             this,
             SLOT (refreshEnvironment())
             );

    // place components into the dialog
    formGridLayout->addWidget( labelKeyId, 0, 0 );
    formGridLayout->addWidget( chooseKeyId, 0, 1 );
    formGridLayout->addWidget( labelEncryption, 1, 0 );
    formGridLayout->addWidget( chooseEncryption, 1, 1 );
    formGridLayout->addWidget( buttons, 2, 0, 2, 3 );

    setLayout( formGridLayout );

}

void EnvironmentDialog::slotAcceptEnvironment(){

    QString alg_name = chooseEncryption->currentText();
    QHashIterator<int, se3_algo> it(algo_list);
    uint16_t alg_id = SE3_ALGO_MAX + 1;
    while(it.hasNext()){
        it.next();
        if(QString::compare(alg_name, QString((char *)it.value().name), Qt::CaseSensitive) == 0){
            alg_id = (uint16_t) it.key();
        }
    }
    if ( secure_update(NULL, key_list.at(chooseKeyId->currentIndex()).id, alg_id)){

        exit(1);
    }

    accept();
}

void EnvironmentDialog::refreshEnvironment(){
    enum{
        MAX_KEY = 100
    };
    uint16_t ret = SE3_OK, count = 0;
    uint16_t i = 0, j = 0;
    se3_key key[MAX_KEY];
    se3_algo algo[SE3_ALGO_MAX];


//    while( (ret = L1_key_list(&session, i, 1, &key, &count)) == SE3_OK && count>0){
//        key_list.push_back(key);
//        i++;
//    }
    memset(key, '0', MAX_KEY*sizeof(se3_key));
    while( (ret = L1_key_list(&session, i, MAX_KEY, NULL, key, &count)) == SE3_OK && count>0){
        for(j=0;j<count;j++){
            if(key[j].validity>time(0))
                key_list.push_back(key[j]);
        }
        i+=count;
        memset(key, '0', MAX_KEY*sizeof(se3_key));
    }

    if( ret != SE3_OK ){

        exit(1);
    }
    count = 0;
    if((ret = L1_get_algorithms(&session, 0, SE3_ALGO_MAX, algo, &count))!= SE3_OK || count == 0 ){

        exit(1);
    }
    for( i = 0 ; i < count ; i++){
        if(algo[i].type == SE3_CRYPTO_TYPE_BLOCKCIPHER_AUTH && i != SE3_ALGO_AES_HMAC) //Until SE3_ALGO_AES_HMAC we need to be sure that can not be set as environment
            algo_list.insert(i, algo[i]);
    }
    while(chooseKeyId->count())
        chooseKeyId->removeItem(0);

    while(chooseEncryption->count())
        chooseEncryption->removeItem(0);

    for ( i = 0; i < key_list.count(); i++){
        //chooseKeyId->addItem(QString().number(key_list.at(i).id).append(" ").append(QString((char *)key_list.at(i).name)));
        QString key_id = QString().number(key_list.at(i).id);
        int key_label_len = (int)key_list.at(i).name_size;
        QString key_label = QString::fromUtf8((char*)key_list.at(i).name, key_label_len);
        chooseKeyId->addItem(key_id + " " + key_label);
    }
    QHashIterator<int, se3_algo> it(algo_list);
    while(it.hasNext()){
        it.next();
        chooseEncryption->addItem(QString((char *)it.value().name));
    }

}

void EnvironmentDialog::slotAbortEnvironment(){
    reject();
}


