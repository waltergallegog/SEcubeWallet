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
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_buttonBox_accepted(){

    QSettings settings;
    QString currentGen;

    switch (ui->cb_gens->currentIndex()){

    case PWGEN:
        currentGen = ui->le_pwgenpath->text();
        currentGen.append(" ");
        if (ui->cbx_pwgenCustom->isChecked())
            currentGen.append(ui->le_pwgenCustom->text());
        else{
            if(!ui->cb_randOrPron->currentText().compare("Random"))
                currentGen.append(" -s ");
            int len = ui->le_pwgenLen->text().toInt();
            if(len)
                currentGen.append(QString::number(len));
            else
                currentGen.append(" 16 ");
            currentGen.append(" 1 ");
        }



        break;
    case PWQGEN:
        break;
    case CUSTOM:
        break;
    case NONE:

    default:
        break;


    }
    qDebug() << currentGen;
    settings.setValue("passGen/current", currentGen);

    this->accept();//

}
