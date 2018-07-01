#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include "dictlist.h"
#include "edituserdict.h"
#include "userwordslist.h"

#include <QSettings>
#include <QDebug>
#include <QProcess>
#include <QFileDialog>


PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    setWindowTitle( tr("SEcubeWallet Settings") );
    setModal(true); //Modal, so user cannot access main window without first closing this one

    QSettings settings;

    ui->cb_gens->setCurrentIndex(settings.value("passGens/gen").toInt());

    ui->le_pwgenLen->setText(settings.value("passGens/pwgen/len").toString());

    ui->cb_1cap->setChecked(settings.value("passGens/pwgen/1cap").toBool());
    ui->cb_1num->setChecked(settings.value("passGens/pwgen/1num").toBool());
    ui->cb_1spec->setChecked(settings.value("passGens/pwgen/1spec").toBool());
    ui->cb_noAmb->setChecked(settings.value("passGens/pwgen/noAmb").toBool());
    ui->cb_noCap->setChecked(settings.value("passGens/pwgen/noCap").toBool());
    ui->cb_noNum->setChecked(settings.value("passGens/pwgen/noNum").toBool());
    ui->cb_noVow->setChecked(settings.value("passGens/pwgen/noVow").toBool());
    ui->cb_random->setChecked(settings.value("passGens/pwgen/random").toBool());

    //passphrase gen settings
    ui->sb_words->setValue(settings.value("passGens/ppgen/numWords").toInt());
    ui->cb_minLen->setChecked(settings.value("passGens/ppgen/minLenEnab").toBool());
    ui->sb_minLen->setValue(settings.value("passGens/ppgen/minLen").toInt());
    ui->cb_lower->setChecked(settings.value("passGens/ppgen/lowerEnab").toBool());
    ui->sb_lower->setValue(settings.value("passGens/ppgen/lower").toInt());

    if(settings.value("passGens/ppgen/path").toString().isEmpty())
        ui->pb_chDicPhra->setEnabled(false);



    connect(ui->l_1cap  , SIGNAL(clicked()), this, SLOT(l_1cap_clicked()));
    connect(ui->l_1num  , SIGNAL(clicked()), this, SLOT(l_1num_clicked()));
    connect(ui->l_1spec , SIGNAL(clicked()), this, SLOT(l_1spec_clicked()));
    connect(ui->l_noAmb , SIGNAL(clicked()), this, SLOT(l_noAmb_clicked()));
    connect(ui->l_noCap , SIGNAL(clicked()), this, SLOT(l_noCap_clicked()));
    connect(ui->l_noNum , SIGNAL(clicked()), this, SLOT(l_noNum_clicked()));
    connect(ui->l_noVow , SIGNAL(clicked()), this, SLOT(l_noVow_clicked()));
    connect(ui->l_random, SIGNAL(clicked()), this, SLOT(l_random_clicked()));

    //TODO: include set environment in preferences and add qsettings?
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_buttonBox_accepted(){

    QSettings settings;

    //// ***** Password generator settings.
    settings.setValue("passGens/gen", ui->cb_gens->currentIndex());

    //pwgen settings
    settings.setValue("passGens/pwgen/len", ui->le_pwgenLen->text());
    settings.setValue("passGens/pwgen/1cap", ui->cb_1cap->isChecked());
    settings.setValue("passGens/pwgen/1num", ui->cb_1num->isChecked());
    settings.setValue("passGens/pwgen/1spec", ui->cb_1spec->isChecked());
    settings.setValue("passGens/pwgen/noAmb", ui->cb_noAmb->isChecked());
    settings.setValue("passGens/pwgen/noCap", ui->cb_noCap->isChecked());
    settings.setValue("passGens/pwgen/noNum", ui->cb_noNum->isChecked());
    settings.setValue("passGens/pwgen/noVow", ui->cb_noVow->isChecked());
    settings.setValue("passGens/pwgen/random", ui->cb_random->isChecked());

    //passphrase gen settings
    settings.setValue("passGens/ppgen/numWords", ui->sb_words->value());
    settings.setValue("passGens/ppgen/minLenEnab", ui->cb_minLen->isChecked());
    settings.setValue("passGens/ppgen/minLen", ui->sb_minLen->value());
    settings.setValue("passGens/ppgen/lowerEnab", ui->cb_lower->isChecked());
    settings.setValue("passGens/ppgen/lower", ui->sb_lower->value());

    this->accept();//

}

