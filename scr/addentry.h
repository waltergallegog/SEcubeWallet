#ifndef ADDENTRY_H
#define ADDENTRY_H

#include <QDialog>
#include <QString>
#include <QAbstractButton>
#include <QDialogButtonBox>

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

private:
    Ui::AddEntry *ui;

    void EnableOkButton();
    void PasswordWarning();

    bool EqPass;

    //const char*  Line;
    //QString textCopy;
};

#endif // ADDENTRY_H
