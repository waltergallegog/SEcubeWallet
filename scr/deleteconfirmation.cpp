#include "deleteconfirmation.h"
#include "ui_deleteconfirmation.h"

//Nothing to see here, just a simple dialog, configuration is done in the UI editor.
DeleteConfirmation::DeleteConfirmation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteConfirmation)
{
    ui->setupUi(this);

    setWindowTitle( tr("Confrim Delete") );
    setModal( true );
}

DeleteConfirmation::~DeleteConfirmation(){
    delete ui;
}
