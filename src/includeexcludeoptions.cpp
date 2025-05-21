#include "includeexcludeoptions.h"
#include "ui_includeexcludeoptions.h"
#define css "background-color:#404040;color:white"

includeExcludeOptions::includeExcludeOptions(QWidget *parent) : QWidget(parent), ui(new Ui::includeExcludeOptions)
{
    ui->setupUi(this);
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini");
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
QString section = "REGEXP_and_IncludeExclude";
QString checked;
QString keyword;
QString value;
    if (readSettingsInProgress == false) {
        keyword = "DontScanFileNamesMatchingRegExp";
        value = ui->dontScanFileNameLineEdit->text();
        ui->dontScanFileNameCheckBox->isChecked()?checked="checked":checked="not checked";
        setupFile->setSectionValue(section,keyword,checked + "|" + value);

        keyword = "DontScanDiretoriesMatchingRegExp";
        value = ui->dontScanDirLineEdit->text();
        ui->dontScanDirCheckBox->isChecked()?checked="checked":checked="not checked";
        setupFile->setSectionValue(section,keyword,checked + "|" + value);

        keyword = "OnlyScanFileNamesMatchingRegExp";
        value = ui->onlyScanFileNameLineEdit->text();
        ui->onlyScanFileNameCheckBox->isChecked()?checked="checked":checked="not checked";
        setupFile->setSectionValue(section,keyword,checked + "|" + value);

        keyword = "OnlyScanDiretoriesMatchingRegExp";
        value = ui->onlyScanDirLineEdit->text();
        ui->onlyScanDirCheckBox->isChecked()?checked="checked":checked="not checked";
        setupFile->setSectionValue(section,keyword,checked + "|" + value);

        keyword = "EnablePUAOptions";
        setupFile->setSectionValue(section,keyword,ui->enablePUACheckBox->isChecked());

        keyword = "LoadPUAPacked";
        setupFile->setSectionValue(section,keyword,ui->loadPUAPackedRadioButon->isChecked());

        keyword = "LoadPUAPWTool";
        setupFile->setSectionValue(section,keyword,ui->loadPUAPWToolRadioButton->isChecked());

        keyword = "LoadPUANetTool";
        setupFile->setSectionValue(section,keyword,ui->loadPUANetToolRadioButton->isChecked());

        keyword = "LoadPUAP2P";
        setupFile->setSectionValue(section,keyword,ui->loadPUAP2PRadioButton->isChecked());

        keyword = "LoadPUAIRC";
        setupFile->setSectionValue(section,keyword,ui->loadPUAIRCRadioButton->isChecked());

        keyword = "LoadPUARAT";
        setupFile->setSectionValue(section,keyword,ui->loadPUARATRadioButton->isChecked());

        keyword = "LoadPUANetToolSpy";
        setupFile->setSectionValue(section,keyword,ui->loadPUANetToolSpyRadioButton->isChecked());

        keyword = "LoadPUAServer";
        setupFile->setSectionValue(section,keyword,ui->loadPUAServerRadioButton->isChecked());

        keyword = "LoadPUAScript";
        setupFile->setSectionValue(section,keyword,ui->loadPUAScriptRadioButton->isChecked());

        keyword = "LoadPUAAndr";
        setupFile->setSectionValue(section,keyword,ui->loadPUAAndrRadioButton->isChecked());

        keyword = "LoadPUAJava";
        setupFile->setSectionValue(section,keyword,ui->loadPUAJavaRadioButton->isChecked());

        keyword = "LoadPUAOsx";
        setupFile->setSectionValue(section,keyword,ui->loadPUAOsxRadioButton->isChecked());

        keyword = "LoadPUATool";
        setupFile->setSectionValue(section,keyword,ui->loadPUAToolRadioButton->isChecked());

        keyword = "LoadPUAUnix";
        setupFile->setSectionValue(section,keyword,ui->loadPUAUnixRadioButton->isChecked());

        keyword = "LoadPUAWin";
        setupFile->setSectionValue(section,keyword,ui->loadPUAWinRadioButton->isChecked());

        emit updateClamdConf();
    }
}

void includeExcludeOptions::readSettings(){
QString section = "REGEXP_and_IncludeExclude";
QString checked;
QString keyword;
QString value;

    keyword = "DontScanFileNamesMatchingRegExp";
    value = setupFile->getSectionValue(section,keyword);
    checked = value.left(value.indexOf("|"));
    value = value.mid(value.indexOf("|") + 1);
    checked=="checked"?ui->dontScanFileNameCheckBox->setChecked(true):ui->dontScanFileNameCheckBox->setChecked(false);
    ui->dontScanFileNameLineEdit->setText(value);

    keyword = "DontScanDiretoriesMatchingRegExp";
    value = setupFile->getSectionValue(section,keyword);
    checked = value.left(value.indexOf("|"));
    value = value.mid(value.indexOf("|") + 1);
    checked=="checked"?ui->dontScanDirCheckBox->setChecked(true):ui->dontScanDirCheckBox->setChecked(false);
    ui->dontScanDirLineEdit->setText(value);

    keyword = "OnlyScanFileNamesMatchingRegExp";
    value = setupFile->getSectionValue(section,keyword);
    checked = value.left(value.indexOf("|"));
    value = value.mid(value.indexOf("|") + 1);
    checked=="checked"?ui->onlyScanFileNameCheckBox->setChecked(true):ui->onlyScanFileNameCheckBox->setChecked(false);
    ui->onlyScanFileNameLineEdit->setText(value);

    keyword = "OnlyScanDiretoriesMatchingRegExp";
    value = setupFile->getSectionValue(section,keyword);
    checked = value.left(value.indexOf("|"));
    value = value.mid(value.indexOf("|") + 1);
    checked=="checked"?ui->onlyScanDirCheckBox->setChecked(true):ui->onlyScanDirCheckBox->setChecked(false);
    ui->onlyScanDirLineEdit->setText(value);

    keyword = "EnablePUAOptions";
    if (setupFile->getSectionBoolValue(section,keyword) == true){
        ui->enablePUACheckBox->setChecked(true);
        ui->PUAFrame->setEnabled(true);
    } else {
        ui->enablePUACheckBox->setChecked(false);
        ui->PUAFrame->setEnabled(false);
    }

    keyword = "LoadPUAPacked";
    ui->loadPUAPackedRadioButon->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUAPWTool";
    ui->loadPUAPWToolRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUANetTool";
    ui->loadPUANetToolRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUAP2P";
    ui->loadPUAP2PRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUAIRC";
    ui->loadPUAIRCRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUARAT";
    ui->loadPUARATRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUANetToolSpy";
    ui->loadPUANetToolSpyRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUAServer";
    ui->loadPUAServerRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUAScript";
    ui->loadPUAScriptRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUAAndr";
    ui->loadPUAAndrRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUAJava";
    ui->loadPUAJavaRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUAOsx";
    ui->loadPUAOsxRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUATool";
    ui->loadPUAToolRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUAUnix";
    ui->loadPUAUnixRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

    keyword = "LoadPUAWin";
    ui->loadPUAWinRadioButton->setChecked(setupFile->getSectionBoolValue(section,keyword));

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
