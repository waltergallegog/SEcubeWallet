#ifndef EDITUSERDICT_H
#define EDITUSERDICT_H

#include <QDialog>
#include <QFile>

namespace Ui {
class editUserDict;
}

class editUserDict : public QDialog
{
    Q_OBJECT

public:
    explicit editUserDict(QWidget *parent = 0);
    editUserDict(QWidget *parent, QString fileName);

    QStringList getList();

    ~editUserDict();

private:
    Ui::editUserDict *ui;

    QFile *file;
};

#endif // EDITUSERDICT_H
