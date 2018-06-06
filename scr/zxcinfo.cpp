#include "zxcinfo.h"
#include "ui_zxcinfo.h"

zxcInfo::zxcInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::zxcInfo)
{
    ui->setupUi(this);
}

zxcInfo::zxcInfo(QWidget *parent, QString info) :
    QDialog(parent),
    ui(new Ui::zxcInfo)
{
    ui->setupUi(this);
    ui->lb_info->setText(info);
    setModal(true);
}

zxcInfo::~zxcInfo()
{
    delete ui;
}