//just to keep generators combobox and toolbox in sync
void PreferencesDialog::on_cb_gens_currentIndexChanged(int index){
    if (index<2)
        ui->tb_passgens->setCurrentIndex(index);

}

// As qcheckbox text cannot be wrapped, a textless checkbox plus a label was used. When the label is clicked the
// correspondent checkbox is togled
void PreferencesDialog::l_1cap_clicked(){
    ui->cb_1cap->toggle();
}

void PreferencesDialog::l_1num_clicked(){
    ui->cb_1num->toggle();
}

void PreferencesDialog::l_1spec_clicked(){
    ui->cb_1spec->toggle();
}

void PreferencesDialog::l_noAmb_clicked(){
    ui->cb_noAmb->toggle();
}

void PreferencesDialog::l_noCap_clicked(){
    ui->cb_noCap->toggle();
}


void PreferencesDialog::l_noNum_clicked(){
    ui->cb_noNum->toggle();
}

void PreferencesDialog::l_noVow_clicked(){
    ui->cb_noVow->toggle();
}

void PreferencesDialog::l_random_clicked(){
    ui->cb_random->toggle();
}
// end of lable_clicked => checkbox togled


//// ****** Passphrase Generator preferences **** ////

void PreferencesDialog::on_pb_chDirPhra_clicked(){
    QSettings settings;
    QString zxcvbn_path = QCoreApplication::applicationDirPath().
            append("/../../SEcubeWallet/zxcvbn/");

    QString path = QFileDialog::getExistingDirectory(this, "select directory", zxcvbn_path, 0);
    settings.setValue("passGens/ppgen/path",path);

    if(path.isEmpty())
        ui->pb_chDicPhra->setEnabled(false);
    else
        ui->pb_chDicPhra->setEnabled(true);
}


void PreferencesDialog::on_pb_chDicPhra_clicked(){
    QSettings settings;
    QString path = settings.value("passGens/ppgen/path").toString();
    if(path.isEmpty())
        return;

    dictList *list = new dictList(this, settings.value("passGens/ppgen/dicts").toStringList(), path);
    list->exec();
    if(list->result()==QDialog::Rejected)
        return; // If error or cancel, do nothing

    QStringList dicts = list->getChecked();
    settings.setValue("passGens/ppgen/dicts", dicts);

    QStringList dictsLen;//list containig the total number of words for each dictionary, in string so we can save it to settings easily
    int lines=0;
    int totalLen=0;
    foreach (const QString dict, dicts) {
        QFile inputFile(path+"/"+dict);
        if (inputFile.open(QIODevice::ReadOnly| QIODevice::Text)){
            lines=0;
            QTextStream in(&inputFile);
            while (!in.atEnd()){
                in.readLine();
                lines++;
            }
            inputFile.close();
            dictsLen << QString::number(lines);
            totalLen+=lines;
        }
    }
    qDebug() << dictsLen;
    settings.setValue("passGens/ppgen/dictsLen", dictsLen);
    settings.setValue("passGens/ppgen/dictsLenTotal", totalLen);
}



