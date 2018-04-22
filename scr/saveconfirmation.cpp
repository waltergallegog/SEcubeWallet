#include "saveconfirmation.h"
#include "ui_saveconfirmation.h"

SaveConfirmation::SaveConfirmation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveConfirmation)
{
    ui->setupUi(this);
}

SaveConfirmation::~SaveConfirmation(){
    delete ui;
}

bool SaveConfirmation::getResult(){
    return saveOrDisc;
}

void SaveConfirmation::on_buttonBox_clicked(QAbstractButton *button){
    if ((QPushButton *)button==ui->buttonBox->button(QDialogButtonBox::Save))
        saveOrDisc = SAVE;

    else if ((QPushButton *)button==ui->buttonBox->button(QDialogButtonBox::Discard))
        saveOrDisc = DISCARD;

    this->accept();

}
