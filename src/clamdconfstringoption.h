#ifndef CLAMDCONFSTRINGOPTION_H
#define CLAMDCONFSTRINGOPTION_H

#include <QWidget>
#include "translator.h"
#include "setupfilehandler.h"


namespace Ui {
class clamdConfStringOption;
}

class clamdConfStringOption : public QWidget
{
    Q_OBJECT

public:
    explicit clamdConfStringOption(QWidget *parent = nullptr,QString keyword="", bool checked=false, QString label="", QString options="");
    ~clamdConfStringOption();

private:
    Ui::clamdConfStringOption *ui;
    QString optionKeyword;
    QString optionValue;
    bool startup;
    QString languageset;
    setupFileHandler    * setupFile;

private slots:
    void slot_checkBoxClicked();
    void slot_lineEditChanged();

signals:
    void settingChanged();
};

#endif // CLAMDCONFSTRINGOPTION_H
