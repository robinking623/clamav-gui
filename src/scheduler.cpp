#include "scheduler.h"
#include "ui_scheduler.h"

scheduler::scheduler(QWidget *parent) : QWidget(parent), ui(new Ui::scheduler)
{
QStringList header;

    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    ui->setupUi(this);
    header << tr("ID") << tr("Interval") << tr("Profile") << tr("Last Scan") << tr("Next Scan") << tr("Remove") << tr("Scan Now") << tr("Log-File");
    ui->scanJobTableWidget->setHorizontalHeaderLabels(header);
    ui->scanJobTableWidget->setColumnWidth(6,120);
    ui->scanJobTableWidget->setColumnWidth(5,120);
    removeButtonGroup = new QButtonGroup(this);
    scanNowButtonGroup = new QButtonGroup(this);
    logButtonGroup = new QButtonGroup(this);
    connect(removeButtonGroup,SIGNAL(idClicked(int)),this,SLOT(slot_removeButtonClicked(int)));
    connect(scanNowButtonGroup,SIGNAL(idClicked(int)),this,SLOT(slot_scanButtonClicked(int)));
    connect(logButtonGroup,SIGNAL(idClicked(int)),this,SLOT(slot_logButtonClicked(int)));
    slot_updateProfiles();
    updateScheduleList();
    checkTimer = new QTimer(this);
    checkTimer->setSingleShot(false);
    connect(checkTimer,SIGNAL(timeout()),this,SLOT(slot_checkTimerTimeout()));
    checkTimer->start(15000);
}

scheduler::~scheduler()
{
    delete ui;
}

void scheduler::slot_addDailyScanJobButtonClicked(){
QDateTime m_currentDateTime = QDateTime::currentDateTime();
QString m_scanTime = ui->dailyTimeEdit->text();
    if (m_scanTime.length() < 8) m_scanTime = m_scanTime + ":00";
QDateTime m_nextScanObject = QDateTime(QDate::currentDate(),QTime::fromString(m_scanTime));
QString m_id = QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());
QString m_entry;

    if (m_nextScanObject < m_currentDateTime) m_nextScanObject = m_nextScanObject.addDays(1);
    m_entry = "daily|" + ui->profileComboBox->currentText() + "|" + "never" + "|" + QString::number(m_nextScanObject.toMSecsSinceEpoch());
    setupFile->setSectionValue("ScanJobs",m_id,m_entry);
    updateScheduleList();
}

void scheduler::slot_addWeeklyScanJobButtonClicked(){
QDateTime m_currentDateTime = QDateTime::currentDateTime();
QDateTime m_nextScanObject = m_currentDateTime;
QString m_id = QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());
int m_currentDayofweek = m_currentDateTime.date().dayOfWeek();
int m_dayofweek = ui->weeklyDayOfWeekComboBox->currentIndex() + 1;
QString m_entry;
QString m_scanTime = ui->weeklyTimeEdit->text();

    if (m_scanTime.length() < 8) m_scanTime = m_scanTime + ":00";

    if (m_currentDayofweek < m_dayofweek){
        int diff = m_dayofweek - m_currentDayofweek;
        m_nextScanObject = m_nextScanObject.addDays(diff);
    } else {
        if (m_currentDayofweek > m_dayofweek){
            int diff = 7 - (m_currentDayofweek - m_dayofweek);
            m_nextScanObject = m_nextScanObject.addDays(diff);
        } else {
            if (QTime::currentTime() > QTime::fromString(m_scanTime)){
                m_nextScanObject = m_nextScanObject = m_nextScanObject.addDays(7);
            }
        }
    }
    m_nextScanObject = QDateTime(m_nextScanObject.date(),QTime::fromString(m_scanTime));
    m_entry = "weekly|" + ui->profileComboBox->currentText() + "|" + "never" + "|" + QString::number(m_nextScanObject.toMSecsSinceEpoch());
    setupFile->setSectionValue("ScanJobs",m_id,m_entry);
    updateScheduleList();
}

