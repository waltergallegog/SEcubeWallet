#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QSettings>
#include <QDebug>


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

    connect(ui->l_1cap  , SIGNAL(clicked()), this, SLOT(l_1cap_clicked()));
    connect(ui->l_1num  , SIGNAL(clicked()), this, SLOT(l_1num_clicked()));
    connect(ui->l_1spec , SIGNAL(clicked()), this, SLOT(l_1spec_clicked()));
    connect(ui->l_noAmb , SIGNAL(clicked()), this, SLOT(l_noAmb_clicked()));
    connect(ui->l_noCap , SIGNAL(clicked()), this, SLOT(l_noCap_clicked()));
    connect(ui->l_noNum , SIGNAL(clicked()), this, SLOT(l_noNum_clicked()));
    connect(ui->l_noVow , SIGNAL(clicked()), this, SLOT(l_noVow_clicked()));
    connect(ui->l_random, SIGNAL(clicked()), this, SLOT(l_random_clicked()));

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

    this->accept();//

}

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
