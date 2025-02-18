#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QMessageBox>
#include <QStringList>
#include <QString>

class translator
{
public:
    translator(QString lang);
    QString languageset;
    QString translateit(QString original);
    static QString beautifyString(QString value, int length = 50);
};

#endif // TRANSLATOR_H
