#include "zxcinfo.h"
#include "ui_zxcinfo.h"

#include <QFont>
#include <QTableView>
#include <QScrollBar>
#include <QDebug>
#include <QTimer>
#include <QSignalMapper>

zxcInfo::zxcInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::zxcInfo)
{
    ui->setupUi(this);
}

zxcInfo::zxcInfo(QWidget *parent, QStandardItemModel* model, QStandardItemModel *model_whole, QStandardItemModel* model_multi, QStandardItemModel *model_times) :
    QDialog(parent),
    ui(new Ui::zxcInfo)
{
    ui->setupUi(this);
    setModal(true);

    //ui->lb_info->setText(info);

    ui->tv_pass->setModel(model_whole);
    QFont font (ui->tv_pass->font());
    font.setBold(true);
    ui->tv_pass->setFont(font);
    ui->tv_pass->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->tv_info->setModel(model);
    ui->tv_info->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tv_info->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tv_info->horizontalHeader()->setVisible(false);


    ui->tv_multi->setModel(model_multi);
    ui->tv_multi->horizontalHeader()->setVisible(false);

    verticalResizeTableViewToContents(ui->tv_pass);
    verticalResizeTableViewToContents(ui->tv_multi);
    QTimer::singleShot(0,this,SLOT(resizeOnce_caller()));


    ui->tv_times->setModel(model_times);
    ui->tv_times->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalResizeTableViewToContents(ui->tv_times);

}

zxcInfo::~zxcInfo()
{
    delete ui;
}

void zxcInfo::verticalResizeTableViewToContents(QTableView *tableView)
{
    int rowTotalHeight=0;

    //    if (tableView->horizontalScrollBar()->isVisible()){
    //         rowTotalHeight+=tableView->horizontalScrollBar()->height();
    //    }
    int count=tableView->verticalHeader()->count();
    for (int i = 0; i < count; ++i) {
        // 2018-03 edit: only account for row if it is visible
        if (!tableView->verticalHeader()->isSectionHidden(i)) {
            rowTotalHeight+=tableView->verticalHeader()->sectionSize(i);
        }
    }



    if (!tableView->horizontalHeader()->isHidden())
    {
        rowTotalHeight+=tableView->horizontalHeader()->height();
    }

    tableView->setFixedHeight(rowTotalHeight+2);

}

void zxcInfo::resizeOnce_caller(){
    resizeHoriz(ui->tv_pass);
    resizeHoriz(ui->tv_multi);

    //    if (!ui->tv_info->verticalScrollBar()->isVisible())
    //        verticalResizeTableViewToContents(ui->tv_info);
}

void zxcInfo::resizeHoriz(QTableView *tableView){

    //    tableView->horizontalScrollBar()->setVisible(false);

    int count = tableView->horizontalHeader()->count();
    for (int i = 0; i < count; ++i)
        tableView->horizontalHeader()->resizeSection(i, ui->tv_info->horizontalHeader()->sectionSize(i));

    int size = ui->tv_info->verticalHeader()->sizeHint().width();
    tableView->verticalHeader()->setFixedWidth(size);

    //    tableView->horizontalScrollBar()->setVisible(true);


}

void zxcInfo::resizeEvent(QResizeEvent* event){

    //    static bool scrollState = false;

    resizeHoriz(ui->tv_pass);
    resizeHoriz(ui->tv_multi);

    //    if( ui->tv_pass->horizontalScrollBar()->isVisible()!=scrollState){
    //        verticalResizeTableViewToContents(ui->tv_pass);
    //        verticalResizeTableViewToContents(ui->tv_multi);
    //        scrollState=ui->tv_pass->horizontalScrollBar()->isVisible();

    //    }
    //    if (!ui->tv_info->verticalScrollBar()->isVisible())
    //        verticalResizeTableViewToContents(ui->tv_info);
    //    else{
    //        ui->tv_info->setMinimumHeight(0);
    //        ui->tv_info->setMaximumHeight(this->height());
    //    }

    QDialog::resizeEvent(event);

}


void zxcInfo::on_tabWidget_currentChanged(int index){
    if (!index){
        resizeHoriz(ui->tv_pass);
        resizeHoriz(ui->tv_multi);
    }
}
