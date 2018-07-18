#include "filtersaligned.h"
#include <QHeaderView>
#include <QCheckBox>


FiltersAligned::FiltersAligned(QWidget *parent)
    : QHBoxLayout(parent){}

FiltersAligned::FiltersAligned()
    : QHBoxLayout()
{
    idFilter = new QLineEdit();//not really used, just for consistency
    userFilter = new QLineEdit();
    domFilter = new QLineEdit();
    descFilter = new QLineEdit();
    passFilter = new QLineEdit();
    passFilter->setEnabled(false);

    dateWidget = new QWidget;
    dateLayout = new QHBoxLayout;

    dateFilter = new QLineEdit();
    dateUnit = new QComboBox();
    dateUnit->addItem("-");
    dateUnit->addItem("Days");
    dateUnit->addItem("Weeks");
    dateUnit->addItem("Months");
    dateUnit->addItem("Years");

    dateChoose = new QDateEdit;
    dateChoose->setDisplayFormat("yyyy-MM-dd");
    dateChoose->setEnabled(false);
    dateCheck = new QCheckBox;
    dateCheck->setFixedWidth(dateCheck->width());

    dateWidget->setLayout(dateLayout);
    dateLayout->addWidget(dateFilter);
    dateLayout->addWidget(dateUnit);
    dateLayout->addWidget(dateCheck);
    dateLayout->addWidget(dateChoose);
    dateCheck->setFixedWidth(15);
    dateLayout->setContentsMargins(1,0,0,0);
    dateLayout->setSpacing(0);
    dateFilter->setVisible(false);
    dateUnit->setVisible(false);

    userFilter->setClearButtonEnabled(true);
    domFilter->setClearButtonEnabled(true);
    passFilter->setClearButtonEnabled(true);
    dateFilter->setClearButtonEnabled(true);
    descFilter->setClearButtonEnabled(true);

    userFilter->setPlaceholderText("Filter");
    domFilter->setPlaceholderText ("Filter");
    passFilter->setPlaceholderText("Filter");
    descFilter->setPlaceholderText("Filter");
    dateFilter->setPlaceholderText("Older");
    dateFilter->setToolTip("Display only passwords created/modified before than...");

    addWidget(idFilter);
    addWidget(userFilter);
    addWidget(domFilter);
    addWidget(passFilter);
    addWidget(dateWidget);//alignedLayout->addWidget(ui->dateFilterCombo);
    addWidget(descFilter);
    setContentsMargins(0,0,0,0);
}


void FiltersAligned::setTableColumnsToTrack(QHeaderView *view) {
    headerView = view;
}

void FiltersAligned::setGeometry(const QRect &r){
    QHBoxLayout::setGeometry(r);

    Q_ASSERT_X(headerView, "layout", "no table columns to track");
    if (!headerView) {
        return;
    }

    Q_ASSERT_X(headerView->count() == count(), "layout", "there must be as many items in the layout as there are columns in the table");
    if (headerView->count() != count()) {
        return;
    }

    Q_ASSERT(parentWidget());

    int widgetX = parentWidget()->mapToGlobal(QPoint(0, 0)).x();
    int headerX = headerView->mapToGlobal(QPoint(0, 0)).x();
    int delta = headerX - widgetX;

    for (int ii = 0; ii < headerView->count(); ++ii) {
        int pos = headerView->sectionViewportPosition(ii);
        int size = headerView->sectionSize(ii);

        auto item = itemAt(ii);
        auto r = item->geometry();
        r.setLeft(pos + delta);
        r.setWidth(size);
        item->setGeometry(r);
    }
}
