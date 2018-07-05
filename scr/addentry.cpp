#include "addentry.h"
#include "ui_addentry.h"

#include <QLineEdit>
#include <QString>
#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QAbstractButton>
#include <QtMath>
#include <QLibrary>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QFile>
//#include <QtAlgorithms>

#include "preferencesdialog.h"
#include "passwordInfo.h"
#include "passphrasegen.h"

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


#define OK_BUTTON 0
#define UNUSED(expr) (void)(expr)

//TODO: crash when no password has been entered and details is clicked

//Default constructor, called from add entry
AddEntry::AddEntry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddEntry){

    ui->setupUi(this);
    setWindowTitle( tr("Fillout the new entry") );
    setModal(true); //Modal, so user cannot access main window without first closing this one

    load_zxcvbn_dicts();

    ui->buttonBox->buttons()[OK_BUTTON]->setEnabled(false); //Ok button initially disabled, as user has not entered any data
    ui->pb_secInfo->setEnabled(false);
    ui->InPass2->setEnabled(false);
    ui->sh_pass->setEnabled(false);
    ui->match_pass->setVisible(false);
    ui->score->setValue(0);

    QSettings settings;
    if(settings.value("passGen/gen").toInt()==NONE)
        ui->gen_pass->setEnabled(false);

}

// Second constructor, called from eddit entry
AddEntry::AddEntry(QWidget *parent, QString EditUserIn, QString EditPassIn, QString EditDomIn, QString EditDescIn) :
    QDialog(parent),
    ui(new Ui::AddEntry){

    ui->setupUi(this);
    setWindowTitle( tr("Edit the entry") );
    setModal(true);

    load_zxcvbn_dicts();
    //Initialize fields with the data to edit
    ui->InUser->setText(EditUserIn);
    ui->InDomain->setText(EditDomIn);
    ui->InPass->setText(EditPassIn);
    ui->InPass2->setText(EditPassIn);
    ui->InDesc->setText(EditDescIn);

    QSettings settings;
    if(settings.value("passGen/gen").toInt()==NONE)
        ui->gen_pass->setEnabled(false);

}

AddEntry::~AddEntry(){
    delete ui;
}

void AddEntry::load_zxcvbn_dicts(){

    // Load zxcvbn library and resolve functions. General dictionaries are included in the sources
    QString zxcvbn_lib_path = QCoreApplication::applicationDirPath().append("/../../SEcubeWallet/zxcvbn/libzxcvbn");
    if (zxcvbnLib){
        zxcvbnLib->unload();
        ZxcvbnMatch = 0;
        ZxcvbnFreeInfo = 0;
        free(zxcvbnLib);
    }
    zxcvbnLib = new QLibrary(zxcvbn_lib_path);

    if(zxcvbnLib->load()){
        qDebug()<<"ok load";
        ZxcvbnMatch = (ZxcvbnMatch_type) zxcvbnLib->resolve("ZxcvbnMatch");
        qDebug()<< "match " << ZxcvbnMatch;
        ZxcvbnFreeInfo =(ZxcvbnFreeInfo_type) zxcvbnLib->resolve("ZxcvbnFreeInfo");
        qDebug()<< "free " <<  ZxcvbnFreeInfo;
    }

    if (!ZxcvbnMatch || !ZxcvbnFreeInfo ){//if any of the two functions did not resolved correctly
        ui->pb_secInfo->setEnabled(false);//info button not necessary
        ui->score->setTextVisible(true);
        ui->score->setFormat("No zxcvbn Library");
        ui->score->setEnabled(false);
        ui->lb_secLevel->clear();
        ui->label_level->setEnabled(false);
    }
    else{
        ui->pb_secInfo->setEnabled(true);//info button not necessary
        ui->score->setTextVisible(true);
        ui->score->setFormat("%v");
        ui->score->setEnabled(true);
        ui->label_level->setEnabled(true);
    }


    // Load user dictionaries from settings
    QSettings settings;
    QStringList userDictList= settings.value("userDictChecked").toStringList();
    userDict = new char*[userDictList.size()+1];
    int i=0;
    foreach(QString s, userDictList){
        userDict[i] = new char[s.toLatin1().size()+1];
        strcpy(userDict[i], s.toLatin1().data());
        i++;
    }
    userDict[i]=0; // last entry of char**userdict must be zero (like argv), so zxcvbn knows where to stop
    for (i=0;i<userDictList.size()+1;i++)
        qDebug()<<userDict[i];
}

