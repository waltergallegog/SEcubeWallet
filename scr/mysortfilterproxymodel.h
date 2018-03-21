#ifndef MYSORTFILTERPROXYMODEL_H
#define MYSORTFILTERPROXYMODEL_H

#include <QDate>
#include <QSortFilterProxyModel>

class MySortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    MySortFilterProxyModel(QObject *parent = 0);

    void setFilterUser(const QString &regExp);
    void setFilterDomain(const QString &regExp);
    void setFilterDesc(const QString &regExp);
    void setFilterOlder(bool isCustom, int index);

private:
    QRegExp userRegExp;
    QRegExp domainRegExp;
    QRegExp descRegExp;
    QDate   filterDate;
    QString format;
    int vec_months[6]={-1,-2,-4,-8,-12,-30};

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

};

#endif // MYSORTFILTERPROXYMODEL_H
