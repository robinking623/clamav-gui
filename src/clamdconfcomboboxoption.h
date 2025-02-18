#ifndef CLAMDCONFCOMBOBOXOPTION_H
#define CLAMDCONFCOMBOBOXOPTION_H

#include <QWidget>
#include <QMessageBox>
#include "setupfilehandler.h"
#include "translator.h"

namespace Ui {
class clamdconfcomboboxoption;
}

class clamdconfcomboboxoption : public QWidget
{
    Q_OBJECT

public:
    explicit clamdconfcomboboxoption(QWidget *parent = nullptr, QString keyword="", bool checked=false, QString label="", QString options="");
    ~clamdconfcomboboxoption();

private:
    Ui::clamdconfcomboboxoption *ui;
    QString optionKeyword;
    bool startup;
    QString languageset;
    setupFileHandler    * setupFile;

private slots:
    void slot_checkBoxClicked();
    void slot_comboBoxChanged();

signals:
    void settingChanged();
};

#endif // CLAMDCONFCOMBOBOXOPTION_H
