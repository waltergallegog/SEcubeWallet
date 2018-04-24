#include "passworditemdelegate.h"
#define UNUSED(expr) (void)(expr)

PasswordItemDelegate::PasswordItemDelegate(QObject* parent): QStyledItemDelegate(parent)
{

}

QString PasswordItemDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    UNUSED (locale);
    UNUSED (value);
    return (QString("%1").arg(QChar(0x2022)).repeated(8)); // just returns the bullet character 8 times
}
