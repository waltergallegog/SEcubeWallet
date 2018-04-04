#ifndef NEWTABLE_H
#define NEWTABLE_H

#include <QDialog>

namespace Ui {
class NewTable;
}

class NewTable : public QDialog
{
    Q_OBJECT

public:
    explicit NewTable(QWidget *parent = 0);
    ~NewTable();

    QString getName();

private:
    Ui::NewTable *ui;
};

#endif // NEWTABLE_H
