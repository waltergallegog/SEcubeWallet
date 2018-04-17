#include "myqtableview.h"
#include <QDebug>
#include <QHeaderView>

#define IDENT_COL   0
#define USER_COL    1
#define DOM_COL     2
#define PASS_COL    3
#define DATE_COL    4
#define DESC_COL    5

#define QD qDebug()

MyQTableView::MyQTableView (QWidget *parent)
    : QTableView(parent)
{

}

//// Override the resizeEvent to resize the columns smoothly when the user changes the main windows' size

void MyQTableView::resizeEvent(QResizeEvent* event)
{
    static int walletWidth, walletWidthOld=0;
    static int getsmod = USER_COL;
    int userWidth = this->horizontalHeader()->sectionSize(USER_COL);

    if (userWidth){ //just to know if table was already created
        walletWidthOld = walletWidth;
        walletWidth = width();

        if (walletWidthOld){
            int diff = walletWidth-walletWidthOld;
            int totalWidth=0;
            int col;
            for (col=USER_COL;col<=DESC_COL; col++)//Get total space ocup. by columns
                totalWidth+= horizontalHeader()->sectionSize(col);

            if ((totalWidth<walletWidth && diff<0 && diff>totalWidth-walletWidth) || (totalWidth+3>walletWidth && diff>0 && diff<=totalWidth-walletWidth+3)) {}

            else{
                if(totalWidth<walletWidth && diff<0 && diff<totalWidth-walletWidth)
                    diff +=walletWidth-totalWidth;
                if(totalWidth>walletWidth && diff>0 && diff>=totalWidth-walletWidth-1)
                    diff -=totalWidth-walletWidth;

                int div = diff / 5;
                int mod = diff % 5;

                //one of the columns gets the module
                horizontalHeader()->resizeSection(getsmod, div + mod + horizontalHeader()->sectionSize(getsmod));
                //add to each column the correspo. value.
                for (col=USER_COL;col<=DESC_COL; col++)
                    if (col!=getsmod)
                        horizontalHeader()->resizeSection(col, div + horizontalHeader()->sectionSize(col));

                getsmod++;
                if(getsmod>DESC_COL) getsmod = USER_COL;//restart

            }

        }
    }

    QTableView::resizeEvent(event);
}
