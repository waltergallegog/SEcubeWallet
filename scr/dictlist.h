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
     dictList(QWidget *parent, QStringList genDict, QString path);
    ~dictList();

    QStringList getChecked();

private:
    Ui::dictList *ui;
};

#endif // DICTLIST_H
