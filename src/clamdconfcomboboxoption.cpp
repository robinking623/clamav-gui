#include "clamdconfcomboboxoption.h"
#include "ui_clamdconfcomboboxoption.h"
#define css_mono "background-color:#404040;color:white"

clamdconfcomboboxoption::clamdconfcomboboxoption(QWidget *parent, QString keyword, bool checked, QString label, QString options) :
    QWidget(parent),
    ui(new Ui::clamdconfcomboboxoption),
    optionKeyword(keyword)
{
    startup = true;
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/clamd.conf",this);
    setupFileHandler * baseSetup = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);

    languageset = baseSetup->getSectionValue("Setup","language");
    translator * trans = new translator(languageset);

    ui->setupUi(this);
    ui->checkBox->setChecked(checked);
    QStringList comboBoxValues = options.split(",");

    if (comboBoxValues.length() > 0) {
        for (int i = 0; i < comboBoxValues.length()-1; i++) {
            ui->comboBox->addItem(comboBoxValues.at(i));
        }

        if (setupFile->singleLineExists(optionKeyword) == true) {
            ui->comboBox->setCurrentText(setupFile->getSingleLineValue(optionKeyword));
            ui->checkBox->setChecked(true);
        } else {
            ui->comboBox->setCurrentText(comboBoxValues.at(comboBoxValues.length()-1));
        }
    } else {
        ui->comboBox->setVisible(false);
    }

    label = trans->translateit(label);
    ui->checkBox->setText(translator::beautifyString(label,120));

    startup = false;

    slot_checkBoxClicked();
}

clamdconfcomboboxoption::~clamdconfcomboboxoption()
{
    delete ui;
}

void clamdconfcomboboxoption::slot_checkBoxClicked()
{
    if (startup == false) {
        bool state = ui->checkBox->isChecked();

        ui->comboBox->setEnabled(state);
        (state == true)?ui->frame->setStyleSheet(css_mono):ui->frame->setStyleSheet("");
        if (state == true) {
            QString value = setupFile->getSingleLineValue(optionKeyword);
            setupFile->removeSingleLine(optionKeyword,value);
            setupFile->setSingleLineValue(optionKeyword,ui->comboBox->currentText());
        } else {
            QString value = setupFile->getSingleLineValue(optionKeyword);
            setupFile->removeSingleLine(optionKeyword,value);
        }
        emit settingChanged();
    }
}

void clamdconfcomboboxoption::slot_comboBoxChanged()
{
    if (startup == false) {
        QString value = setupFile->getSingleLineValue(optionKeyword);
        if (setupFile->singleLineExists(optionKeyword) == true) setupFile->removeSingleLine(optionKeyword,value);
        setupFile->setSingleLineValue(optionKeyword,ui->comboBox->currentText());
        emit settingChanged();
    }
}
