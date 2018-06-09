#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

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
    void l_1cap_clicked();
    void l_1num_clicked();
    void l_1spec_clicked();
    void l_noAmb_clicked();
    void l_noCap_clicked();
    void l_noNum_clicked();
    void l_noVow_clicked();
    void l_random_clicked();

    void on_pb_genGen_clicked();

private:
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
