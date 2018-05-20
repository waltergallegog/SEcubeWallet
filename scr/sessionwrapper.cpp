#include "sessionwrapper.h"

#include <QApplication>
#include <QDebug>
#include <QString>

#define QD(print) qDebug() << "WRAPPER: " << print

SessionWrapper::SessionWrapper(int no) : QObject(){ // this class does not have any GUI, so qobject is ok
    n=no;
    walletProc=0;
}

SessionWrapper::~SessionWrapper(){
    QD("destructor");
}

void SessionWrapper::init()
{
    // define shared memory segments used to comunicate with wallet process
    shaMemSession = new QSharedMemory("sessionSharing");
    shaMemReq = new QSharedMemory("requestSharing");
    shaMemRes = new QSharedMemory("responseSharing");
    shaMemDevBuf =  new QSharedMemory("devBufSharing");

    if (shaMemSession->isAttached())
        if (!shaMemSession->detach())
            qDebug() << "WRAPPER: Unable to detach from shared memory sess";

    if (shaMemReq->isAttached())
        if (!shaMemReq->detach())
            qDebug() << "WRAPPER: Unable to detach from shared memory req";

    if (shaMemRes->isAttached())
        if (!shaMemRes->detach())
            qDebug() << "WRAPPER: Unable to detach from shared memory res";

    if (shaMemDevBuf->isAttached())
        if (!shaMemDevBuf->detach())
            qDebug() << "WRAPPER: Unable to detach from shared memory buf";


    /// create segment used to send session
    if (!shaMemSession->create(sizeof(se3_session))) {
        qDebug() << "WRAPPER: Unable to create session, try attach, detach method";
        QD(shaMemSession->attach());
        QD(shaMemSession->detach());
        if (!shaMemSession->create(sizeof(se3_session))) {
            qDebug() << "WRAPPER: attach detach did not work";
            QCoreApplication::exit(1);//could not create sharedmem TODO:instead of exit, work without shared.
        }
    }

    /// in L0.c, device request and response are assigned (uint8_t*)malloc(SE3_COMM_N*SE3_COMM_BLOCK);
    if (!shaMemReq->create(SE3_COMM_N*SE3_COMM_BLOCK)) {
        qDebug() << "WRAPPER: Unable to create req, try attach, detach method";
        QD(shaMemReq->attach());
        QD(shaMemReq->detach());
        if (!shaMemReq->create(SE3_COMM_N*SE3_COMM_BLOCK)) {
            qDebug() << "WRAPPER: attach detach did not work";
             QCoreApplication::exit(1);
        }
    }
    if (!shaMemRes->create(SE3_COMM_N*SE3_COMM_BLOCK)) {
        qDebug() << "WRAPPER: Unable to create res, try attach, detach method";
        QD(shaMemRes->attach());
        QD(shaMemRes->detach());
        if (!shaMemRes->create((SE3_COMM_N*SE3_COMM_BLOCK))) {
            qDebug() << "WRAPPER: attach detach did not work";
            QCoreApplication::exit(1);
        }
    }

    /// device has a void pointer which is later initialized as memalign SE3_COMM_BLOCK
    if (!shaMemDevBuf->create(SE3_COMM_BLOCK)) {
        qDebug() << "WRAPPER: Unable to create buf, try attach, detach method";
        QD(shaMemDevBuf->attach());
        QD(shaMemDevBuf->detach());
        if (!shaMemDevBuf->create((SE3_COMM_BLOCK))) {
            qDebug() << "WRAPPER: attach detach did not work";
            QCoreApplication::exit(1);
        }
    }

    ///after shared mem segments are created, we start the wallet process

    walletProc = new QProcess(this);
    walletProc->setProcessChannelMode(QProcess::ForwardedChannels);

    connect(walletProc,
            SIGNAL(finished(int,QProcess::ExitStatus)),
            this,
            SLOT(processFinished(int, QProcess::ExitStatus))); //when wallet proc finishes, we call slot

    QStringList args;
    args << QString::number(n);

    walletProc->start(QCoreApplication::applicationFilePath(), args);
}


void SessionWrapper::processFinished(int code , QProcess::ExitStatus status){
    qDebug()<< "WRAPPER: on process finished";
    qDebug() << code <<status;
    if (status==QProcess::NormalExit){// && code == 0){
        // destructor was called in wallet, so logout was already performed, regardless code
    }
    else {// crash exit,
        secubeLogout();
    }
    clean(); // dettach from shared mems
    QApplication::quit();
}

void SessionWrapper::secubeLogout(){
    QD ("logout");
    int ret;

    shaMemSession->lock();
    shaMemReq->lock();
    shaMemRes->lock();
    shaMemDevBuf->lock();

    s = (se3_session*)shaMemSession->data();
    if(s->logged_in){
        QD ("s is logged in");
        // inside s there are 3 pointers, pointing to the other shared mem segments. But from the wallet, the shared mem
        //adresses are diferent, so we need to fix them first.
        s->device.request = (uint8_t*)shaMemReq->data();
        s->device.response = (uint8_t* )shaMemRes->data();
        s->device.f.buf = shaMemDevBuf->data();

        //s->device.f is a int representing a file descriptor. File descriptors make sense only inside the process that
        //created them, so we need to open the file again from this process. The file to open is in info.path
        if( (ret = fix_fd(s->device.info.path, &(s->device.f))) != SE3_OK )
            QD("error fixing file descriptor");
        else if ( (ret = L1_logout(s)) == SE3_OK )
            QD("logged out succesfully");
        else
            QD("error logging out");
    }

    shaMemSession->unlock();
    shaMemReq->unlock();
    shaMemRes->unlock();
    shaMemDevBuf->unlock();
}

void SessionWrapper::clean(){
    QD ("clean");

    if (shaMemSession->isAttached()){
        if (!shaMemSession->detach())
            qDebug() << "WRAPPER: Unable to detach from shared memory sess";
    }

    if (shaMemReq->isAttached()){
        if (!shaMemReq->detach())
            qDebug() << "WRAPPER: Unable to detach from shared memory req";
    }

    if (shaMemRes->isAttached()){
        if (!shaMemRes->detach())
            qDebug() << "WRAPPER: Unable to detach from shared memory res";
    }

    if (shaMemDevBuf->isAttached()){
        if (!shaMemDevBuf->detach())
            qDebug() << "WRAPPER: Unable to detach from shared memory buf";
    }
}
