#include "addentry.h"
#include "ui_addentry.h"

#include <QLineEdit>
#include <QString>
#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QAbstractButton>

#include "preferencesdialog.h"
#include "zxcinfo.h"

extern "C"
{
#include "pwgen.h"
}

#define LOG102 0.301029996 //log in base 10 of 2
#define MESS0 "Too guessable: Risky password"
#define MESS1 "Very guessable: Protection from slow online attacks"
#define MESS2 "Somewhat guessable: Protection from fast online attacks"
#define MESS3 "Safely unguessable: moderate protection from offline slow-hash attacks"
#define MESS4 "Very unguessable: Strong protection from offline slow-hash scenario"


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
    if(model){
        model->clear();// as we are going to build a new model, delete the old
        delete(model);
    }
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

    int Level=-1;

    QString messages[5] = {MESS0, MESS1, MESS2, MESS3, MESS4};

    if(!text.isEmpty()){
        ui->InPass2->setEnabled(true);
        ui->sh_pass->setEnabled(true);

        e = ZxcvbnMatch(text.toLatin1().constData(), NULL, &Info); //entropy bits in base 2
        elog = e*LOG102;
        qDebug() << e << elog;

        if (elog < 3) //very weak password
            Level = 0;
        else if (elog<6)
            Level =1;
        else if (elog<8)
            Level =2;
        else if (elog<10)
            Level =3;
        else
            Level =4;

        if (Level>-1){
            ui->score->setTextVisible(true);
            ui->score->setValue(Level);
            ui->lb_secLevel->setText(messages[Level]);
        }

    }else{
        ui->InPass2->setEnabled(false);
        ui->sh_pass->setEnabled(false);
        ui->score->setValue(0);
        ui->score->setTextVisible(false);
        ui->lb_secLevel->clear();
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


void AddEntry::on_buttonBox_clicked(QAbstractButton* button){//TODO: fix by changing to buttonbox_accepted
    if (EqPass)
        this->accept(); //Only emith accept() when passwords are equal
    UNUSED(button);
}


// if user wants to generate a password using one of the installed password generators.
void AddEntry::on_gen_pass_clicked(){

    QSettings settings;

    //PWGEN vars

    QString options = ""; //-a options does not do anything, just to have always 4 arguments
    QString length = "16";

    char* buf;
    QString genPass;


    switch (settings.value("passGens/gen").toInt()){

    case PWGEN:

        //look for user settings to determine which options
        if (settings.value("passGens/pwgen/1cap").toBool())
            options.append("c");
        if (settings.value("passGens/pwgen/1num").toBool())
            options.append("n");
        if (settings.value("passGens/pwgen/1spec").toBool())
            options.append("y");
        if (settings.value("passGens/pwgen/noAmb").toBool())
            options.append("B");
        if (settings.value("passGens/pwgen/noCap").toBool())
            options.append("A");
        if (settings.value("passGens/pwgen/noNum").toBool())
            options.append("0");
        if (settings.value("passGens/pwgen/noVow").toBool())
            options.append("v");
        if (settings.value("passGens/pwgen/random").toBool())
            options.append("s");


        //check if user entered an integer, if not, default is 16
        if(settings.value("passGens/pwgen/len").toInt())
            length = settings.value("passGens/pwgen/len").toString();


        buf = (char*)malloc(length.toInt()+1);

        if(!buf){
            qDebug() << "Memory Allocation Failed";
            return;
        }

        //actual call to password generator
        qDebug() << options;
        main_pwgen(options.length(), options.toLatin1().constData(), length.toInt(), buf);
        genPass = QString::fromLatin1(buf,length.toInt());

        ui->InPass->setText(genPass);
        ui->InPass2->setText(genPass);

        free(buf);
        break;

    case PWQGEN:
        break;

    case CUSTOM:
        break;

    case NONE:
        break;

    default:
        break;
    }
}


void AddEntry::on_pb_confgen_clicked(){
    PreferencesDialog *pref = new PreferencesDialog(this);
    pref->exec();

}

void AddEntry::on_pb_secInfo_clicked(){


    if(!(!currentPass.compare(ui->InPass->text()))){ //if password changed from the last info click
        currentPass = ui->InPass->text();
        QByteArray tempB = currentPass.toLatin1();
        const char* Pwd = tempB.constData();
        if(model){
            model->clear();// as we are going to build a new model, delete the old
            delete(model);
        }
        build_info_model(Pwd);//only called once per different password
    }

    zxcInfo *info = new zxcInfo(this, model);
    info->exec();
}

void AddEntry::build_info_model(const char* Pwd){

    QString temp, temp2;
    QStringList headers;
    int n;
    double m;

    p = Info;
    model = new QStandardItemModel();

    headers << "subPass" << "Type" << "Length" <<  "Entropy bits" <<  "Log entropy";

    model->setHorizontalHeaderLabels(headers);

    while(p)
    {
        items.clear();

        for(n = 0; n < p->Length; ++n, ++Pwd){
            temp.sprintf("%c", *Pwd);
            temp2.append(temp);
        }
        items.append(new QStandardItem(temp2));
        temp2.clear();

        switch((int)p->Type)
        {
        case BRUTE_MATCH:                     items.append(new QStandardItem("Bruteforce"));       break;
        case DICTIONARY_MATCH:                items.append(new QStandardItem("Dictionary"));        break;
        case DICT_LEET_MATCH:                 items.append(new QStandardItem("Dict+Leet"));         break;
        case USER_MATCH:                      items.append(new QStandardItem("User Words"));        break;
        case USER_LEET_MATCH:                 items.append(new QStandardItem("User+Leet"));         break;
        case REPEATS_MATCH:                   items.append(new QStandardItem("Repeated"));          break;
        case SEQUENCE_MATCH:                  items.append(new QStandardItem("Sequence"));          break;
        case SPATIAL_MATCH:                   items.append(new QStandardItem("Spatial"));           break;
        case DATE_MATCH:                      items.append(new QStandardItem("Date"));              break;
        case BRUTE_MATCH+MULTIPLE_MATCH:      items.append(new QStandardItem("Bruteforce(Rep)"));   break;
        case DICTIONARY_MATCH+MULTIPLE_MATCH: items.append(new QStandardItem("Dictionary(Rep)"));   break;
        case DICT_LEET_MATCH+MULTIPLE_MATCH:  items.append(new QStandardItem("Dict+Leet(Rep)"));    break;
        case USER_MATCH+MULTIPLE_MATCH:       items.append(new QStandardItem("User Words(Rep)"));   break;
        case USER_LEET_MATCH+MULTIPLE_MATCH:  items.append(new QStandardItem("User+Leet(Rep)"));    break;
        case REPEATS_MATCH+MULTIPLE_MATCH:    items.append(new QStandardItem("Repeated(Rep)"));     break;
        case SEQUENCE_MATCH+MULTIPLE_MATCH:   items.append(new QStandardItem("Sequence(Rep)"));     break;
        case SPATIAL_MATCH+MULTIPLE_MATCH:    items.append(new QStandardItem("Spatial(Rep)"));      break;
        case DATE_MATCH+MULTIPLE_MATCH:       items.append(new QStandardItem("Date(Rep)"));         break;

        default:
            temp.sprintf("Unknown %d ", p->Type);
            items.append(new QStandardItem(temp));
            break;
        }
        temp.sprintf("%d", p->Length);
        items.append(new QStandardItem(temp));

        temp.sprintf("%6.3f", p->Entrpy);
        items.append(new QStandardItem(temp));

        temp.sprintf("%.2f", p->Entrpy * LOG102);
        items.append(new QStandardItem(temp));

        model->appendRow(items);
        m += p->Entrpy;

        p = p->Next;
    }

    items.clear();
    items.append(new QStandardItem(ui->InPass->text()));
    items.append(new QStandardItem("-"));

    temp.sprintf("%d", ui->InPass->text().length());
    items.append(new QStandardItem(temp));

    temp.sprintf("%6.3f", e);
    items.append(new QStandardItem(temp));

    temp.sprintf("%.2f", elog);
    items.append(new QStandardItem(temp));

    model->insertRow(0,items);

    items.clear();
    items.append(new QStandardItem("Multi-word extra bits"));
    items.append(new QStandardItem("-"));
    items.append(new QStandardItem("-"));
    temp.sprintf("%6.3f", e-m);
    items.append(new QStandardItem(temp));

    temp.sprintf("%.2f", (e-m)*LOG102);
    items.append(new QStandardItem(temp));

    model->appendRow(items);

    ZxcvbnFreeInfo(Info);
}


