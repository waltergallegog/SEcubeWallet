#include "addentry.h"
#include "ui_addentry.h"

AddEntry::AddEntry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddEntry){

    ui->setupUi(this);
    ui->PassDoNot->setVisible(false);
    ui->buttonBox->buttons()[0]->setEnabled(false);
    setWindowTitle( tr("Fillout the new entry") );
    setModal( true );
}

AddEntry::AddEntry(QWidget *parent, QString EditUserIn, QString EditPassIn, QString EditDomIn ) :
    QDialog(parent),
    ui(new Ui::AddEntry)
{

    ui->setupUi(this);
    ui->InUser->setText(EditUserIn);
    ui->InDomain->setText(EditDomIn);
    ui->InPass->setText(EditPassIn);
    ui->InPass2->setText(EditPassIn);

    ui->PassDoNot->setVisible(false);
    ui->buttonBox->buttons()[0]->setEnabled(false);
    setWindowTitle( tr("Fillout the new entry") );
    setModal( true );
}

AddEntry::~AddEntry(){
    delete ui;
}

QString AddEntry::getUser(){
    return ui->InUser->text();
}

QString AddEntry::getDomain(){
    return ui->InDomain->text();
}

QString AddEntry::getPassword(){
    return ui->InPass->text();
}


void AddEntry::EnableOkButton(){
    bool ok = !ui->InDomain->text().isEmpty()
    && !ui->InUser->text().isEmpty()
    && !ui->InPass->text().isEmpty()
    && !ui->InPass2->text().isEmpty();

    ui->buttonBox->buttons()[0]->setEnabled(ok);
}

void AddEntry::PasswordWarning(){
    if (ui->InPass->text() == ui->InPass2->text())
        EqPass=true;
    else
        EqPass=false;
    ui->PassDoNot->setVisible(!EqPass);
}

void AddEntry::on_InUser_textChanged(const QString &arg1){
    EnableOkButton();
}

void AddEntry::on_InDomain_textChanged(const QString &arg1){
    EnableOkButton();
}

void AddEntry::on_InPass_textChanged(const QString &arg1){
    EnableOkButton();
    PasswordWarning();
}

void AddEntry::on_InPass2_textChanged(const QString &arg1){
    EnableOkButton();
    PasswordWarning();
}


void AddEntry::on_buttonBox_clicked(QAbstractButton *button){
    if (EqPass)
        this->accept();
}

void AddEntry::on_ShowPass_toggled(bool checked)
{
    if (checked){
        ui->InPass->setEchoMode(QLineEdit::Normal);
        ui->InPass2->setEchoMode(QLineEdit::Normal);
    }
    else{
        ui->InPass->setEchoMode(QLineEdit::Password);
        ui->InPass2->setEchoMode(QLineEdit::Password);
    }
}