void scheduler::slot_addMonthlyScanJobButtonClicked(){
QString m_entry;
QString m_id = QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());
QDateTime m_currentDateTime = QDateTime::currentDateTime();
QString m_scanTime = ui->monthlyTimeEdit->text();
    if (m_scanTime.length() < 8) m_scanTime = m_scanTime + ":00";
QDateTime m_nextScanObject = QDateTime(QDate(QDate::currentDate().year(),QDate::currentDate().month(),ui->monthlyDaySpinBox->value()),QTime::fromString(m_scanTime));

    if (m_nextScanObject < QDateTime::currentDateTime()){
        m_nextScanObject = m_nextScanObject.addMonths(1);
    }
    m_entry = "monthly|" + ui->profileComboBox->currentText() + "|" + "never" + "|" + QString::number(m_nextScanObject.toMSecsSinceEpoch());
    setupFile->setSectionValue("ScanJobs",m_id,m_entry);
    updateScheduleList();
}

void scheduler::slot_updateProfiles(){
QStringList m_profiles = setupFile->getKeywords("Profiles");
QStringList m_selectableProfiles;
setupFileHandler m_sf(this);

    foreach(QString profile,m_profiles){
        m_sf.setSetupFileName(QDir::homePath() + "/.clamav-gui/profiles/" + profile + ".ini");
        if (m_sf.getSectionValue(profile,"Directories") != "") m_selectableProfiles << profile;
    }

    ui->profileComboBox->clear();
    ui->profileComboBox->addItems(m_selectableProfiles);
    if (m_selectableProfiles.count() == 0){
        ui->dailyAddScanJobButton->setEnabled(false);
        ui->weeklyAddScanJobButton->setEnabled(false);
        ui->monthlyAddScanJobButton->setEnabled(false);
        ui->dailyProfileLabel->setText("--------------");
        ui->weeklyProfileLabel->setText("--------------");
        ui->monthlyProfileLable->setText("--------------");
    } else {
        ui->dailyAddScanJobButton->setEnabled(true);
        ui->weeklyAddScanJobButton->setEnabled(true);
        ui->monthlyAddScanJobButton->setEnabled(true);
        ui->dailyProfileLabel->setText(ui->profileComboBox->currentText());
        ui->weeklyProfileLabel->setText(ui->profileComboBox->currentText());
        ui->monthlyProfileLable->setText(ui->profileComboBox->currentText());
        slot_profileSelectionChanged();
    }
}


void scheduler::updateScheduleList(){
QStringList m_jobs = setupFile->getKeywords("ScanJobs");
QStringList m_jobData;
QDateTime m_tempDateTime;
int m_width[8] = {130,80,160,180,180,130,130,130};
int m_id = 0;
int m_rowCount;

    while (ui->scanJobTableWidget->rowCount() > 0){
        QPushButton * m_removeButton = (QPushButton*)ui->scanJobTableWidget->cellWidget(0,5);
        QPushButton * m_scanNowButton = (QPushButton*)ui->scanJobTableWidget->cellWidget(0,6);
        QPushButton * m_logButton = (QPushButton*)ui->scanJobTableWidget->cellWidget(0,7);
        removeButtonGroup->removeButton(m_removeButton);
        scanNowButtonGroup->removeButton(m_scanNowButton);
        logButtonGroup->removeButton(m_logButton);
        ui->scanJobTableWidget->removeRow(0);
    }

    foreach(QString job,m_jobs){
        m_jobData = setupFile->getSectionValue("ScanJobs",job).split("|");
        m_rowCount = ui->scanJobTableWidget->rowCount();
        ui->scanJobTableWidget->insertRow(m_rowCount);
        for (int i = 0; i < 8; i++){
            ui->scanJobTableWidget->setColumnWidth(i,m_width[i]);
        }
        ui->scanJobTableWidget->setItem(m_rowCount,0,new QTableWidgetItem(job));
        ui->scanJobTableWidget->setItem(m_rowCount,1,new QTableWidgetItem(m_jobData[0]));
        ui->scanJobTableWidget->setItem(m_rowCount,2,new QTableWidgetItem(m_jobData[1]));
        if (m_jobData[2] == "never") {
            ui->scanJobTableWidget->setItem(m_rowCount,3,new QTableWidgetItem("Never"));
        } else {
            m_tempDateTime.setMSecsSinceEpoch(m_jobData[2].toLongLong());
            ui->scanJobTableWidget->setItem(m_rowCount,3,new QTableWidgetItem(m_tempDateTime.toString("dd.MM.yyyy 'at' hh:mm")));
        }
        m_tempDateTime.setMSecsSinceEpoch(m_jobData[3].toLongLong());
        ui->scanJobTableWidget->setItem(m_rowCount,4,new QTableWidgetItem(m_tempDateTime.toString("dd.MM.yyyy 'at' hh:mm")));
        QPushButton * m_removeButton = new QPushButton(QIcon(":/icons/icons/trash-can.png"),tr("remove task"),this);
        removeButtonGroup->addButton(m_removeButton,m_id);
        QPushButton * m_scanNowButton = new QPushButton(QIcon(":/icons/icons/start.png"),tr("scan now"),this);
        scanNowButtonGroup->addButton(m_scanNowButton,m_id);
        QPushButton * m_logButton = new QPushButton(QIcon(":/icons/icons/information.png"),tr("Log-File"),this);
        logButtonGroup->addButton(m_logButton,m_id);
        ui->scanJobTableWidget->setCellWidget(m_rowCount,5,m_removeButton);
        ui->scanJobTableWidget->setCellWidget(m_rowCount,6,m_scanNowButton);
        ui->scanJobTableWidget->setCellWidget(m_rowCount,7,m_logButton);
        for (int i = 0; i < 5; i++){
            ui->scanJobTableWidget->item(m_rowCount,i)->setTextAlignment(Qt::AlignCenter);
        }
        m_id++;
    }
}

