#include "helpwindow.h"
#include "ui_helpwindow.h"

helpWindow::helpWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::helpWindow){
    ui->setupUi(this);
    setWindowTitle( tr("Help") );
}

helpWindow::~helpWindow()
{
    delete ui;
}