/// **** zxcvbn preferences buttons *****
//TODO: change settings names to format /esti/zxcvbn/
void PreferencesDialog::on_pb_genGen_clicked(){

    QProcess sh;
    QString zxcvbn_make_path = QCoreApplication::applicationDirPath().
            append("/../../SEcubeWallet/zxcvbn/");
    QString cd = "cd "+zxcvbn_make_path;
    QString make_command = QString(" make WORDS='");
    QSettings settings;

    foreach (const QString dict, settings.value("genDict").toStringList()) {
        make_command.append(dict).append(" ");
    }
    make_command.append("'");

    QString cdandmake = cd+" && make clean && "+make_command;
    qDebug() << cdandmake;

    setAllEnabled(false);//generate process takes a while, so disable all buttons.
    qApp->processEvents(); // otherwise the repaint take place after this function finishes
    sh.start("sh", QStringList() << "-c" << cdandmake);
    sh.waitForFinished();
    setAllEnabled(true);

    QString output(sh.readAllStandardOutput());
    qDebug()<<output;
    QString err(sh.readAllStandardError());
    qDebug()<<err;
}

void PreferencesDialog::on_pb_genChoose_clicked(){
    QSettings settings;
    dictList *list = new dictList(this, settings.value("genDict").toStringList(),QString());
    list->exec();
    if(list->result()==QDialog::Rejected)
        return; // If error or cancel, do nothing


    settings.setValue("genDict", list->getChecked());
}

void PreferencesDialog::on_pb_genClean_clicked(){
    QProcess sh;
    QString zxcvbn_make_path = QCoreApplication::applicationDirPath().
            append("/../../SEcubeWallet/zxcvbn/");
    QString cd = "cd "+zxcvbn_make_path;
    QString make_command = QString(" make clean");

    QString cdandmake = cd+" && "+make_command;
    qDebug() << cdandmake;

    setAllEnabled(false); //clean process takes a while, so disable all buttons.
    qApp->processEvents(); // otherwise the repaint take place after this function finishes
    sh.start("sh", QStringList() << "-c" << cdandmake);
    sh.waitForFinished();
    setAllEnabled(true);

    QString output(sh.readAllStandardOutput());
    qDebug().noquote() <<output;
    QString err(sh.readAllStandardError());
    qDebug().noquote() <<err;

}

void PreferencesDialog::setAllEnabled(bool enabled){
    ui->buttonBox->setEnabled(enabled);
    ui->tabWidget->setEnabled(enabled);
}

void PreferencesDialog::on_pb_userBrowse_clicked(){

    QSettings settings;

    QString fileName = QFileDialog::getOpenFileName(this, "select filename",
                                                    QCoreApplication::applicationDirPath()
                                                    .append("/../../SEcubeWallet/zxcvbn/"),
                                                    "Plain Text Files(words*.txt)");
    if (fileName.isEmpty())
        return;

    editUserDict *edit = new editUserDict (this,fileName);
    edit->exec();
    if(edit->result()==QDialog::Rejected)
        return; // If error or cancel, do nothing

    QStringList current = settings.value("userDict").toStringList();
    QStringList checked = settings.value("userDictChecked").toStringList();
    foreach (const QString word, edit->getList()){
        if(!current.contains(word,Qt::CaseInsensitive)){
            current.append(word);
            checked.append(word);//by default checked
        }
    }

    settings.setValue("userDict", current);
    settings.setValue("userDictChecked", checked);
}

void PreferencesDialog::on_pb_userMan_clicked(){

    QSettings settings;

    editUserDict *edit = new editUserDict (this);
    edit->exec();
    if(edit->result()==QDialog::Rejected)
        return; // If error or cancel, do nothing

    QStringList current = settings.value("userDict").toStringList();
    QStringList checked = settings.value("userDictChecked").toStringList();
    foreach (const QString word, edit->getList()){
        if(!current.contains(word,Qt::CaseInsensitive)){
            current.append(word);
            checked.append(word);//by default checked
        }
    }

    settings.setValue("userDict", current);
    settings.setValue("userDictChecked", checked);
}

void PreferencesDialog::on_pb_userCurrent_clicked(){

    QSettings settings;
    QStringList current = settings.value("userDict").toStringList();
    current.sort();

    userWordsList *list = new userWordsList(this);
    list->exec();
    if(list->result()==QDialog::Rejected)
        return; // If error or cancel, do nothing

    settings.setValue("userDict", list->getAll());
    settings.setValue("userDictChecked", list->getChecked());

}