void AddEntry::clean(){
    if(model){
        model->clear();// as we are going to build a new model, delete the old
        delete(model);

        model_whole->clear();
        delete(model_whole);

        model_multi->clear();
        delete(model_multi);
    }

    qDebug() << "unload "<<zxcvbnLib->unload();
    free(userDict);
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
        ui->lb_length->setText(QStringLiteral("(%1)").arg(text.length()));
        if (ZxcvbnMatch){
            e = ZxcvbnMatch(text.toLatin1().constData(), const_cast<const char**>(userDict), &Info); //entropy bits in base 2
            elog = e*LOG102;
            qDebug() << e << elog;

            if (elog < 3) //very weak password
                Level = 0;
            else if (elog<6)
                Level = 1;
            else if (elog<8)
                Level = 2;
            else if (elog<10)
                Level = 3;
            else
                Level = 4;

            if (Level>-1){
                ui->score->setTextVisible(true);
                ui->score->setValue(Level);
                ui->lb_secLevel->setText(messages[Level]);
                ui->pb_secInfo->setEnabled(true);
            }
        }
    }
    else{
        ui->InPass2->setEnabled(false);
        ui->sh_pass->setEnabled(false);
        ui->lb_length->clear();
        ui->pb_secInfo->setEnabled(false);
        if (ZxcvbnMatch){
            ui->score->setValue(0);
            ui->score->setTextVisible(false);
            ui->lb_secLevel->clear();
        }

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


void AddEntry::on_buttonBox_accepted(){
    if (EqPass)
        this->accept(); //Only emith accept() when passwords are equal
}

// if user wants to generate a password using one of the installed password generators.
void AddEntry::on_gen_pass_clicked(){

    QSettings settings;

    QString genPass =""; //generated password

    //PWGEN vars
    QString options = ""; //-a options does not do anything, just to have always 4 arguments
    QString length = "16";
    char* buf;

    //PASSPHRASE vars
    QString path;
    QStringList dicts;
    QStringList dictsLen;
    int totalLen;
    int numWords;
    bool ppgenMinLenEnab;
    int ppgenMinLen;
    bool capFirst;
    bool ppgenLowerEnab;
    int ppgenLower;

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
        free(buf);

        //if succesfully finishes, change passwords
        if(!genPass.isEmpty()){
            ui->InPass->setText(genPass);
            ui->InPass2->setText(genPass);
        }

        break;


    case PASSPHRASE:

        //read values configured in preference dialog
        path            = settings.value("passGens/ppgen/path").toString();
        dicts           = settings.value("passGens/ppgen/dicts").toStringList();
        dictsLen        = settings.value("passGens/ppgen/dictsLen").toStringList();
        numWords        = settings.value("passGens/ppgen/numWords").toInt();
        ppgenMinLenEnab = settings.value("passGens/ppgen/minLenEnab").toBool();
        ppgenMinLen     = settings.value("passGens/ppgen/minLen").toInt();
        capFirst        = settings.value("passGens/ppgen/cap").toBool();
        ppgenLowerEnab  = settings.value("passGens/ppgen/lowerEnab").toBool();
        ppgenLower      = settings.value("passGens/ppgen/lower").toInt();

        if (path.isEmpty()){//check path is not empty
            QMessageBox::information(
                        this,
                        "SEcubeWallet",
                        "Passphrase generator is not configured correctly");
            break;
        }

        if(dicts.isEmpty()){//check there are selected dicts
            QMessageBox::information(
                        this,
                        "SEcubeWallet",
                        "Passphrase generator has no dictionaries, please add some");
            break;
        }

        if(dicts.length()!=dictsLen.length()){//check both list are the same length
            QMessageBox::information(
                        this,
                        "SEcubeWallet",
                        "Passphrase generator: There is an error with the dictionaries, plese try selecting them again");
            break;
        }

        if (!numWords>0)//if user did not choose at least 1 word, then default to 4
            numWords=4;

        totalLen = dictsLen.last().toInt();//last dictsLen is total
        if (ppgenLowerEnab)//only use ppgenLower part of the dictionaries
            totalLen = qRound(0.01*ppgenLower*totalLen);

        if (totalLen<numWords){//if not enough words in the dictionary
            QMessageBox::information(
                        this,
                        "SEcubeWallet",
                        "Passphrase generator: Dictionaries seem to be empty, or too small for the numbWords");
            break;
        }

        //if all the test passed, call the function (in other file) that reads the dicts and extracts the words
        genPass = PassPhraseGen(path, dicts, dictsLen, totalLen, numWords,
                    ppgenMinLenEnab, ppgenMinLen, capFirst, ppgenLowerEnab, ppgenLower);

        //if succesfully finishes, change passwords
        if (!genPass.isEmpty()){
            ui->InPass->setText(genPass);
            ui->InPass2->setText(genPass);
        }

        break;

    case NONE:
        QMessageBox::information(
                    this,
                    "SEcubeWallet",
                    "Please select a Password Generator in settings");
        break;

    default:
        break;
    }

}


