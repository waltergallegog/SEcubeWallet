#ifndef COLUMNALIGNEDLAYOUT_H
#define COLUMNALIGNEDLAYOUT_H

#include <QHBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QCheckBox>

class QHeaderView;

class FiltersAligned : public QHBoxLayout
{
    Q_OBJECT
public:
    FiltersAligned();
    explicit FiltersAligned(QWidget *parent);
    void setTableColumnsToTrack(QHeaderView *view);

    QWidget* dateWidget;
    QHBoxLayout* dateLayout;
    QLineEdit *idFilter;
    QLineEdit *userFilter;
    QLineEdit *domFilter;
    QLineEdit *passFilter;
    QLineEdit *dateFilter;
    QComboBox *dateUnit;
    QLineEdit *descFilter;
    QDateEdit *dateChoose;
    QCheckBox *dateCheck;


private:
    void setGeometry(const QRect &r);
    QHeaderView *headerView;
};

#endif // COLUMNALIGNEDLAYOUT_H
