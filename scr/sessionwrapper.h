#ifndef SESSIONWRAPPER_H
#define SESSIONWRAPPER_H

#include <QProcess>
#include "se3/L1.h"
#include <QSharedMemory>

class SessionWrapper : public QObject
{
    Q_OBJECT
public:
    SessionWrapper(int no);
    ~SessionWrapper();
    void init();

private:
    int n;
    QProcess *walletProc;
    se3_session *s;              // session variable
    QSharedMemory *shaMemSession, *shaMemReq, *shaMemRes, *shaMemDevBuf;

    void clean();

private slots:
    void processFinished(int code , QProcess::ExitStatus status);
};

#endif // SESSIONWRAPPER_H
