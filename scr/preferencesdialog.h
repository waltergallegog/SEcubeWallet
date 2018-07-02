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


    void on_cb_gens_currentIndexChanged(int index);

    void l_1cap_clicked();
    void l_1num_clicked();
    void l_1spec_clicked();
    void l_noAmb_clicked();
    void l_noCap_clicked();
    void l_noNum_clicked();
    void l_noVow_clicked();
    void l_random_clicked();



    void on_pb_chDicPhra_clicked();
    void on_pb_chDirPhra_clicked();
    void on_pb_applyPhra_clicked();

    //zxcvbn
    void on_pb_genGen_clicked();
    void on_pb_genChoose_clicked();
    void on_pb_userBrowse_clicked();
    void on_pb_userMan_clicked();
    void on_pb_userCurrent_clicked();
    void on_pb_genClean_clicked();

private:
    Ui::PreferencesDialog *ui;

    void setAllEnabled(bool enabled);


    QString ppgenPath;
    QStringList ppgenDicts;
    QStringList ppgenDictsLen;//list containig the total number of words for each dictionary, in string so we can save it to settings easily

    bool ppgenMinLenEnab;
    int ppgenMinLen;
    bool ppgenLowerEnab;
    int ppgenLower;

};

#endif // PREFERENCESDIALOG_H
