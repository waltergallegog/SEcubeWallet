#ifndef ADDENTRY_H
#define ADDENTRY_H

#include <QDialog>
#include <QString>
#include <QAbstractButton>
#include <QDialogButtonBox>

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
    explicit AddEntry(QWidget *parent, QString EditUserIn, QString EditPassIn, QString EditDomIn);

    ~AddEntry();//Destructor

    //Methods to access data from main.
    QString getUser();
    QString getDomain();
    QString getPassword();


private slots:
    //void on_InUser_textChanged(QString arg1);
    //void on_InDomain_textChanged(QString arg1);
    void on_InPass_textChanged(QString text);
    void on_InPass2_textChanged(QString text);
    void on_sh_pass_stateChanged(int value);
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::AddEntry *ui;

    //void EnableOkButton();
    //void PasswordWarning();

    //bool EqPass;
};

#endif // ADDENTRY_H
