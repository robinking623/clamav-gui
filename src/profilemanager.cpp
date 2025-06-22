#include "profilemanager.h"
#include "ui_profilemanager.h"

ProfileManager::ProfileManager(QWidget *parent) : QWidget(parent), ui(new Ui::ProfileManager){
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    ui->setupUi(this);
    getProfileList();
    slot_readProfileSettings();
    checkMonochromeSettings();

}

ProfileManager::~ProfileManager(){
    delete ui;
}

void ProfileManager::getProfileList(){
QStringList m_profiles = setupFile->getKeywords("Profiles");
QStringList m_selectableProfiles;
setupFileHandler m_sf(this);

    foreach(QString profile,m_profiles){
        m_sf.setSetupFileName(QDir::homePath() + "/.clamav-gui/profiles/" + profile + ".ini");
        if (m_sf.getSectionValue(profile,"Directories") != "") m_selectableProfiles << profile;
    }

    ui->profileComboBox->clear();
    ui->profileComboBox->addItems(m_profiles);
}

void ProfileManager::checkMonochromeSettings()
{
    if (setupFile->getSectionBoolValue("Setup","DisableLogHighlighter") == true) {
        ui->targetsLabel->setStyleSheet("background-color:#404040;color:white;padding:3px;border-radius:3px");
        ui->filenameLabel->setStyleSheet("background-color:#404040;color:white;padding:3px;border-radius:3px");
        ui->optionsLabel->setStyleSheet("background-color:#404040;color:white;padding:3px;border-radius:3px");
    } else {
        ui->targetsLabel->setStyleSheet("background-color:#c0c0c0;color:black;padding:3px;border-radius:3px");
        ui->filenameLabel->setStyleSheet("background-color:#c0c0c0;color:black;padding:3px;border-radius:3px");
        ui->optionsLabel->setStyleSheet("background-color:#c0c0c0;color:black;padding:3px;border-radius:3px");
    }
}

void ProfileManager::slot_readProfileSettings(){
QString m_profileName = ui->profileComboBox->currentText();
if (m_profileName == "")
    return;

setupFileHandler * m_tempSetupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/profiles/" + m_profileName + ".ini",this);
QStringList m_targets;
QString m_targetLabel;
QStringList m_options;
QString m_optionLabel;
QString m_logFile = m_tempSetupFile->getSectionValue("Directories","ScanReportToFile");

    m_targets = m_tempSetupFile->getSectionValue(m_profileName,"Directories").split('\n');
    m_options = m_tempSetupFile->getKeywords("SelectedOptions");

    if ((m_targets[0] != "") &(m_targets.count() > 0)) m_targetLabel = m_targetLabel + m_targets[0];
    for (int i = 1; i < m_targets.count(); i++){
        if ((m_targets[i] != "") & (m_targetLabel != "")){
            m_targetLabel = m_targetLabel + "\n" + m_targets[i];
        } else {
            if (m_targets[i] != "") m_targetLabel = m_targets[i];
        }
    }

    ui->targetsListLabel->setText(m_targetLabel);

    if (m_options.count() > 0) m_optionLabel = m_optionLabel + m_options[0];
    for (int i = 1; i < m_options.count(); i++){
        m_optionLabel = m_optionLabel + "\n" + m_options[i];
    }

    if (m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanFileNamesMatchingRegExp").indexOf("not checked") == -1) m_optionLabel = m_optionLabel + "\n" + "--exclude=" + m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanFileNamesMatchingRegExp").mid(m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanFileNamesMatchingRegExp").indexOf("|") + 1);
    if (m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").indexOf("not checked") == -1) m_optionLabel = m_optionLabel + "\n" + "--exclude-dir=" + m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").mid(m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").indexOf("|") + 1);
    if (m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanFileNamesMatchingRegExp").indexOf("not checked") == -1) m_optionLabel = m_optionLabel + "\n" + "--include=" + m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanFileNamesMatchingRegExp").mid(m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanFileNamesMatchingRegExp").indexOf("|") + 1);
    if (m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanDiretoriesMatchingRegExp").indexOf("not checked") == -1) m_optionLabel = m_optionLabel + "\n" + "--include-dir=" + m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanDiretoriesMatchingRegExp").mid(m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanDiretoriesMatchingRegExp").indexOf("|") + 1);

    if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","EnablePUAOptions") == true) {
        QStringList m_keywords;
        QStringList m_switches;
        m_keywords << "LoadPUAPacked" << "LoadPUAPWTool" << "LoadPUANetTool" << "LoadPUAP2P" << "LoadPUAIRC" << "LoadPUARAT" << "LoadPUANetToolSpy";
        m_keywords << "LoadPUAServer" << "LoadPUAScript" << "LoadPUAAndr" << "LoadPUAJava" << "LoadPUAOsx" << "LoadPUATool" << "LoadPUAUnix" << "LoadPUAWin";
        m_switches << "--include-pua=Packed" << "--include-pua=PWTool" << "--include-pua=NetTool" << "--include-pua=P2P" << "--include-pua=IRC" << "--include-pua=RAT";
        m_switches << "--include-pua=NetToolSpy" << "--include-pua=Server" << "--include-pua=Script" << "--include-pua=Andr" << "--include-pua=Java";
        m_switches << "--include-pua=Osx" << "--include-pua=Tool" << "--include-pua=Unix" << "--include-pua=Win";
        for (int i = 0; i < m_keywords.length(); i++) {
            if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude",m_keywords.at(i)) == true) m_optionLabel = m_optionLabel + "\n" + m_switches.at(i);
        }
    }
    // Scan Limitations
    QString m_value = "";
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Files larger than this will be skipped and assumed clean");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-filesize=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","The maximum amount of data to scan for each container file");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-scansize=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","The maximum number of files to scan for each container file");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-files=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Maximum archive recursion level for container file");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-recursion=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Maximum directory recursion level");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-dir-recursion=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Maximum size file to check for embedded PE");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-embeddedpe=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Maximum size of HTML file to normalize");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-htmlnormalized=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Maximum size of normalized HTML file to scan");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-htmlnotags=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Maximum size of script file to normalize");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-scriptnormalize=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Maximum size zip to type reanalyze");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-ziptypercg=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Maximum number of partitions in disk image to be scanned");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-partitions=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Maximum number of icons in PE file to be scanned");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-iconspe=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Bytecode timeout in milliseconds");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--bytecode-timeout=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Collect and print execution statistics");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--statistics " + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Structured SSN Format");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--structured-ssn-format=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Structured SSN Count");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--structured-ssn-count=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Structured CC Count");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--structured-cc-count=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Structured CC Mode");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--structured-cc-mode=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Max Scan-Time");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-scantime=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Max recursion to HWP3 parsing function");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--max-rechwp3=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Max calls to PCRE match function");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--pcre-match-limit=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Max recursion calls to the PCRE match function");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--pcre-recmatch-limit=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Max PCRE file size");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + "--pcre-max-filesize=" + m_value.mid(m_value.indexOf("|") + 1);
    m_value = m_tempSetupFile->getSectionValue("ScanLimitations","Database outdated if older than x days");
    if ((m_value != "") && (m_value.indexOf("not checked") == -1)) m_optionLabel = m_optionLabel + "\n" + " --fail-if-cvd-older-than=" + m_value.mid(m_value.indexOf("|") + 1);

    if (m_tempSetupFile->getSectionBoolValue(m_profileName,"Recursion") == true){
        if (m_optionLabel != ""){
            m_optionLabel = m_optionLabel + "\n" + "-r";
        } else {
            m_optionLabel = "-r";
        }
    }

    m_optionLabel = m_optionLabel.replace("<equal>","=");
    ui->optionsListLabel->setText(m_optionLabel);

    if (m_logFile.left(m_logFile.indexOf("|")) == "checked"){
        m_logFile = m_logFile.mid(m_logFile.indexOf("|") + 1);
    } else {
        m_logFile = "";
    }

    ui->filenameTextLabel->setText(m_logFile);
}