void AddEntry::on_pb_confgen_clicked(){
    PreferencesDialog *pref = new PreferencesDialog(this);
    pref->exec();
    QSettings settings;
    settings.sync();

    load_zxcvbn_dicts();//load again
    on_InPass_textChanged(ui->InPass->text());//update zxcvbn display


    if(settings.value("passGen/gen").toInt()==NONE)
        ui->gen_pass->setEnabled(false);
    else
        ui->gen_pass->setEnabled(true);
}

void AddEntry::on_pb_secInfo_clicked(){


    if(!(!currentPass.compare(ui->InPass->text()))){ //if password changed from the last info click
        currentPass = ui->InPass->text();
        QByteArray tempB = currentPass.toLatin1();
        const char* Pwd = tempB.constData();
        if(model){
            model->clear();// as we are going to build a new model, delete the old
            delete(model);

            model_whole->clear();
            delete(model_whole);

            model_multi->clear();
            delete(model_multi);
        }
        build_info_model(Pwd);//only called once per different password
    }

    passwordInfo *info = new passwordInfo(this, model, model_whole, model_multi, model_times);
    info->exec();
}

void AddEntry::build_info_model(const char* Pwd){

    QString temp, temp2;
    QStringList headers;
    int n;
    double m;

    p = Info;
    model = new QStandardItemModel();
    model_whole = new QStandardItemModel();
    model_multi = new QStandardItemModel();
    model_times = new QStandardItemModel();

    //// ********** model for crack times table ********

    headers << "Type of Attack" << "Guesses per time" << "Time for cracking";
    model_times->setHorizontalHeaderLabels(headers);
    double to_crack;

    for (int i=0; i<4;i++){
        items.clear();
        items.append(new QStandardItem(attacksType.at(i)));
        items.append(new QStandardItem(attacksTime_s.at(i)));
        items.last()->setTextAlignment(Qt::AlignCenter);

        to_crack = pow10(elog - attacksTime[i]); // seconds to crack passw
        qDebug() <<to_crack;
        if (to_crack<1)
            items.append(new QStandardItem("less than a second"));
        else if (to_crack <60){
            temp.sprintf("%.0f seconds", to_crack);
            items.append(new QStandardItem(temp));
        }
        else if ((to_crack/=60) <60){//now in minutes, less than an hour
            temp.sprintf("%.0f minutes", to_crack);
            items.append(new QStandardItem(temp));
        }
        else if ((to_crack/=60) < 24){//now in hours, less than a day
            temp.sprintf("%.0f hours", to_crack);
            items.append(new QStandardItem(temp));
        }
        else if ((to_crack/=24) < 30){//now in days, less than a month
            temp.sprintf("%.0f days", to_crack);
            items.append(new QStandardItem(temp));
        }
        else if ((to_crack/=30) < 12){//now in months, less than a year
            temp.sprintf("%.0f months", to_crack);
            items.append(new QStandardItem(temp));
        }
        else{
            to_crack/=12;
            temp.sprintf("%.0f years", to_crack);
            items.append(new QStandardItem(temp));
        }
        items.last()->setTextAlignment(Qt::AlignRight);

        model_times->appendRow(items);
    }



    //// ********* models for Break down table ************

    headers.clear();
    headers << "Password" << "Type" << "Length" <<  "Entropy bits" <<  "Log entropy";
    model_whole->setHorizontalHeaderLabels(headers);
    model->setHorizontalHeaderLabels(headers);

    items.clear();
    items.append(new QStandardItem(ui->InPass->text()));
    items.append(new QStandardItem("-"));
    items.last()->setTextAlignment(Qt::AlignCenter);

    temp.sprintf("%d", ui->InPass->text().length());
    items.append(new QStandardItem(temp));
    items.last()->setTextAlignment(Qt::AlignRight);

    temp.sprintf("%6.3f", e);
    items.append(new QStandardItem(temp));
    items.last()->setTextAlignment(Qt::AlignRight);

    temp.sprintf("%.2f", elog);
    items.append(new QStandardItem(temp));
    items.last()->setTextAlignment(Qt::AlignRight);

    model_whole->insertRow(0,items);

    headers.clear();
    headers << " ";
    model_whole->setVerticalHeaderLabels(headers);





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
        items.last()->setTextAlignment(Qt::AlignRight);

        temp.sprintf("%6.3f", p->Entrpy);
        items.append(new QStandardItem(temp));
        items.last()->setTextAlignment(Qt::AlignRight);

        temp.sprintf("%.2f", p->Entrpy * LOG102);
        items.append(new QStandardItem(temp));
        items.last()->setTextAlignment(Qt::AlignRight);



        model->appendRow(items);
        m += p->Entrpy;

        p = p->Next;
    }


    items.clear();
    items.append(new QStandardItem("Multi-word"));
    items.append(new QStandardItem("-"));
    items.last()->setTextAlignment(Qt::AlignCenter);
    items.append(new QStandardItem("-"));
    items.last()->setTextAlignment(Qt::AlignCenter);

    temp.sprintf("%6.3f", e-m);
    items.append(new QStandardItem(temp));
    items.last()->setTextAlignment(Qt::AlignRight);

    temp.sprintf("%.2f", (e-m)*LOG102);
    items.append(new QStandardItem(temp));
    items.last()->setTextAlignment(Qt::AlignRight);

    model_multi->appendRow(items);
    headers.clear();
    headers << " ";
    model_multi->setVerticalHeaderLabels(headers);

    if(ZxcvbnFreeInfo)
        ZxcvbnFreeInfo(Info);
}


