#ifndef PASSWORDINFO_H
#define PASSWORDINFO_H

#include <QDialog>
#include <QStandardItemModel>
#include <QTableView>

namespace Ui {
class zxcInfo;
}

class passwordInfo : public QDialog
{
    Q_OBJECT

public:
    explicit passwordInfo(QWidget *parent = 0);
    explicit passwordInfo(QWidget *parent = 0, QStandardItemModel* model=NULL, QStandardItemModel* model_whole=NULL, QStandardItemModel* model_multi=NULL, QStandardItemModel* model_times=NULL);
    ~passwordInfo();

private:
    Ui::zxcInfo *ui;
    void verticalResizeTableViewToContents(QTableView *tableView);
    void resizeHoriz(QTableView *tableView);

private slots:
    void resizeOnce_caller();
    void on_tabWidget_currentChanged(int index);

protected:
    void resizeEvent(QResizeEvent *event) override;


};


#endif // PASSWORDINFO_H
