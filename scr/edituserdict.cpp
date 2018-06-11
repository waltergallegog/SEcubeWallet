#include "edituserdict.h"
#include "ui_edituserdict.h"

editUserDict::editUserDict(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editUserDict)
{
    ui->setupUi(this);
    setWindowTitle("Add words");
    setModal(true);
}

editUserDict::editUserDict(QWidget *parent, QString fileName) :
    QDialog(parent),
    ui(new Ui::editUserDict)
{
    ui->setupUi(this);
    setWindowTitle(QString("Edit file ").append(fileName));
    setModal(true);


    file = new QFile(fileName);
    file->open(QFile::ReadOnly | QFile::Text);
    ui->pt_userDict->setPlainText(file->readAll());
}

QStringList editUserDict::getList(){
    return ui->pt_userDict->toPlainText().split(QRegExp("\\s+"), QString::SkipEmptyParts);//split by any type of whitespace

}

editUserDict::~editUserDict()
{
    delete ui;
}
