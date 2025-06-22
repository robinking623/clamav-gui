#include "includeexcludeoptions.h"
#include "ui_includeexcludeoptions.h"
#define css "background-color:#404040;color:white"

includeExcludeOptions::includeExcludeOptions(QWidget *parent) : QWidget(parent), ui(new Ui::includeExcludeOptions)
{
    ui->setupUi(this);
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    if (setupFile->sectionExists("REGEXP_and_IncludeExclude") == true){
        readSettingsInProgress = true;
        readSettings();
    }

    slot_highlightSettings();
}

includeExcludeOptions::~includeExcludeOptions()
{
    delete ui;
}

void includeExcludeOptions::slot_writeSettings(){
QString m_section = "REGEXP_and_IncludeExclude";
QString m_checked;
QString m_keyword;
QString m_value;
    if (readSettingsInProgress == false) {
        m_keyword = "DontScanFileNamesMatchingRegExp";
        m_value = ui->dontScanFileNameLineEdit->text();
        ui->dontScanFileNameCheckBox->isChecked()?m_checked="m_checked":m_checked="not m_checked";
        setupFile->setSectionValue(m_section,m_keyword,m_checked + "|" + m_value);

        m_keyword = "DontScanDiretoriesMatchingRegExp";
        m_value = ui->dontScanDirLineEdit->text();
        ui->dontScanDirCheckBox->isChecked()?m_checked="m_checked":m_checked="not m_checked";
        setupFile->setSectionValue(m_section,m_keyword,m_checked + "|" + m_value);

        m_keyword = "OnlyScanFileNamesMatchingRegExp";
        m_value = ui->onlyScanFileNameLineEdit->text();
        ui->onlyScanFileNameCheckBox->isChecked()?m_checked="m_checked":m_checked="not m_checked";
        setupFile->setSectionValue(m_section,m_keyword,m_checked + "|" + m_value);

        m_keyword = "OnlyScanDiretoriesMatchingRegExp";
        m_value = ui->onlyScanDirLineEdit->text();
        ui->onlyScanDirCheckBox->isChecked()?m_checked="m_checked":m_checked="not m_checked";
        setupFile->setSectionValue(m_section,m_keyword,m_checked + "|" + m_value);

        m_keyword = "EnablePUAOptions";
        setupFile->setSectionValue(m_section,m_keyword,ui->enablePUACheckBox->isChecked());

        m_keyword = "LoadPUAPacked";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAPackedRadioButon->isChecked());

        m_keyword = "LoadPUAPWTool";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAPWToolRadioButton->isChecked());

        m_keyword = "LoadPUANetTool";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUANetToolRadioButton->isChecked());

        m_keyword = "LoadPUAP2P";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAP2PRadioButton->isChecked());

        m_keyword = "LoadPUAIRC";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAIRCRadioButton->isChecked());

        m_keyword = "LoadPUARAT";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUARATRadioButton->isChecked());

        m_keyword = "LoadPUANetToolSpy";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUANetToolSpyRadioButton->isChecked());

        m_keyword = "LoadPUAServer";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAServerRadioButton->isChecked());

        m_keyword = "LoadPUAScript";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAScriptRadioButton->isChecked());

        m_keyword = "LoadPUAAndr";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAAndrRadioButton->isChecked());

        m_keyword = "LoadPUAJava";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAJavaRadioButton->isChecked());

        m_keyword = "LoadPUAOsx";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAOsxRadioButton->isChecked());

        m_keyword = "LoadPUATool";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAToolRadioButton->isChecked());

        m_keyword = "LoadPUAUnix";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAUnixRadioButton->isChecked());

        m_keyword = "LoadPUAWin";
        setupFile->setSectionValue(m_section,m_keyword,ui->loadPUAWinRadioButton->isChecked());

        emit updateClamdConf();
    }
}

