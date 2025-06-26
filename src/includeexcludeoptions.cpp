#include "includeexcludeoptions.h"
#include "ui_includeexcludeoptions.h"
#define css "background-color:#404040;color:white"

includeExcludeOptions::includeExcludeOptions(QWidget* parent, setupFileHandler* setupFile) : QWidget(parent), m_setupFile(setupFile), m_ui(new Ui::includeExcludeOptions)
{
    m_ui->setupUi(this);

    if (m_setupFile->sectionExists("REGEXP_and_IncludeExclude") == true) {
        m_readSettingsInProgress = true;
        readSettings();
    }

    slot_highlightSettings();
}

includeExcludeOptions::~includeExcludeOptions()
{
    delete m_ui;
}

void includeExcludeOptions::slot_writeSettings()
{
    QString section = "REGEXP_and_IncludeExclude";
    QString checked;
    QString keyword;
    QString value;
    if (m_readSettingsInProgress == false) {
        keyword = "DontScanFileNamesMatchingRegExp";
        value = m_ui->dontScanFileNameLineEdit->text();
        m_ui->dontScanFileNameCheckBox->isChecked() ? checked = "checked" : checked = "not checked";
        m_setupFile->setSectionValue(section, keyword, checked + "|" + value);

        keyword = "DontScanDiretoriesMatchingRegExp";
        value = m_ui->dontScanDirLineEdit->text();
        m_ui->dontScanDirCheckBox->isChecked() ? checked = "checked" : checked = "not checked";
        m_setupFile->setSectionValue(section, keyword, checked + "|" + value);

        keyword = "OnlyScanFileNamesMatchingRegExp";
        value = m_ui->onlyScanFileNameLineEdit->text();
        m_ui->onlyScanFileNameCheckBox->isChecked() ? checked = "checked" : checked = "not checked";
        m_setupFile->setSectionValue(section, keyword, checked + "|" + value);

        keyword = "OnlyScanDiretoriesMatchingRegExp";
        value = m_ui->onlyScanDirLineEdit->text();
        m_ui->onlyScanDirCheckBox->isChecked() ? checked = "checked" : checked = "not checked";
        m_setupFile->setSectionValue(section, keyword, checked + "|" + value);

        keyword = "EnablePUAOptions";
        m_setupFile->setSectionValue(section, keyword, m_ui->enablePUACheckBox->isChecked());

        keyword = "LoadPUAPacked";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAPackedRadioButon->isChecked());

        keyword = "LoadPUAPWTool";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAPWToolRadioButton->isChecked());

        keyword = "LoadPUANetTool";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUANetToolRadioButton->isChecked());

        keyword = "LoadPUAP2P";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAP2PRadioButton->isChecked());

        keyword = "LoadPUAIRC";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAIRCRadioButton->isChecked());

        keyword = "LoadPUARAT";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUARATRadioButton->isChecked());

        keyword = "LoadPUANetToolSpy";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUANetToolSpyRadioButton->isChecked());

        keyword = "LoadPUAServer";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAServerRadioButton->isChecked());

        keyword = "LoadPUAScript";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAScriptRadioButton->isChecked());

        keyword = "LoadPUAAndr";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAAndrRadioButton->isChecked());

        keyword = "LoadPUAJava";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAJavaRadioButton->isChecked());

        keyword = "LoadPUAOsx";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAOsxRadioButton->isChecked());

        keyword = "LoadPUATool";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAToolRadioButton->isChecked());

        keyword = "LoadPUAUnix";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAUnixRadioButton->isChecked());

        keyword = "LoadPUAWin";
        m_setupFile->setSectionValue(section, keyword, m_ui->loadPUAWinRadioButton->isChecked());

        emit updateClamdConf();
    }
}

void includeExcludeOptions::readSettings()
{
    QString section = "REGEXP_and_IncludeExclude";
    QString checked;
    QString keyword;
    QString value;

    keyword = "DontScanFileNamesMatchingRegExp";
    value = m_setupFile->getSectionValue(section, keyword);
    checked = value.left(value.indexOf("|"));
    value = value.mid(value.indexOf("|") + 1);
    checked == "checked" ? m_ui->dontScanFileNameCheckBox->setChecked(true) : m_ui->dontScanFileNameCheckBox->setChecked(false);
    m_ui->dontScanFileNameLineEdit->setText(value);

    keyword = "DontScanDiretoriesMatchingRegExp";
    value = m_setupFile->getSectionValue(section, keyword);
    checked = value.left(value.indexOf("|"));
    value = value.mid(value.indexOf("|") + 1);
    checked == "checked" ? m_ui->dontScanDirCheckBox->setChecked(true) : m_ui->dontScanDirCheckBox->setChecked(false);
    m_ui->dontScanDirLineEdit->setText(value);

    keyword = "OnlyScanFileNamesMatchingRegExp";
    value = m_setupFile->getSectionValue(section, keyword);
    checked = value.left(value.indexOf("|"));
    value = value.mid(value.indexOf("|") + 1);
    checked == "checked" ? m_ui->onlyScanFileNameCheckBox->setChecked(true) : m_ui->onlyScanFileNameCheckBox->setChecked(false);
    m_ui->onlyScanFileNameLineEdit->setText(value);

    keyword = "OnlyScanDiretoriesMatchingRegExp";
    value = m_setupFile->getSectionValue(section, keyword);
    checked = value.left(value.indexOf("|"));
    value = value.mid(value.indexOf("|") + 1);
    checked == "checked" ? m_ui->onlyScanDirCheckBox->setChecked(true) : m_ui->onlyScanDirCheckBox->setChecked(false);
    m_ui->onlyScanDirLineEdit->setText(value);

    keyword = "EnablePUAOptions";
    if (m_setupFile->getSectionBoolValue(section, keyword) == true) {
        m_ui->enablePUACheckBox->setChecked(true);
        m_ui->PUAFrame->setEnabled(true);
    }
    else {
        m_ui->enablePUACheckBox->setChecked(false);
        m_ui->PUAFrame->setEnabled(false);
    }

    keyword = "LoadPUAPacked";
    m_ui->loadPUAPackedRadioButon->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUAPWTool";
    m_ui->loadPUAPWToolRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUANetTool";
    m_ui->loadPUANetToolRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUAP2P";
    m_ui->loadPUAP2PRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUAIRC";
    m_ui->loadPUAIRCRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUARAT";
    m_ui->loadPUARATRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUANetToolSpy";
    m_ui->loadPUANetToolSpyRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUAServer";
    m_ui->loadPUAServerRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUAScript";
    m_ui->loadPUAScriptRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUAAndr";
    m_ui->loadPUAAndrRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUAJava";
    m_ui->loadPUAJavaRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUAOsx";
    m_ui->loadPUAOsxRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUATool";
    m_ui->loadPUAToolRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUAUnix";
    m_ui->loadPUAUnixRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    keyword = "LoadPUAWin";
    m_ui->loadPUAWinRadioButton->setChecked(m_setupFile->getSectionBoolValue(section, keyword));

    m_readSettingsInProgress = false;
}