void ProfileManager::slot_addProfileButtonClicked(){

    profileWizard = new ProfileWizardDialog(this);
    connect(profileWizard,SIGNAL(signal_profileSaved()),this,SLOT(slot_profileSaved()));
    profileWizard->setModal(true);
    profileWizard->show();
}

void ProfileManager::slot_editProfileButtonClicked(){
QString m_profileName = ui->profileComboBox->currentText();

    if (m_profileName != ""){
        profileWizard = new ProfileWizardDialog(this,m_profileName);
        connect(profileWizard,SIGNAL(signal_profileSaved()),this,SLOT(slot_profileSaved()));
        profileWizard->setModal(true);
        profileWizard->show();
        slot_readProfileSettings();
    }
}

void ProfileManager::slot_eraseProfileButtonClicked(){
QString m_profileName = ui->profileComboBox->currentText();
QStringList m_scanJobs = setupFile->getKeywords("ScanJobs");
QString m_line;
QString m_logfileName;
bool m_found = false;

    if (m_profileName != ""){
        foreach(QString scanJob,m_scanJobs){
            m_line = setupFile->getSectionValue("ScanJobs",scanJob);
            if (m_line.indexOf("|"+m_profileName+"|") != -1) m_found = true;
        }
        if (m_found == false){
            int m_rc = QMessageBox::question(this,tr("WARNING"),tr("Do you realy want to remove this (") + m_profileName + tr(") profile"),QMessageBox::Yes,QMessageBox::No);
            QFile m_tempFile(QDir::homePath() + "/.clamav-gui/profiles/" + ui->profileComboBox->currentText() + ".ini");
                if (m_rc == QMessageBox::Yes){
                    setupFileHandler * m_sf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/profiles/" + ui->profileComboBox->currentText() + ".ini",this);
                    m_logfileName = m_sf->getSectionValue("Directories","ScanReportToFile").mid(m_sf->getSectionValue("Directories","ScanReportToFile").indexOf("|") + 1);
                    if (m_logfileName != "" ){
                        if (QMessageBox::question(this,tr("Info"),tr("There is a log-file associated with this profile. Shall I remove the log-file as well?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes){
                            QFile m_fileRemover(m_logfileName);
                            m_fileRemover.remove();
                        }
                    }
                    setupFile->removeKeyword("Profiles",ui->profileComboBox->currentText());
                    m_tempFile.remove();
                    QMessageBox::information(this,tr("Info"),tr("Profile \"") + ui->profileComboBox->currentText() + tr("\" removed"));
                    getProfileList();
                    slot_readProfileSettings();
                    emit triggerProfilesChanged();
                }
        } else {
            QMessageBox::information(this,tr("ERROR"),tr("Profile can not be removed because it is in use by the scheduler!"));
        }
    }
}

void ProfileManager::slot_profileSaved(){
    emit triggerProfilesChanged();
    getProfileList();
}

void ProfileManager::monochromeModeChanged(bool state)
{
Q_UNUSED(state)
    checkMonochromeSettings();
}
