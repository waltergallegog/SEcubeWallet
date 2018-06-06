#include "zxcinfo.h"
#include "ui_zxcinfo.h"


zxcInfo::zxcInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::zxcInfo)
{
    ui->setupUi(this);
}

zxcInfo::zxcInfo(QWidget *parent, QStandardItemModel* model) :
    QDialog(parent),
    ui(new Ui::zxcInfo)
{
    ui->setupUi(this);
    //ui->lb_info->setText(info);
    ui->tv_info->setModel(model);
    ui->tv_info->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    setModal(true);
}

zxcInfo::~zxcInfo()
{
    delete ui;
}
