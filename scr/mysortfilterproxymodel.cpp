#include <QtWidgets>
#include "mysortfilterproxymodel.h"

#define USER_COL    1
#define DOM_COL     2
#define PASS_COL    3
#define DATE_COL    4
#define DESC_COL    5

MySortFilterProxyModel::MySortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    userRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    domainRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    passRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    descRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    userRegExp.setPatternSyntax(QRegExp::RegExp);
    domainRegExp.setPatternSyntax(QRegExp::RegExp);
    passRegExp.setPatternSyntax(QRegExp::RegExp);
    descRegExp.setPatternSyntax(QRegExp::RegExp);

    format = "yyyy-MM-dd";
}

void MySortFilterProxyModel::setFilterUser(const QString &regExp){
    userRegExp.setPattern(regExp);
    invalidateFilter();
}

void MySortFilterProxyModel::setFilterDomain(const QString &regExp){
    domainRegExp.setPattern(regExp);
    invalidateFilter();
}

void MySortFilterProxyModel::setFilterPass(const QString &regExp){
    passRegExp.setPattern(regExp);
    invalidateFilter();
}

void MySortFilterProxyModel::setFilterDesc(const QString &regExp){
    descRegExp.setPattern(regExp);
    invalidateFilter();
}

void MySortFilterProxyModel::setFilterOlder(int index, const QString &QuantityS, QDate date){

    int Quantity = 0;

    if(index!=-1){
        if (!QuantityS.isEmpty())
            Quantity = QuantityS.toInt()*(-1);// change to negative to substract using add
        else
            index=0;//if empty then no filter
    }

    switch (index) {
    case 0: // no filter
        filterDate_older=QDate::fromString("-1");
        filterDate_exact=QDate::fromString("-1");
        break;
    //older than filter
    case 1:
        filterDate_older=QDate::currentDate().addDays(Quantity);
        filterDate_exact=QDate::fromString("-1");
        break;
    case 2:
        filterDate_older=QDate::currentDate().addDays(7*Quantity); //weeks
        filterDate_exact=QDate::fromString("-1");
        break;
    case 3:
        filterDate_older=QDate::currentDate().addMonths(Quantity);
        filterDate_exact=QDate::fromString("-1");
        break;
    case 4:
        filterDate_older=QDate::currentDate().addYears(Quantity);
        filterDate_exact=QDate::fromString("-1");
        break;

    //Exact date filter
    case -1:
        filterDate_exact=date;
        filterDate_older=QDate::fromString("-1");
        break;

    default:
        filterDate_exact=QDate::fromString("-1");
        filterDate_older=QDate::fromString("-1");
        break;
    }

    qDebug()<<"older: " <<filterDate_older.toString();
    qDebug()<<"exact: " <<filterDate_exact.toString();
    invalidateFilter();
}


bool MySortFilterProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    QModelIndex userIndex = sourceModel()->index(sourceRow, USER_COL, sourceParent);
    QModelIndex domainIndex = sourceModel()->index(sourceRow, DOM_COL, sourceParent);
    QModelIndex passIndex = sourceModel()->index(sourceRow, PASS_COL, sourceParent);
    QModelIndex descIndex = sourceModel()->index(sourceRow, DESC_COL, sourceParent);

    QModelIndex dateIndex = sourceModel()->index(sourceRow, DATE_COL, sourceParent);
    QDate thisDate = QDate::fromString(sourceModel()->data(dateIndex).toString(),format);

    return (    sourceModel()->data(userIndex).toString().contains(userRegExp)
            &&  sourceModel()->data(domainIndex).toString().contains(domainRegExp)
            &&  sourceModel()->data(passIndex).toString().contains(passRegExp)
            &&  sourceModel()->data(descIndex).toString().contains(descRegExp)
            &&  (thisDate <= filterDate_older || !filterDate_older.isValid())
            &&  (thisDate == filterDate_exact || !filterDate_exact.isValid())
            );
}

