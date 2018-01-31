#ifndef DELETECONFIRMATION_H
#define DELETECONFIRMATION_H

#include <QDialog>

namespace Ui {
class DeleteConfirmation;
}

class DeleteConfirmation : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteConfirmation(QWidget *parent = 0);
    ~DeleteConfirmation();

private:
    Ui::DeleteConfirmation *ui;
};

#endif // DELETECONFIRMATION_H
