#include "addentry.h"
#include "ui_addentry.h"

#include <QLineEdit>
#include <QString>
#include <QDebug>
#include <QProcess>
#include <QSettings>

#include <QAbstractButton>
#include "zxcvbn.h"
//TODO: put zxcvbn in a subdir

#define OK_BUTTON 0
#define UNUSED(expr) (void)(expr)

//Default constructor, called from add entry
AddEntry::AddEntry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddEntry){

    ui->setupUi(this);
    ui->buttonBox->buttons()[OK_BUTTON]->setEnabled(false); //Ok button initially disabled, as user has not entered any data
    ui->InPass->setEnabled(true);
    ui->InPass2->setEnabled(false);
    ui->InPass->setEchoMode(QLineEdit::Password);
    ui->InPass2->setEchoMode(QLineEdit::Password);
    ui->sh_pass->setEnabled(false);
    ui->match_pass->setVisible(false);
    ui->score->setValue(0);

    setWindowTitle( tr("Fillout the new entry") );
    setModal(true); //Modal, so user cannot access main window without first closing this one

    //open dictionry. Only once

    if (!ZxcvbnInit()){
        qDebug() << "Failed Open Dictionary File";
    }else{
        qDebug() << "Dictionary File Opened Correctly";
    }
}

// Second constructor, called from eddit entry
AddEntry::AddEntry(QWidget *parent, QString EditUserIn, QString EditPassIn, QString EditDomIn, QString EditDescIn) :
    QDialog(parent),
    ui(new Ui::AddEntry)
{
    ui->setupUi(this);
    //Initialize fields with the data to edit
    ui->InUser->setText(EditUserIn);
    ui->InDomain->setText(EditDomIn);
    ui->InPass->setText(EditPassIn);
    ui->InPass2->setText(EditPassIn);
    ui->InDesc->setText(EditDescIn);


    if (!ZxcvbnInit()){
        qDebug() << "Failed Open Dictionary File";
    }else{
        qDebug() << "Dictionary File Opened Correctly";
    }

    ui->InPass2->setEnabled(true);
    ui->sh_pass->setEnabled(true);

    double e = ZxcvbnMatch(EditPassIn.toLatin1().constData(), NULL, 0);

    ui->score->setValue(e);

    ui->buttonBox->buttons()[OK_BUTTON]->setEnabled(true);
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


QString AddEntry::getDescription(){
    return ui->InDesc->text();
}


/// Text changed in fields.

void AddEntry::on_InUser_textChanged(const QString &arg1){
    EnableOkButton();
    UNUSED(arg1);
}

void AddEntry::on_InDomain_textChanged(const QString &arg1){
    EnableOkButton();
    UNUSED(arg1);
}

void AddEntry::on_InPass2_textChanged(const QString &text){

    EnableOkButton();
    PasswordWarning();
    UNUSED(text);
}

void AddEntry::on_InPass_textChanged(const QString &text){

    EnableOkButton();
    PasswordWarning();

    if(!text.isEmpty()){
        ui->InPass2->setEnabled(true);
        ui->sh_pass->setEnabled(true);

        double e = ZxcvbnMatch(text.toLatin1().constData(), NULL, 0);
        ui->score->setValue(e);

    }else{
        ui->InPass2->setEnabled(false);
        ui->sh_pass->setEnabled(false);
        ui->score->setValue(0);
    }
}



// Each time the user modifies one of the text fields, we check if all of them have data, so we can enable Ok button
void AddEntry::EnableOkButton(){
    bool ok = !ui->InUser->text().isEmpty()
           && !ui->InDomain->text().isEmpty()
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
    ui->match_pass->setVisible(!EqPass && ui->InPass2->isEnabled());
}

// If the user wants to show the passwords, change EchoMode
void AddEntry::on_sh_pass_toggled(bool checked){
    if (checked){
        ui->InPass->setEchoMode(QLineEdit::Normal);
        ui->InPass2->setEchoMode(QLineEdit::Normal);
    }
    else{
        ui->InPass->setEchoMode(QLineEdit::Password);
        ui->InPass2->setEchoMode(QLineEdit::Password);
    }
}


void AddEntry::on_buttonBox_clicked(QAbstractButton* button){
    if (EqPass)
        this->accept(); //Only emith accept() when passwords are equal
    UNUSED(button);
}


// if user wants to generate a password using one of the installed password generators.
void AddEntry::on_gen_pass_clicked(){

    QSettings settings;
    if (!settings.contains("passGen/current")){// no passgen has been configured and enabled
        return;
    }

    QProcess process;
    process.start(settings.value("passGen/current").toString());
    process.waitForFinished();
    QString output(process.readAllStandardOutput());

    //TODO: parse output correctly
    output.remove(QRegExp("[\\n\\t\\r]"));
    qDebug()<<output;

    ui->InPass->setText(output);
    ui->InPass2->setText(output);

}