void scheduler::slot_scanButtonClicked(int id){
int m_rc = QMessageBox::information(this,tr("Start Scan-Job"),tr("Do you realy want to start this Scan-Job?"),QMessageBox::Yes,QMessageBox::No);
QString m_profileName;
qint64 m_today = QDateTime::currentDateTime().toMSecsSinceEpoch();
QStringList m_values;
QString m_temp;
QString m_scanID;

    if (m_rc == QMessageBox::Yes){
        QTableWidgetItem *m_item = (QTableWidgetItem*)ui->scanJobTableWidget->item(id,2);
        QTableWidgetItem *m_item2 = (QTableWidgetItem*)ui->scanJobTableWidget->item(id,0);
        m_profileName = m_item->text();
        m_scanID = m_item2->text();
        m_values = setupFile->getSectionValue("ScanJobs",m_scanID).split("|");
        m_values[2] = QString::number(m_today);
        m_temp = m_values[0] + "|" + m_values[1] + "|" + m_values[2] + "|" + m_values[3];
        setupFile->setSectionValue("ScanJobs",m_scanID,m_temp);
        updateScheduleList();
        startScanJob(m_profileName);
    }
}

void scheduler::slot_removeButtonClicked(int id){
int m_rc = QMessageBox::information(this,tr("Remove Entry"),tr("Do you realy want to remove this entry?"),QMessageBox::Yes,QMessageBox::No);
QString m_jobID;

    if (m_rc == QMessageBox::Yes){
        QTableWidgetItem *m_item = (QTableWidgetItem*)ui->scanJobTableWidget->item(id,0);
        m_jobID = m_item->text();
        setupFile->removeKeyword("ScanJobs",m_jobID);
        updateScheduleList();
    }
}

void scheduler::slot_logButtonClicked(int id){
QString m_profileName;
QString m_logFile;

    QTableWidgetItem *m_item = (QTableWidgetItem*)ui->scanJobTableWidget->item(id,2);
    m_profileName = m_item->text();
    setupFileHandler * m_tempSF = new setupFileHandler(QDir::homePath() + "/.clamav-gui/profiles/" + m_profileName + ".ini",this);
    m_logFile = m_tempSF->getSectionValue("Directories","ScanReportToFile");
    if (m_logFile.left(m_logFile.indexOf("|")) == "checked"){
        m_logFile = m_logFile.mid(m_logFile.indexOf("|") + 1);
        logViewObject * m_logViewer = new logViewObject(this,m_logFile);
        connect(m_logViewer,SIGNAL(logChanged()),this,SLOT(slot_logChanged()));
        m_logViewer->setModal(true);
        m_logViewer->showMaximized();
    } else {
        QMessageBox::information(this,tr("INFO"),tr("No active log-file for this profile specified!"));
    }
}


