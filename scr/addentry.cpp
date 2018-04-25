#include "addentry.h"
#include "ui_addentry.h"

#include <QLineEdit>
#include <QString>
#include <QDebug>

#include <QAbstractButton>
#include "zxcvbn.h"

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

    const char* Line = EditPassIn.toLatin1().constData();
    double e = ZxcvbnMatch(Line, NULL, 0);
    //qDebug() << "Pass: " << text << " ; " << "Entropy: " << QString::number(e);

    ui->score->setValue(e);

    if(e <= 20.0){
        ui->complex->setText("shortPass");
        ui->complex2->setText("The password is too short");

    }else if(e > 20.0 && e <= 30.0){
        ui->complex->setText("badPass");
        ui->complex2->setText("Weak; try combining letters & numbers");

    }else if(e > 30.0 && e <= 40.0){
        ui->complex->setText("goodPass");
        ui->complex2->setText("Medium; try using special charecters");

    }else if(e > 40.0){
        ui->complex->setText("strongPass");
        ui->complex2->setText("Strong password, good job!");
    }

    //    ZxcvbnUnInit();//Finally call ZxcvbnUninit() to free the dictionary data from read from file. This can be omitted when dictionary data is included in the executable.


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

    // aditionally, we need to call strength meter
    if(!text.isEmpty()){
        ui->InPass2->setEnabled(true);
        ui->sh_pass->setEnabled(true);

        QByteArray byteArray = text.toLatin1();
        const char *Line = byteArray.constData();

        double e = ZxcvbnMatch(Line, NULL, 0);

        ui->score->setValue(e);

        if(e <= 20.0){
            ui->complex->setText("shortPass");
            ui->complex2->setText("The password is too short");


        }else if(e > 20.0 && e <= 30.0){
            ui->complex->setText("badPass");
            ui->complex2->setText("Weak; try combining letters & numbers");


        }else if(e > 30.0 && e <= 40.0){
            ui->complex->setText("goodPass");
            ui->complex2->setText("Medium; try using special charecters");


        }else if(e > 40.0){
            ui->complex->setText("strongPass");
            ui->complex2->setText("Strong password, good job!");

        }
        //ZxcvbnUnInit();//Finally call ZxcvbnUninit() to free the dictionary data from read from file. This can be omitted when dictionary data is included in the executable.
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

