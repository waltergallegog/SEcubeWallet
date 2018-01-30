#include "logindialog.h"


LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)
{
    dev.opened = false;    //private: se3_device dev ;

    setUpGUI();
    setWindowTitle( tr("Login") );
    setModal( true ); //When an application modal dialog is opened, the user must finish interacting with the dialog and close it before they can access any other window in the application
}
LoginDialog::~LoginDialog(){

}

void LoginDialog::setUpGUI(){
    // set up the layout
    //QGridLayout* formGridLayout = new QGridLayout( this );
    formGridLayout = new QGridLayout( this );
    chooseDevice = new QComboBox ( this );
    labelDevice = new QLabel ( this );
    labelDevice->setText( tr("Device"));
    labelDevice->setBuddy( chooseDevice );
    refreshDevice();

    editPassword = new QLineEdit( this );
    editPassword->setEchoMode( QLineEdit::Password );

    // initialize the labels

    labelPassword = new QLabel( this );
    labelPassword->setText( tr( "Pin/Password" ) );
    labelPassword->setBuddy( editPassword );

    // initialize buttons
    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok );
    buttons->addButton( QDialogButtonBox::Cancel );
    buttons->addButton( QDialogButtonBox::Reset );
    buttons->button( QDialogButtonBox::Reset )->setText( tr("Refresh") );
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Login") );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Abort") );

    // connects slots
    connect( buttons->button( QDialogButtonBox::Cancel ),
             SIGNAL (clicked()),
             this,
             SLOT (slotAbortLogin())
             );

    connect( buttons->button( QDialogButtonBox::Ok ),
             SIGNAL (clicked()),
             this,
             SLOT (slotAcceptLogin())
             );
    connect( buttons->button( QDialogButtonBox::Reset ),
             SIGNAL (clicked()),
             this,
             SLOT (refreshDevice())
             );
    connect( this, //what is his for?
             SIGNAL (acceptLogin(QString&)),
             this,
             SLOT (slotAcceptLogin())
             );

    formGridLayout->addWidget( labelDevice, 0, 0 );
    formGridLayout->addWidget( chooseDevice, 0, 1 );
    formGridLayout->addWidget( labelPassword, 1, 0 );
    formGridLayout->addWidget( editPassword, 1, 1 );
    formGridLayout->addWidget( buttons, 2, 0, 2, 3 );
    setLayout( formGridLayout );

}


void LoginDialog::setPassword(QString &password){
    editPassword->setText( password );
}
se3_session *LoginDialog::getSession(){ // called from main window to get session
    return &s;
}

void LoginDialog::slotAcceptLogin(){
    QString password = editPassword->text();
    uint8_t pin[32];
    int device_index = chooseDevice->currentIndex();
    se3_disco_it it;
    uint16_t ret = SE3_OK;

    memset(pin, 0, 32);
    memcpy(pin, password.toUtf8().data(), password.size());
    it = device_found.at(device_index);
    if(!dev.opened){
        if((ret = L0_open(&dev, &(it.device_info), SE3_TIMEOUT))!= SE3_OK){

            exit(1);
        }
    }

    //if ( (ret = L1_login(&s, &dev, (uint8_t *)password.toUtf8().data(), SE3_ACCESS_USER)) != SE3_OK ){
    if ( (ret = L1_login(&s, &dev, pin, SE3_ACCESS_USER)) != SE3_OK ){
         if(ret == SE3_ERR_PIN){ //If the password is wrong, a message will appear in the dialog
            QLabel* labelError = new QLabel ( this );
            labelError->setText(tr("Invalid Password"));
            labelError->setStyleSheet("QLabel { color : red; font-weight : bold; }");
            formGridLayout->addWidget( labelError, 2, 1 );
            formGridLayout->addWidget( buttons, 3, 0, 3, 3 );
            updateGeometry();
        }else{

            exit(1);
        }
    }else{
        accept();
    }
}

void LoginDialog::refreshDevice(){
    se3_disco_it it;
    bool found = true;

    while(chooseDevice->count()){
        chooseDevice->removeItem(0);
        device_found.removeFirst();
    }

    L0_discover_init(&it);
    while((found = L0_discover_next(&it))){
#ifdef _WIN32
        chooseDevice->addItem(QString::fromWCharArray(it.device_info.path, -1));
#else
        chooseDevice->addItem(QString::fromLocal8Bit(it.device_info.path, -1));
#endif
        device_found.push_back(it);
    }

}


void LoginDialog::slotAbortLogin(){
    reject();
}


