#include "userwordslist.h"
#include "ui_userwordslist.h"

#include <QSettings>

userWordsList::userWordsList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userWordsList)
{
    ui->setupUi(this);

    setWindowTitle("Choose which user words you want to use");
    setModal(true);

    QSettings settings;
    QStringList strList = settings.value("userDict").toStringList();
    strList.sort();
    QStringList checked =  settings.value("userDictChecked").toStringList();

    ui->lw_words->addItems(strList);

    QListWidgetItem* item = 0;
    for(int i = 0; i < ui->lw_words->count(); ++i){
        item = ui->lw_words->item(i);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        if (checked.contains(item->text()))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
    }

}

userWordsList::~userWordsList()
{
    delete ui;
}

QStringList userWordsList::getChecked(){

    QStringList checked;
    QListWidgetItem* item = 0;
    for(int i = 0; i < ui->lw_words->count(); ++i){
        item = ui->lw_words->item(i);
        if (item->checkState())
            checked << item->text();
    }

    return checked;
}

QStringList userWordsList::getAll(){
    QStringList all;
    for(int i = 0; i < ui->lw_words->count(); ++i)
        all << ui->lw_words->item(i)->text();

    return all;
}


void userWordsList::on_pb_delete_clicked(){
    qDeleteAll(ui->lw_words->selectedItems());
}
