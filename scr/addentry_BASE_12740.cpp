#include "addentry.h"
#include "ui_addentry.h"

#include "zxcvbn.h"

#include <QLineEdit>
#include <QString>
#include <QDebug>

#include <QAbstractButton>

#define OK_BUTTON 0
#define UNUSED(expr) (void)(expr)

//Default constructor, called from add entry
AddEntry::AddEntry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddEntry){

    ui->setupUi(this);
    ui->buttonBox->buttons()[0]->setEnabled(false); //Ok button initially disabled, as user has not entered any data
    ui->InPass->setEnabled(true);
    ui->InPass2->setEnabled(false);
    ui->InPass->setEchoMode(QLineEdit::Password);
    ui->InPass2->setEchoMode(QLineEdit::Password);
    ui->sh_pass->setEnabled(false);
    ui->score->setTextVisible(false);
    ui->score->setValue(0);

    setWindowTitle( tr("Fillout the new entry") );
    setModal(true); //Modal, so user cannot access main window without first closing this one

    //connect(ui->InPass,SIGNAL(textChanged(QString)),this,SLOT(InPass_textChanged(QString)));
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

    if (!ZxcvbnInit()){
        qDebug() << "Failed Open Dictionary File";
    }else{
        qDebug() << "Dictionary File Opened Correctly";
    }
    //----------------------

    ui->InPass2->setEnabled(true);
    ui->sh_pass->setEnabled(true);

    const char* Line = EditPassIn.toLatin1().constData();
    double e = ZxcvbnMatch(Line, NULL, 0);
    //qDebug() << "Pass: " << text << " ; " << "Entropy: " << QString::number(e);

    ui->entropy->setText(QString::number(e));

    if(e <= 20.0){
        ui->complex->setText("shortPass");
        ui->complex2->setText("The password is too short");
        ui->score->setValue(25);

    }else if(e > 20.0 && e <= 30.0){
        ui->complex->setText("badPass");
        ui->complex2->setText("Weak; try combining letters & numbers");
        ui->score->setValue(50);

    }else if(e > 30.0 && e <= 40.0){
        ui->complex->setText("goodPass");
        ui->complex2->setText("Medium; try using special charecters");
        ui->score->setValue(75);

    }else if(e > 40.0){
        ui->complex->setText("strongPass");
        ui->complex2->setText("Strong password, good job!");
        ui->score->setValue(100);
    }

    //----------------------
    ZxcvbnUnInit();


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

/*void AddEntry::on_InUser_textChanged(const QString &arg1){
    EnableOkButton();
    UNUSED(arg1);
}*/

/*void AddEntry::on_InDomain_textChanged(const QString &arg1){
    EnableOkButton();
    UNUSED(arg1);
}*/

void AddEntry::on_InPass_textChanged(QString text){
    /*EnableOkButton();
    PasswordWarning();
    UNUSED(arg1);*/

    qDebug() << "perra";

    if(!text.isEmpty()){
        if (!ZxcvbnInit()){
            qDebug() << "Failed Open Dictionary File";
        }else{
            qDebug() << "Dictionary File Opened Correctly";
        }
        //----------------------

        ui->InPass2->setEnabled(true);
        ui->sh_pass->setEnabled(true);

        const char* Line = text.toLatin1().constData();
        double e = ZxcvbnMatch(Line, NULL, 0);
        //qDebug() << "Pass: " << text << " ; " << "Entropy: " << QString::number(e);

        ui->entropy->setText(QString::number(e));

        if(e <= 20.0){
            ui->complex->setText("shortPass");
            ui->complex2->setText("The password is too short");
            ui->score->setValue(25);

        }else if(e > 20.0 && e <= 30.0){
            ui->complex->setText("badPass");
            ui->complex2->setText("Weak; try combining letters & numbers");
            ui->score->setValue(50);

        }else if(e > 30.0 && e <= 40.0){
            ui->complex->setText("goodPass");
            ui->complex2->setText("Medium; try using special charecters");
            ui->score->setValue(75);

        }else if(e > 40.0){
            ui->complex->setText("strongPass");
            ui->complex2->setText("Strong password, good job!");
            ui->score->setValue(100);
        }

        //----------------------
        ZxcvbnUnInit();
    }else{
        ui->InPass2->setEnabled(false);
        ui->sh_pass->setEnabled(false);
        ui->score->setValue(0);
    }
}

void AddEntry::on_InPass2_textChanged(QString text){
    /*EnableOkButton();
    PasswordWarning();
    UNUSED(arg1);*/

    if(!text.isEmpty()){
        if(ui->InPass->text() == ui->InPass2->text()){
            ui->match_pass->setText("The Passwords Match");
            ui->buttonBox->buttons()[0]->setEnabled(true);
        }else{
            ui->match_pass->setText("The Passwords Do Not Match");
            ui->buttonBox->buttons()[0]->setEnabled(false);
        }

    }else{
        ui->match_pass->setText("");
        ui->buttonBox->buttons()[0]->setEnabled(false);
    }
}

void AddEntry::on_sh_pass_stateChanged(int value){
    /*if (EqPass)
        this->accept(); //Only emith accept() when passwords are equal
    UNUSED(button);*/

    if(value){
        ui->InPass->setEchoMode(QLineEdit::Normal);
        ui->InPass2->setEchoMode(QLineEdit::Normal);
    }else{
        ui->InPass->setEchoMode(QLineEdit::Password);
        ui->InPass2->setEchoMode(QLineEdit::Password);
    }
}

void AddEntry::on_buttonBox_clicked(QAbstractButton* button){
    //if (EqPass)
        //this->accept(); //Only emith accept() when passwords are equal
    UNUSED(button);
    //qDebug() << button;

    if(!ui->InUser->text().isEmpty() && !ui->InDomain->text().isEmpty() && !ui->InPass->text().isEmpty() && !ui->InPass2->text().isEmpty()){
        if(ui->InPass->text() == ui->InPass2->text()){
            this->accept();
        }
    }
}