void scheduler::startScanJob(QString profileName){
setupFileHandler * m_setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/profiles/" + profileName + ".ini",this);
QStringList m_parameters;
QStringList m_selectedOptions = m_setupFile->getKeywords("SelectedOptions");
QStringList m_directoryOptions = m_setupFile->getKeywords("Directories");
QStringList m_scanLimitations = m_setupFile->getKeywords("ScanLimitations");
QString m_option;
QString m_checked;
QString m_value;

    if (m_setupFile->getSectionBoolValue(profileName,"Recursion") == true){
        m_parameters << "-r";
    }
    for (int i = 0; i < m_selectedOptions.count(); i++){
        m_parameters << m_selectedOptions.at(i).left(m_selectedOptions.indexOf("|")).replace("<equal>","=");
    }

    // Directory Options
    for (int i = 0; i < m_directoryOptions.count(); i++){
        m_option = m_directoryOptions.at(i);
        m_value = m_setupFile->getSectionValue("Directories",m_option);
        m_checked = m_value.left(m_value.indexOf("|"));
        m_value = m_value.mid(m_value.indexOf("|") + 1);
        if (m_checked == "m_checked"){
            if (m_option == "LoadSupportedDBFiles") m_parameters << "--database="+ m_value;
            if (m_option == "ScanReportToFile") {
                m_parameters << "--log=" + m_value;
                QFile m_file(m_value);
                if (m_file.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text)){
                    QTextStream m_stream(&m_file);
                    m_stream << "\n<Scanning startet> " << QDateTime::currentDateTime().toString("yyyy/M/d - hh:mm");
                    m_file.close();
                }
            }
            if (m_option == "TmpFile") m_parameters << "--tempdir=" + m_value;
            if (m_option == "MoveInfectedFiles") m_parameters << "--move=" + m_value;
            if (m_option == "CopyInfectedFiles") m_parameters << "--copy=" + m_value;
            if (m_option == "SCanFileFromFiles") m_parameters << "--m_file-list=" + m_value;
            if (m_option == "FollowDirectorySymLinks") m_parameters << "--follow-dir-symlinks=" + m_value;
            if (m_option == "FollowFileSymLinks") m_parameters << "--follow-m_file-symlinks=" + m_value;
        }
    }

    // Scan Limitations
    for (int i = 0; i < m_scanLimitations.count(); i++){
        m_option = m_scanLimitations.at(i);
        m_value = m_setupFile->getSectionValue("ScanLimitations",m_option);
        m_checked = m_value.left(m_value.indexOf("|"));
        m_value = m_value.mid(m_value.indexOf("|") + 1);
        if (m_checked == "m_checked"){
            if (m_option == "Files larger than this will be skipped and assumed clean") m_parameters << "--max-filesize=" + m_value;
            if (m_option == "The maximum amount of data to scan for each container m_file") m_parameters << "--max-scansize=" + m_value;
            if (m_option == "The maximum number of files to scan for each container m_file") m_parameters << "--max-files=" + m_value;
            if (m_option == "Maximum archive recursion level for container m_file") m_parameters << "--max-recursion=" + m_value;
            if (m_option == "Maximum directory recursion level") m_parameters << "--max-dir-recursion=" + m_value;
            if (m_option == "Maximum size m_file to check for embedded PE") m_parameters << "--max-embeddedpe=" + m_value;
            if (m_option == "Maximum size of HTML m_file to normalize") m_parameters << "--max-htmlnormalize=" + m_value;
            if (m_option == "Maximum size of normalized HTML m_file to scan") m_parameters << "--max-htmlnotags=" + m_value;
            if (m_option == "Maximum size of script m_file to normalize") m_parameters << "--max-scriptnormalize=" + m_value;
            if (m_option == "Maximum size zip to type reanalyze") m_parameters << "--max-ziptypercg=" + m_value;
            if (m_option == "Maximum number of partitions in disk image to be scanned") m_parameters << "--max-partitions=" + m_value;
            if (m_option == "Maximum number of icons in PE m_file to be scanned") m_parameters << "--max-iconspe=" + m_value;
            if (m_option == "Number of seconds to wait for waiting a response back from the stats server") m_parameters << "--stats-timeout=" + m_value;
        }
    }

    // REGEXP and Include Exclude Options
    m_value = m_setupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanFileNamesMatchingRegExp");
    m_checked = m_value.left(m_value.indexOf("|"));
    m_value = m_value.mid(m_value.indexOf("|") + 1);
    if (m_checked == "m_checked") m_parameters << "--exclude=" + m_value;

    m_value = m_setupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanDirectoriesMatchingRegExp");
    m_checked = m_value.left(m_value.indexOf("|"));
    m_value = m_value.mid(m_value.indexOf("|") + 1);
    if (m_checked == "m_checked") m_parameters << "--exclude-dir=" + m_value;

    m_value = m_setupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanFileNamesMatchingRegExp");
    m_checked = m_value.left(m_value.indexOf("|"));
    m_value = m_value.mid(m_value.indexOf("|") + 1);
    if (m_checked == "m_checked") m_parameters << "--include=" + m_value;

    m_value = m_setupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanDirectoriesMatchingRegExp");
    m_checked = m_value.left(m_value.indexOf("|"));
    m_value = m_value.mid(m_value.indexOf("|") + 1);
    if (m_checked == "m_checked") m_parameters << "--include-dir=" + m_value;

    if (m_setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","EnablePUAOptions") == true){
        m_setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","SkipPUAPacked") == true?m_parameters << "--exclude-pua=Packed":m_parameters << "--include-pua=Packed";
        m_setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","SkipPUAPWTool") == true?m_parameters << "--exclude-pua=PWTool":m_parameters << "--include-pua=PWTool";
        m_setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","SkipPUANetTool") == true?m_parameters << "--exclude-pua=NetTool":m_parameters << "--include-pua=NetTool";
        m_setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","SkipPUAP2P") == true?m_parameters << "--exclude-pua=P2P":m_parameters << "--include-pua=P2P";
        m_setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","SkipPUAIRC") == true?m_parameters << "--exclude-pua=IRC":m_parameters << "--include-pua=IRC";
        m_setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","SkipPUARAT") == true?m_parameters << "--exclude-pua=RAT":m_parameters << "--include-pua=RAT";
        m_setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","SkipPUANetToolSpy") == true?m_parameters << "--exclude-pua=NetToolSpy":m_parameters << "--include-pua=NetToolSpy";
        m_setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","SkipPUAServer") == true?m_parameters << "--exclude-pua=Server":m_parameters << "--include-pua=Server";
        m_setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","SkipPUAScript") == true?m_parameters << "--exclude-pua=Script":m_parameters << "--include-pua=Script";
    }

    QStringList m_directories = m_setupFile->getSectionValue(profileName,"Directories").split("\n");

    for (int i = 0; i < m_directories.count(); i++){
        if (m_directories.at(i) != "") m_parameters << m_directories.at(i);
    }

    emit triggerScanJob(profileName,m_parameters);
}

