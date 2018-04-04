#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QDialog>

namespace Ui {
class helpWindow;
}

class helpWindow : public QDialog
{
    Q_OBJECT

public:
    explicit helpWindow(QWidget *parent = 0);
    ~helpWindow();

private:
    Ui::helpWindow *ui;
};

#endif // HELPWINDOW_H
