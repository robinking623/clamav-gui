#include "clamdconfspinboxoption.h"
#include "ui_clamdconfspinboxoption.h"
#define css_mono "background-color:#404040;color:white"

clamdconfspinboxoption::clamdconfspinboxoption(QWidget *parent, QString keyword, bool checked, QString label, QString options) :
    QWidget(parent),
    ui(new Ui::clamdconfspinboxoption),
    optionKeyword(keyword)
{
    startup = true;

    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/clamd.conf", this);
    setupFileHandler baseSetup(QDir::homePath() + "/.clamav-gui/settings.ini");

    languageset = baseSetup.getSectionValue("Setup","language");
    translator * trans = new translator(languageset);

    QString helper;

    ui->setupUi(this);
    ui->checkBox->setChecked(checked);

    QStringList values = options.split(",");
    int spinvalue;
    int spinmin;
    int spinmax;
    helper = values.at(0);
    spinmin = helper.toInt();
    helper = values.at(1);
    spinmax = helper.toInt();
    helper = values.at(2);
    spinvalue = helper.toInt();

    if (spinvalue > 1048576) {
        spinmin = spinmin / 1048576;
        spinmax = spinmax / 1048576;
        spinvalue = spinvalue / 1048576;
    }
    ui->spinBox->setMinimum(spinmin);
    ui->spinBox->setMaximum(spinmax);
    connect(ui->checkBox,SIGNAL(clicked(bool)),this,SLOT(slot_checkboxClicked()));

    if (setupFile->singleLineExists(optionKeyword) == true) {
        ui->checkBox->setChecked(true);
        QString valueHelper = setupFile->getSingleLineValue(optionKeyword);
        if (valueHelper.indexOf("M") != -1) {
            ui->label->setText("MB");
            valueHelper = valueHelper.replace("M","");
        }
        ui->spinBox->setValue(valueHelper.toInt());
    } else {
        ui->spinBox->setValue(spinvalue);
    }

    label = trans->translateit(label);
    ui->checkBox->setText(translator::beautifyString(label,120));
    startup = false;

    slot_checkboxClicked();
}

clamdconfspinboxoption::~clamdconfspinboxoption()
{
    delete ui;
}

void clamdconfspinboxoption::slot_checkboxClicked()
{
    if (startup == false) {
        bool state = ui->checkBox->isChecked();
        ui->spinBox->setEnabled(state);
        ui->label->setEnabled(state);
        (state == true)?ui->frame->setStyleSheet(css_mono):ui->frame->setStyleSheet("");
        if (state == true) {
            if (ui->label->text() == "MB") {
                setupFile->setSingleLineValue(optionKeyword,QString::number(ui->spinBox->value()) + "M");
            } else {
                setupFile->setSingleLineValue(optionKeyword,QString::number(ui->spinBox->value()));
            }
        } else {
            QString value = setupFile->getSingleLineValue(optionKeyword);
            setupFile->removeSingleLine(optionKeyword,value);
        }
        emit settingChanged();
    }
}

void clamdconfspinboxoption::slot_spinboxChanged()
{
    if (startup == false) {
        bool state = ui->checkBox->isChecked();
        if (state == true) {
            QString value = setupFile->getSingleLineValue(optionKeyword);
            setupFile->removeSingleLine(optionKeyword,value);
            if (ui->label->text() == "MB") {
                setupFile->setSingleLineValue(optionKeyword,QString::number(ui->spinBox->value()) + "M");
            } else {
                setupFile->setSingleLineValue(optionKeyword,QString::number(ui->spinBox->value()));
            }
            emit settingChanged();
        }
    }
}
