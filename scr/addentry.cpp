#include "addentry.h"
#include "ui_addentry.h"

#define OK_BUTTON 0
#define UNUSED(expr) (void)(expr)

//Default constructor, called from add entry
AddEntry::AddEntry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddEntry){

    ui->setupUi(this);
    ui->PassDoNot->setVisible(false);
    ui->buttonBox->buttons()[0]->setEnabled(false); //Ok button initially disabled, as user has not entered any data
    setWindowTitle( tr("Fillout the new entry") );
    setModal(true); //Modal, so user cannot access main window without first closing this one.
}

// Second constructor, called from eddit entry
AddEntry::AddEntry(QWidget *parent, QString EditUserIn, QString EditPassIn, QString EditDomIn ) :
    QDialog(parent),
    ui(new Ui::AddEntry)
{
    ui->setupUi(this);
    //Initialize fields with the data to edit
    ui->InUser->setText(EditUserIn);
    ui->InDomain->setText(EditDomIn);
    ui->InPass->setText(EditPassIn);
    ui->InPass2->setText(EditPassIn);

    ui->PassDoNot->setVisible(false);
    ui->buttonBox->buttons()[OK_BUTTON]->setEnabled(false);
    setWindowTitle( tr("Edit the entry") );
    setModal(true);
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

// Each time the user modifies one of the text fields, we check if all of them have data, so we can enable Ok button
void AddEntry::EnableOkButton(){
    bool ok = !ui->InDomain->text().isEmpty()
    && !ui->InUser->text().isEmpty()
    && !ui->InPass->text().isEmpty()
    && !ui->InPass2->text().isEmpty();

    ui->buttonBox->buttons()[OK_BUTTON]->setEnabled(ok);
}

// If passwords are not equal, warning message
void AddEntry::PasswordWarning(){
    if (ui->InPass->text() == ui->InPass2->text())
        EqPass=true; // Flag, to allow ok button
    else
        EqPass=false;
    ui->PassDoNot->setVisible(!EqPass);
}

// If the user wants to show the passwords, change EchoMode
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

void AddEntry::on_InUser_textChanged(const QString &arg1){
    EnableOkButton();
    UNUSED(arg1);
}

void AddEntry::on_InDomain_textChanged(const QString &arg1){
    EnableOkButton();
    UNUSED(arg1);
}

void AddEntry::on_InPass_textChanged(const QString &arg1){
    EnableOkButton();
    PasswordWarning();
    UNUSED(arg1);
}

void AddEntry::on_InPass2_textChanged(const QString &arg1){
    EnableOkButton();
    PasswordWarning();
    UNUSED(arg1);
}

void AddEntry::on_buttonBox_clicked(QAbstractButton* button){
    if (EqPass)
        this->accept(); //Only emith accept() when passwords are equal
    UNUSED(button);
}
