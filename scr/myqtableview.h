#ifndef MYQTABLEVIEW_H
#define MYQTABLEVIEW_H

#include <QTableView>



class MyQTableView : public QTableView
{
    Q_OBJECT

public:
    MyQTableView(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent* event) override;
};



#endif // MYQTABLEVIEW_H