void scheduler::slot_checkTimerTimeout(){
QStringList m_scanJobs = setupFile->getKeywords("ScanJobs");
QStringList m_values;
QString m_line;
qint64 m_today = QDateTime::currentDateTime().toMSecsSinceEpoch();
qint64 m_scanDate;
QDateTime m_scanDateTime;

    foreach(QString scanJob,m_scanJobs){
        m_values = setupFile->getSectionValue("ScanJobs",scanJob).split("|");
        m_scanDate = m_values[3].toLongLong();
        if (m_scanDate < m_today){
            if (m_values[0] == "daily"){
                m_scanDateTime = QDateTime::fromMSecsSinceEpoch(m_scanDate);
                while (m_scanDateTime.toMSecsSinceEpoch() < m_today){
                    m_scanDateTime = m_scanDateTime.addDays(1);
                }
            }
            if (m_values[0] == "weekly"){
                m_scanDateTime = QDateTime::fromMSecsSinceEpoch(m_scanDate);
                while (m_scanDateTime.toMSecsSinceEpoch() < m_today){
                    m_scanDateTime = m_scanDateTime.addDays(7);
                }
            }
            if (m_values[0] == "monthly"){
                m_scanDateTime = QDateTime::fromMSecsSinceEpoch(m_scanDate);
                while (m_scanDateTime.toMSecsSinceEpoch() < m_today){
                    m_scanDateTime = m_scanDateTime.addMonths(1);
                }
            }
            m_line = m_values[0] + "|" + m_values[1] + "|" + QString::number(m_today) + "|" + QString::number(m_scanDateTime.toMSecsSinceEpoch());
            setupFile->setSectionValue("ScanJobs",scanJob,m_line);
            updateScheduleList();
            startScanJob(m_values[1]);
        }
    }
}

