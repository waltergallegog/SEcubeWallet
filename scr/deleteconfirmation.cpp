#include "deleteconfirmation.h"
#include "ui_deleteconfirmation.h"

DeleteConfirmation::DeleteConfirmation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteConfirmation)
{
    ui->setupUi(this);

    setWindowTitle( tr("Confrim Delete") );
    setModal( true );
}

DeleteConfirmation::~DeleteConfirmation()
{
    delete ui;
}
