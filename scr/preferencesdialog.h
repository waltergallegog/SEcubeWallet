#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#define PWGEN 0
#define PWQGEN 1
#define CUSTOM 2
#define NONE 3

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
