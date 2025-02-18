#ifndef CLAMDCONFSPINBOXOPTION_H
#define CLAMDCONFSPINBOXOPTION_H

#include <QWidget>
#include "translator.h"
#include "setupfilehandler.h"

namespace Ui {
class clamdconfspinboxoption;
}

class clamdconfspinboxoption : public QWidget
{
    Q_OBJECT

public:
    explicit clamdconfspinboxoption(QWidget *parent = nullptr, QString keyword = "", bool checked = false, QString label = "", QString options = "0,99,1,1");
    ~clamdconfspinboxoption();


private:
    Ui::clamdconfspinboxoption *ui;
    QString optionKeyword;
    bool startup;
    QString languageset;
    setupFileHandler    * setupFile;

private slots:
    void slot_checkboxClicked();
    void slot_spinboxChanged();

signals:
    void settingChanged();
};

#endif // CLAMDCONFSPINBOXOPTION_H
