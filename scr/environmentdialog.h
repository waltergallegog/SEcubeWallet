#ifndef ENVIRONMENTDIALOG_H
#define ENVIRONMENTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>


#include "SEfile.h"
#include "se3/L1.h"

#define N_KEYS 100

class EnvironmentDialog : public QDialog
{
    Q_OBJECT

private:

    se3_session session;
    QList<se3_key> key_list;
    QHash<int, se3_algo> algo_list;

    QLabel* labelEncryption;
    QLabel* labelKeyId;
    QGridLayout* formGridLayout;

    QComboBox* chooseEncryption;
    QComboBox* chooseKeyId;

    /*!
    * The standard dialog button box.
    */
    QDialogButtonBox* buttons;

    /*!
    * A method to set up all dialog components and
    * initialize them.
    */
    void setUpGUI();

public:
    explicit EnvironmentDialog(QWidget *parent = 0, se3_session *s = NULL);
//    ~EnvironmentDialog();


public slots:

    void slotAbortEnvironment();
    void slotAcceptEnvironment();
    void refreshEnvironment();
};

#endif // ENVIRONMENTDIALOG_H
