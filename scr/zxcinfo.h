#ifndef ZXCINFO_H
#define ZXCINFO_H

#include <QDialog>

namespace Ui {
class zxcInfo;
}

class zxcInfo : public QDialog
{
    Q_OBJECT

public:
    explicit zxcInfo(QWidget *parent = 0);
    explicit zxcInfo(QWidget *parent = 0, QString info="Empty");
    ~zxcInfo();

private:
    Ui::zxcInfo *ui;
};

#endif // ZXCINFO_H