void AddEntry::on_pb_l33t_clicked(){

    QString tmp = ui->InPass->text();

    tmp.replace(QString("o"), QString("0"), Qt::CaseInsensitive);
    tmp.replace(QString("i"), QString("1"), Qt::CaseInsensitive);
    tmp.replace(QString("z"), QString("2"), Qt::CaseInsensitive);
    tmp.replace(QString("E"), QString("3"));
    tmp.replace(QString("A"), QString("4"));
    tmp.replace(QString("s"), QString("5"), Qt::CaseInsensitive);
    tmp.replace(QString("b"), QString("6"));
    tmp.replace(QString("T"), QString("7"));
    tmp.replace(QString("B"), QString("8"));
    tmp.replace(QString("P"), QString("9"));

    tmp.replace(QString("l"), QString("|"));

    if(EqPass)
        ui->InPass2->setText(tmp);
    ui->InPass->setText(tmp);
}

void AddEntry::on_pb_unl33t_clicked(){
    QString tmp = ui->InPass->text();

    tmp.replace(QString("0"), QString("o"));
    tmp.replace(QString("1"), QString("i"));
    tmp.replace(QString("2"), QString("z"));
    tmp.replace(QString("3"), QString("E"));
    tmp.replace(QString("4"), QString("A"));
    tmp.replace(QString("5"), QString("s"));
    tmp.replace(QString("6"), QString("b"));
    tmp.replace(QString("7"), QString("T"));
    tmp.replace(QString("8"), QString("B"));
    tmp.replace(QString("9"), QString("P"));

    tmp.replace(QString("|"), QString("l"));


    if(EqPass)
        ui->InPass2->setText(tmp);
    ui->InPass->setText(tmp);

}
