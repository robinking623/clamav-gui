#include "clamdconfstringoption.h"
#include "ui_clamdconfstringoption.h"
#define css_mono "background-color:#404040;color:white"

clamdConfStringOption::clamdConfStringOption(QWidget *parent, QString keyword, bool checked, QString label, QString options) :
    QWidget(parent),
    ui(new Ui::clamdConfStringOption),
    optionKeyword(keyword),
    optionValue(options)
{
    startup = true;
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/clamd.conf",this);
    setupFileHandler * m_baseSetup = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);

    languageset = m_baseSetup->getSectionValue("Setup","language");
    translator * m_trans = new translator(languageset);

    ui->setupUi(this);

    if (setupFile->singleLineExists(keyword) == true) {
        ui->checkBox->setChecked(true);
        ui->lineEdit->setText(setupFile->getSingleLineValue(keyword));
    } else {
        ui->checkBox->setChecked(checked);
        ui->lineEdit->setText(options);
    }

    label = m_trans->translateit(label);
    label = translator::beautifyString(label,120);

    ui->checkBox->setText(label);

    startup = false;
    slot_checkBoxClicked();
}

clamdConfStringOption::~clamdConfStringOption()
{
    delete ui;
}

void clamdConfStringOption::slot_checkBoxClicked()
{
    if (startup == false) {
        bool state = ui->checkBox->isChecked();
        ui->lineEdit->setEnabled(state);
        if (state == true) {
            QString m_value = setupFile->getSingleLineValue(optionKeyword);
            setupFile->removeSingleLine(optionKeyword,m_value);
            setupFile->setSingleLineValue(optionKeyword,ui->lineEdit->text());
        } else {
            QString m_value = setupFile->getSingleLineValue(optionKeyword);
            setupFile->removeSingleLine(optionKeyword,m_value);
        }
        emit settingChanged();
        (state == true)?ui->frame->setStyleSheet(css_mono):ui->frame->setStyleSheet("");
    }
}

void clamdConfStringOption::slot_lineEditChanged()
{
    if (startup == false) {
        if (ui->checkBox->isChecked() == true){
            QString m_value = setupFile->getSingleLineValue(optionKeyword);
            setupFile->removeSingleLine(optionKeyword,m_value);
            setupFile->setSingleLineValue(optionKeyword,ui->lineEdit->text());
            emit settingChanged();
        }
    }
}
