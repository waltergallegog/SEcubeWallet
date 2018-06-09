#ifndef ADDENTRY_H
#define ADDENTRY_H

#include <QDialog>
#include <QString>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QStandardItemModel>


//#include "zxcvbn.h"

//password generators
#define PWGEN 0
#define PWQGEN 1
#define CUSTOM 2
#define NONE 3

namespace Ui {
class AddEntry;
}

class AddEntry : public QDialog
{
    Q_OBJECT

public:
     // Constructor when calling from add
    explicit AddEntry(QWidget *parent = 0);

    //Constructor when calling from edit.
    explicit AddEntry(QWidget *parent, QString EditUserIn, QString EditPassIn, QString EditDomIn, QString EditDescIn);

    ~AddEntry();//Destructor

    //Methods to access data from main.
    QString getUser();
    QString getDomain();
    QString getPassword();
    QString getDescription();



private slots:
    void on_InUser_textChanged(const QString &arg1);
    void on_InDomain_textChanged(const QString &arg1);
    void on_InPass_textChanged(const QString &text);
    void on_InPass2_textChanged(const QString &text);
    void on_sh_pass_toggled(bool checked);
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_gen_pass_clicked();
    void on_pb_confgen_clicked();
    void on_pb_secInfo_clicked();
    void build_info_model(const char *Pwd);

private:
    Ui::AddEntry *ui;

    void EnableOkButton();
    void PasswordWarning();

    bool EqPass;

    double e, elog;
    ZxcMatch_t *Info, *p;
    QStandardItemModel *model=0;      //model for the subpass
    QStandardItemModel *model_whole=0;//model for first pass only
    QStandardItemModel *model_multi=0;//model for multi bits
    QStandardItemModel *model_times=0;//model for crack times
    QList<QStandardItem *> items;
    QString currentPass;

    const QStringList attacksType = QStringList() << "Throttled online attack"
                                                  << "Unthrottled online attack"
                                                  << "Offline attack, slow hash, many cores"
                                                  << "Offline attack, fast hash, many cores";

    const QStringList attacksTime_s = QStringList() << "100 / hour"
                                                    << "10  / sec"
                                                    << "10K / sec"
                                                    << "10B / sec";


    const double attacksTime[4] = {-1.55623,1,4,10};

};

#endif // ADDENTRY_H

