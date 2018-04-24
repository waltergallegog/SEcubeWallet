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
    void setFilterPass(const QString &regExp);
    void setFilterDesc(const QString &regExp);
    void setFilterOlder(int index, const QString &QuantityS);

private:
    QRegExp userRegExp;
    QRegExp domainRegExp;
    QRegExp passRegExp;
    QRegExp descRegExp;
    QDate   filterDate;
    QString format;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

};

#endif // MYSORTFILTERPROXYMODEL_H
