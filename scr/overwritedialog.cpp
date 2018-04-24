#include "overwritedialog.h"
#include "ui_overwritedialog.h"

OverwriteDialog::OverwriteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OverwriteDialog)
{
    ui->setupUi(this);
    setWindowTitle( tr("Confirm OverWrite") );
    setModal( true );
}

OverwriteDialog::~OverwriteDialog()
{
    delete ui;
}