void includeExcludeOptions::readSettings(){
QString m_section = "REGEXP_and_IncludeExclude";
QString m_checked;
QString m_keyword;
QString m_value;

    m_keyword = "DontScanFileNamesMatchingRegExp";
    m_value = setupFile->getSectionValue(m_section,m_keyword);
    m_checked = m_value.left(m_value.indexOf("|"));
    m_value = m_value.mid(m_value.indexOf("|") + 1);
    m_checked=="m_checked"?ui->dontScanFileNameCheckBox->setChecked(true):ui->dontScanFileNameCheckBox->setChecked(false);
    ui->dontScanFileNameLineEdit->setText(m_value);

    m_keyword = "DontScanDiretoriesMatchingRegExp";
    m_value = setupFile->getSectionValue(m_section,m_keyword);
    m_checked = m_value.left(m_value.indexOf("|"));
    m_value = m_value.mid(m_value.indexOf("|") + 1);
    m_checked=="m_checked"?ui->dontScanDirCheckBox->setChecked(true):ui->dontScanDirCheckBox->setChecked(false);
    ui->dontScanDirLineEdit->setText(m_value);

    m_keyword = "OnlyScanFileNamesMatchingRegExp";
    m_value = setupFile->getSectionValue(m_section,m_keyword);
    m_checked = m_value.left(m_value.indexOf("|"));
    m_value = m_value.mid(m_value.indexOf("|") + 1);
    m_checked=="m_checked"?ui->onlyScanFileNameCheckBox->setChecked(true):ui->onlyScanFileNameCheckBox->setChecked(false);
    ui->onlyScanFileNameLineEdit->setText(m_value);

    m_keyword = "OnlyScanDiretoriesMatchingRegExp";
    m_value = setupFile->getSectionValue(m_section,m_keyword);
    m_checked = m_value.left(m_value.indexOf("|"));
    m_value = m_value.mid(m_value.indexOf("|") + 1);
    m_checked=="m_checked"?ui->onlyScanDirCheckBox->setChecked(true):ui->onlyScanDirCheckBox->setChecked(false);
    ui->onlyScanDirLineEdit->setText(m_value);

    m_keyword = "EnablePUAOptions";
    if (setupFile->getSectionBoolValue(m_section,m_keyword) == true){
        ui->enablePUACheckBox->setChecked(true);
        ui->PUAFrame->setEnabled(true);
    } else {
        ui->enablePUACheckBox->setChecked(false);
        ui->PUAFrame->setEnabled(false);
    }

    m_keyword = "LoadPUAPacked";
    ui->loadPUAPackedRadioButon->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUAPWTool";
    ui->loadPUAPWToolRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUANetTool";
    ui->loadPUANetToolRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUAP2P";
    ui->loadPUAP2PRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUAIRC";
    ui->loadPUAIRCRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUARAT";
    ui->loadPUARATRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUANetToolSpy";
    ui->loadPUANetToolSpyRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUAServer";
    ui->loadPUAServerRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUAScript";
    ui->loadPUAScriptRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUAAndr";
    ui->loadPUAAndrRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUAJava";
    ui->loadPUAJavaRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUAOsx";
    ui->loadPUAOsxRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUATool";
    ui->loadPUAToolRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUAUnix";
    ui->loadPUAUnixRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    m_keyword = "LoadPUAWin";
    ui->loadPUAWinRadioButton->setChecked(setupFile->getSectionBoolValue(m_section,m_keyword));

    readSettingsInProgress = false;
}

void includeExcludeOptions::slot_enablePUACheckBoxClicked(){
    ui->PUAFrame->setEnabled(ui->enablePUACheckBox->isChecked());
    slot_writeSettings();
}

void includeExcludeOptions::slot_highlightSettings()
{
    ui->dontScanFileNameCheckBox->isChecked()?ui->frame01->setStyleSheet(css):ui->frame01->setStyleSheet("");
    ui->dontScanDirCheckBox->isChecked()?ui->frame02->setStyleSheet(css):ui->frame02->setStyleSheet("");
    ui->onlyScanFileNameCheckBox->isChecked()?ui->frame03->setStyleSheet(css):ui->frame03->setStyleSheet("");
    ui->onlyScanDirCheckBox->isChecked()?ui->frame04->setStyleSheet(css):ui->frame04->setStyleSheet("");

    ui->loadPUAPackedRadioButon->isChecked()?ui->loadPUAPackedRadioButon->setStyleSheet(css):ui->loadPUAPackedRadioButon->setStyleSheet("");
    ui->loadPUAPWToolRadioButton->isChecked()?ui->loadPUAPWToolRadioButton->setStyleSheet(css):ui->loadPUAPWToolRadioButton->setStyleSheet("");
    ui->loadPUANetToolRadioButton->isChecked()?ui->loadPUANetToolRadioButton->setStyleSheet(css):ui->loadPUANetToolRadioButton->setStyleSheet("");
    ui->loadPUAP2PRadioButton->isChecked()?ui->loadPUAP2PRadioButton->setStyleSheet(css):ui->loadPUAP2PRadioButton->setStyleSheet("");
    ui->loadPUAIRCRadioButton->isChecked()?ui->loadPUAIRCRadioButton->setStyleSheet(css):ui->loadPUAIRCRadioButton->setStyleSheet("");
    ui->loadPUARATRadioButton->isChecked()?ui->loadPUARATRadioButton->setStyleSheet(css):ui->loadPUARATRadioButton->setStyleSheet("");
    ui->loadPUANetToolSpyRadioButton->isChecked()?ui->loadPUANetToolSpyRadioButton->setStyleSheet(css):ui->loadPUANetToolSpyRadioButton->setStyleSheet("");
    ui->loadPUAServerRadioButton->isChecked()?ui->loadPUAServerRadioButton->setStyleSheet(css):ui->loadPUAServerRadioButton->setStyleSheet("");
    ui->loadPUAScriptRadioButton->isChecked()?ui->loadPUAScriptRadioButton->setStyleSheet(css):ui->loadPUAScriptRadioButton->setStyleSheet("");
    ui->loadPUAAndrRadioButton->isChecked()?ui->loadPUAAndrRadioButton->setStyleSheet(css):ui->loadPUAAndrRadioButton->setStyleSheet("");
    ui->loadPUAJavaRadioButton->isChecked()?ui->loadPUAJavaRadioButton->setStyleSheet(css):ui->loadPUAJavaRadioButton->setStyleSheet("");
    ui->loadPUAOsxRadioButton->isChecked()?ui->loadPUAOsxRadioButton->setStyleSheet(css):ui->loadPUAOsxRadioButton->setStyleSheet("");
    ui->loadPUAToolRadioButton->isChecked()?ui->loadPUAToolRadioButton->setStyleSheet(css):ui->loadPUAToolRadioButton->setStyleSheet("");
    ui->loadPUAUnixRadioButton->isChecked()?ui->loadPUAUnixRadioButton->setStyleSheet(css):ui->loadPUAUnixRadioButton->setStyleSheet("");
    ui->loadPUAWinRadioButton->isChecked()?ui->loadPUAWinRadioButton->setStyleSheet(css):ui->loadPUAWinRadioButton->setStyleSheet("");
}
