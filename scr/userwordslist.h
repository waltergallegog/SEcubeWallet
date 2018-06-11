#ifndef USERWORDSLIST_H
#define USERWORDSLIST_H

#include <QDialog>

namespace Ui {
class userWordsList;
}

class userWordsList : public QDialog
{
    Q_OBJECT

public:
    explicit userWordsList(QWidget *parent = 0);
    ~userWordsList();

    QStringList getChecked();
    QStringList getAll();

private slots:
    void on_pb_delete_clicked();

private:
    Ui::userWordsList *ui;
};

#endif // USERWORDSLIST_H