void scheduler::slot_profileSelectionChanged(){
QString m_profileName = ui->profileComboBox->currentText();
setupFileHandler * m_tempSetupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/profiles/" + m_profileName + ".ini",this);
QStringList m_targets;
QString m_targetLabel;
QStringList m_options;
QString m_optionLabel;
QString m_logFile = m_tempSetupFile->getSectionValue("Directories","ScanReportToFile");

    ui->dailyProfileLabel->setText(m_profileName);
    ui->weeklyProfileLabel->setText(m_profileName);
    ui->monthlyProfileLable->setText(m_profileName);

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

    ui->targetInfoLabel->setText(m_targetLabel);

    if (m_options.count() > 0) m_optionLabel = m_optionLabel + m_options[0];
    for (int i = 1; i < m_options.count(); i++){
        m_optionLabel = m_optionLabel + "\n" + m_options[i];
    }

    if (m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanFileNamesMatchingRegExp").indexOf("not checked") == -1) m_optionLabel = m_optionLabel + "\n" + "--exclude=" + m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanFileNamesMatchingRegExp").mid(m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanFileNamesMatchingRegExp").indexOf("|") + 1);
    if (m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").indexOf("not checked") == -1) m_optionLabel = m_optionLabel + "\n" + "--exclude-dir=" + m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").mid(m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").indexOf("|") + 1);
    if (m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanFileNamesMatchingRegExp").indexOf("not checked") == -1) m_optionLabel = m_optionLabel + "\n" + "--include=" + m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanFileNamesMatchingRegExp").mid(m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanFileNamesMatchingRegExp").indexOf("|") + 1);
    if (m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanDiretoriesMatchingRegExp").indexOf("not checked") == -1) m_optionLabel = m_optionLabel + "\n" + "--include-dir=" + m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanDiretoriesMatchingRegExp").mid(m_tempSetupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanDiretoriesMatchingRegExp").indexOf("|") + 1);

    if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","EnablePUAOptions") == true) {
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAPacked") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=Packed";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAPWTool") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=PWTool";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUANetTool") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=NetTool";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAP2P") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=P2P";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAIRC") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=IRC";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUARAT") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=RAT";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUANetToolSpy") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=NetToolSpy";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAServer") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=Server";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAScript") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=Script";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAAndr") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=Andr";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAJava") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=Java";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAOsx") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=Osx";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUATool") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=Tool";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAUnix") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=Unix";
        if (m_tempSetupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAWin") == true) m_optionLabel = m_optionLabel + "\n" + "--include-pua=Win";
    }

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
    ui->optionsInfoLabel->setText(m_optionLabel);

    if (m_logFile.left(m_logFile.indexOf("|")) == "checked"){
        m_logFile = m_logFile.mid(m_logFile.indexOf("|") + 1);
    } else {
        m_logFile = "";
    }
    ui->logFileLabel->setText("Log-File : " + m_logFile);
}

void scheduler::slot_logChanged(){
    emit logChanged();
}

void scheduler::slot_disableScheduler()
{
    ui->scanJobTableWidget->setEnabled(false);
}
