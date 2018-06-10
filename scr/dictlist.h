#ifndef DICTLIST_H
#define DICTLIST_H

#include <QDialog>

namespace Ui {
class dictList;
}

class dictList : public QDialog
{
    Q_OBJECT

public:
    explicit dictList(QWidget *parent = 0);
    ~dictList();

    QStringList getChecked();

private:
    Ui::dictList *ui;
};

#endif // DICTLIST_H
