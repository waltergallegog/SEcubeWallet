#ifndef SAVECONFIRMATION_H
#define SAVECONFIRMATION_H

#define SAVE 1
#define DISCARD 0

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class SaveConfirmation;
}

class SaveConfirmation : public QDialog
{
    Q_OBJECT

public:
    explicit SaveConfirmation(QWidget *parent = 0);
    ~SaveConfirmation();

    bool getResult();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::SaveConfirmation *ui;
    bool saveOrDisc;


};

#endif // SAVECONFIRMATION_H
