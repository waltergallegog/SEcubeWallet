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

void MySortFilterProxyModel::setFilterOlder(int index, const QString &QuantityS){

    int Quantity = 0;
    if (!QuantityS.isEmpty())
        Quantity = QuantityS.toInt()*(-1);// change to negative to substract using add
    else
        index=0;//if empty then no filter

    switch (index) {
    case 0: // no filter
        filterDate=QDate::fromString("-1");
        break;
    case 1:
        filterDate=QDate::currentDate().addDays(Quantity);
        break;
    case 2:
        filterDate=QDate::currentDate().addDays(7*Quantity); //weeks
        break;
    case 3:
        filterDate=QDate::currentDate().addMonths(Quantity);
        break;
    case 4:
        filterDate=QDate::currentDate().addYears(Quantity);
        break;
    default:
        filterDate=QDate::fromString("-1");
        break;
    }

    qDebug()<<filterDate.toString();
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
            &&  (thisDate <= filterDate || !filterDate.isValid())
            );
}