void includeExcludeOptions::slot_enablePUACheckBoxClicked()
{
    m_ui->PUAFrame->setEnabled(m_ui->enablePUACheckBox->isChecked());
    slot_writeSettings();
}

void includeExcludeOptions::slot_highlightSettings()
{
    m_ui->dontScanFileNameCheckBox->isChecked() ? m_ui->frame01->setStyleSheet(css) : m_ui->frame01->setStyleSheet("");
    m_ui->dontScanDirCheckBox->isChecked() ? m_ui->frame02->setStyleSheet(css) : m_ui->frame02->setStyleSheet("");
    m_ui->onlyScanFileNameCheckBox->isChecked() ? m_ui->frame03->setStyleSheet(css) : m_ui->frame03->setStyleSheet("");
    m_ui->onlyScanDirCheckBox->isChecked() ? m_ui->frame04->setStyleSheet(css) : m_ui->frame04->setStyleSheet("");

    m_ui->loadPUAPackedRadioButon->isChecked() ? m_ui->loadPUAPackedRadioButon->setStyleSheet(css) : m_ui->loadPUAPackedRadioButon->setStyleSheet("");
    m_ui->loadPUAPWToolRadioButton->isChecked() ? m_ui->loadPUAPWToolRadioButton->setStyleSheet(css)
                                                : m_ui->loadPUAPWToolRadioButton->setStyleSheet("");
    m_ui->loadPUANetToolRadioButton->isChecked() ? m_ui->loadPUANetToolRadioButton->setStyleSheet(css)
                                                 : m_ui->loadPUANetToolRadioButton->setStyleSheet("");
    m_ui->loadPUAP2PRadioButton->isChecked() ? m_ui->loadPUAP2PRadioButton->setStyleSheet(css) : m_ui->loadPUAP2PRadioButton->setStyleSheet("");
    m_ui->loadPUAIRCRadioButton->isChecked() ? m_ui->loadPUAIRCRadioButton->setStyleSheet(css) : m_ui->loadPUAIRCRadioButton->setStyleSheet("");
    m_ui->loadPUARATRadioButton->isChecked() ? m_ui->loadPUARATRadioButton->setStyleSheet(css) : m_ui->loadPUARATRadioButton->setStyleSheet("");
    m_ui->loadPUANetToolSpyRadioButton->isChecked() ? m_ui->loadPUANetToolSpyRadioButton->setStyleSheet(css)
                                                    : m_ui->loadPUANetToolSpyRadioButton->setStyleSheet("");
    m_ui->loadPUAServerRadioButton->isChecked() ? m_ui->loadPUAServerRadioButton->setStyleSheet(css)
                                                : m_ui->loadPUAServerRadioButton->setStyleSheet("");
    m_ui->loadPUAScriptRadioButton->isChecked() ? m_ui->loadPUAScriptRadioButton->setStyleSheet(css)
                                                : m_ui->loadPUAScriptRadioButton->setStyleSheet("");
    m_ui->loadPUAAndrRadioButton->isChecked() ? m_ui->loadPUAAndrRadioButton->setStyleSheet(css) : m_ui->loadPUAAndrRadioButton->setStyleSheet("");
    m_ui->loadPUAJavaRadioButton->isChecked() ? m_ui->loadPUAJavaRadioButton->setStyleSheet(css) : m_ui->loadPUAJavaRadioButton->setStyleSheet("");
    m_ui->loadPUAOsxRadioButton->isChecked() ? m_ui->loadPUAOsxRadioButton->setStyleSheet(css) : m_ui->loadPUAOsxRadioButton->setStyleSheet("");
    m_ui->loadPUAToolRadioButton->isChecked() ? m_ui->loadPUAToolRadioButton->setStyleSheet(css) : m_ui->loadPUAToolRadioButton->setStyleSheet("");
    m_ui->loadPUAUnixRadioButton->isChecked() ? m_ui->loadPUAUnixRadioButton->setStyleSheet(css) : m_ui->loadPUAUnixRadioButton->setStyleSheet("");
    m_ui->loadPUAWinRadioButton->isChecked() ? m_ui->loadPUAWinRadioButton->setStyleSheet(css) : m_ui->loadPUAWinRadioButton->setStyleSheet("");
}
