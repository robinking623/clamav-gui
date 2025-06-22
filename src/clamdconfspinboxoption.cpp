#include "clamdconfspinboxoption.h"
#include "ui_clamdconfspinboxoption.h"
#define css_mono "background-color:#404040;color:white"

clamdconfspinboxoption::clamdconfspinboxoption(QWidget *parent, QString keyword, bool checked, QString label, QString options) :
    QWidget(parent),
    ui(new Ui::clamdconfspinboxoption),
    optionKeyword(keyword)
{
    startup = true;

    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/clamd.conf",this);
    setupFileHandler * m_baseSetup = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);

    languageset = m_baseSetup->getSectionValue("Setup","language");
    translator * m_trans = new translator(languageset);

    QString m_helper;

    ui->setupUi(this);
    ui->checkBox->setChecked(checked);

    QStringList values = options.split(",");
    int m_spinvalue;
    int m_spinmin;
    int m_spinmax;
    m_helper = values.at(0);
    m_spinmin = m_helper.toInt();
    m_helper = values.at(1);
    m_spinmax = m_helper.toInt();
    m_helper = values.at(2);
    m_spinvalue = m_helper.toInt();

    if (m_spinvalue > 1048576) {
        m_spinmin = m_spinmin / 1048576;
        m_spinmax = m_spinmax / 1048576;
        m_spinvalue = m_spinvalue / 1048576;
    }
    ui->spinBox->setMinimum(m_spinmin);
    ui->spinBox->setMaximum(m_spinmax);
    connect(ui->checkBox,SIGNAL(clicked(bool)),this,SLOT(slot_checkboxClicked()));

    if (setupFile->singleLineExists(optionKeyword) == true) {
        ui->checkBox->setChecked(true);
        QString m_valueHelper = setupFile->getSingleLineValue(optionKeyword);
        if (m_valueHelper.indexOf("M") != -1) {
            ui->label->setText("MB");
            m_valueHelper = m_valueHelper.replace("M","");
        }
        ui->spinBox->setValue(m_valueHelper.toInt());
    } else {
        ui->spinBox->setValue(m_spinvalue);
    }

    label = m_trans->translateit(label);
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
        bool m_state = ui->checkBox->isChecked();
        ui->spinBox->setEnabled(m_state);
        ui->label->setEnabled(m_state);
        (m_state == true)?ui->frame->setStyleSheet(css_mono):ui->frame->setStyleSheet("");
        if (m_state == true) {
            if (ui->label->text() == "MB") {
                setupFile->setSingleLineValue(optionKeyword,QString::number(ui->spinBox->value()) + "M");
            } else {
                setupFile->setSingleLineValue(optionKeyword,QString::number(ui->spinBox->value()));
            }
        } else {
            QString m_value = setupFile->getSingleLineValue(optionKeyword);
            setupFile->removeSingleLine(optionKeyword,m_value);
        }
        emit settingChanged();
    }
}

void clamdconfspinboxoption::slot_spinboxChanged()
{
    if (startup == false) {
        bool m_state = ui->checkBox->isChecked();
        if (m_state == true) {
            QString m_value = setupFile->getSingleLineValue(optionKeyword);
            setupFile->removeSingleLine(optionKeyword,m_value);
            if (ui->label->text() == "MB") {
                setupFile->setSingleLineValue(optionKeyword,QString::number(ui->spinBox->value()) + "M");
            } else {
                setupFile->setSingleLineValue(optionKeyword,QString::number(ui->spinBox->value()));
            }
            emit settingChanged();
        }
    }
}
