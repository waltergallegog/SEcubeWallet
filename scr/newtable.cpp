#include "newtable.h"
#include "ui_newtable.h"

NewTable::NewTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTable)
{
    ui->setupUi(this);

    setWindowTitle( tr("New Table") );
    setModal(true); //Modal, so user cannot access main window without first closing this one
}

NewTable::~NewTable()
{
    delete ui;
}

QString NewTable::getName(){
    return ui->walletName->text();
}
//TODO: allow Ok button only when something in text field
