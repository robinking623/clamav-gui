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
    setupFileHandler * m_baseSetup = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);

    languageset = m_baseSetup->getSectionValue("Setup","language");
    translator * m_trans = new translator(languageset);

    ui->setupUi(this);
    ui->checkBox->setChecked(checked);
    QStringList m_comboBoxValues = options.split(",");

    if (m_comboBoxValues.length() > 0) {
        for (int i = 0; i < m_comboBoxValues.length()-1; i++) {
            ui->comboBox->addItem(m_comboBoxValues.at(i));
        }

        if (setupFile->singleLineExists(optionKeyword) == true) {
            ui->comboBox->setCurrentText(setupFile->getSingleLineValue(optionKeyword));
            ui->checkBox->setChecked(true);
        } else {
            ui->comboBox->setCurrentText(m_comboBoxValues.at(m_comboBoxValues.length()-1));
        }
    } else {
        ui->comboBox->setVisible(false);
    }

    label = m_trans->translateit(label);
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
            QString m_value = setupFile->getSingleLineValue(optionKeyword);
            setupFile->removeSingleLine(optionKeyword,m_value);
            setupFile->setSingleLineValue(optionKeyword,ui->comboBox->currentText());
        } else {
            QString m_value = setupFile->getSingleLineValue(optionKeyword);
            setupFile->removeSingleLine(optionKeyword,m_value);
        }
        emit settingChanged();
    }
}

void clamdconfcomboboxoption::slot_comboBoxChanged()
{
    if (startup == false) {
        QString m_value = setupFile->getSingleLineValue(optionKeyword);
        if (setupFile->singleLineExists(optionKeyword) == true) setupFile->removeSingleLine(optionKeyword,m_value);
        setupFile->setSingleLineValue(optionKeyword,ui->comboBox->currentText());
        emit settingChanged();
    }
}
