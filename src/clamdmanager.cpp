#include "clamdmanager.h"
#include "ui_clamdmanager.h"

clamdManager::clamdManager(QWidget *parent) : QWidget(parent), ui(new Ui::clamdManager){
    ui->setupUi(this);
    startup = true;
    waitForFreshclam = true;
    clamdStartupCounter = 0;

    startDelayTimer = new QTimer(this);
    startDelayTimer->setSingleShot(true);
    connect(startDelayTimer,SIGNAL(timeout()),this,SLOT(slot_startDelayTimerExpired()));

    processWatcher = new QTimer(this);
    connect(processWatcher,SIGNAL(timeout()),this,SLOT(slot_processWatcherExpired()));
    processWatcher->start(30000);

    initClamdSettings();
}

clamdManager::~clamdManager(){
    delete ui;
}

QString clamdManager::trimLocationOutput(QString value){
    QString rc = value;
    int start = rc.indexOf(" ") + 1;
    int ende = rc.indexOf(" ",start);

    ende = ende - start;
    rc = value.mid(start,ende);
    rc = rc.replace("\n","");
    return rc;
}

void clamdManager::initClamdSettings(){
    initprocessrunning = true;
    QStringList keywords;

    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini");
    sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");

    clamdConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/clamd.conf");

    QStringList parameters;
    QStringList monitorings = setupFile->getKeywords("Clamonacc");
    ui->monitoringComboBox->addItems(monitorings);
    logHighlighter = new highlighter(ui->clamdLogPlainTextEdit->document());

    clamdPidWatcher = new QFileSystemWatcher(this);
    connect(clamdPidWatcher,SIGNAL(fileChanged(QString)),this,SLOT(slot_pidWatcherTriggered()));

    clamdLocationProcess = new QProcess(this);
    connect(clamdLocationProcess,SIGNAL(finished(int)),this,SLOT(slot_clamdLocationProcessFinished()));

    clamonaccLocationProcess = new QProcess(this);
    connect(clamonaccLocationProcess,SIGNAL(finished(int)),this,SLOT(slot_clamonaccLocationProcessFinished()));

    startClamdProcess = new QProcess(this);
    connect(startClamdProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(slot_startClamdProcessFinished(int,QProcess::ExitStatus)));

    killProcess = new QProcess(this);
    connect(killProcess,SIGNAL(finished(int)),this,SLOT(slot_killClamdProcessFinished()));

    findclamonaccProcess = new QProcess(this);
    connect(findclamonaccProcess,SIGNAL(finished(int)),this,SLOT(slot_findclamonaccProcessFinished(int)));

    restartClamonaccProcess = new QProcess(this);
    connect(restartClamonaccProcess,SIGNAL(finished(int)),this,SLOT(slot_restartClamonaccProcessFinished()));

    if (checkClamdRunning() == true) {
        ui->startStopClamdPushButton->setStyleSheet("background-color:green");
        ui->startStopClamdPushButton->setText(tr("  Clamd running - Stop clamd"));
        ui->startStopClamdPushButton->setIcon(QIcon(":/icons/icons/stopclamd.png"));
        clamdPidWatcher->addPath("/tmp/clamd.pid");
    } else {
        ui->startStopClamdPushButton->setStyleSheet("background-color:red");
    }

    clamdRestartInProgress = false;

    keywords << "LogFileMaxSize" << "LogTimes" << "LotRotate" << "OnAccessMaxFileSize" << "OnAccessMaxThreads" << "OnAccessPrevention" ;
    keywords << "OnAccessDenyOnError" << "OnAccessExtraScanning" << "OnAccessRetryAttempts" << "ExtendedDetection" << "InfectedFiles";

    for (int i = 0; i < keywords.length(); i++) {
        if (setupFile->keywordExists("OnAccess",keywords.at(i)) == true) {
            switch (i) {
                case 0 : ui->accLogFileMaxSizeSpinBox->setValue(setupFile->getSectionIntValue("OnAccess","LogFileMaxSize"));
                            break;
                case 1 : ui->accLogTimesComboBox->setCurrentText(setupFile->getSectionValue("OnAccess","LogTimes"));
                            break;
                case 2 : ui->accLogRotateComboBox->setCurrentText(setupFile->getSectionValue("OnAccess","LogRotate"));
                            break;
                case 3 : ui->onAccessMaxFileSizeSpinBox->setValue(setupFile->getSectionDoubleValue("OnAccess","OnAccessMaxFileSize"));
                            break;
                case 4 : ui->onAccessMaxThreadsSpinBox->setValue(setupFile->getSectionDoubleValue("OnAccess","OnAccessMaxThreads"));
                            break;
                case 5 : ui->onAccessPreventionComboBox->setCurrentText(setupFile->getSectionValue("OnAccess","OnAccessPrevention"));
                            break;
                case 6 : ui->onAccessDenyOnErrorComboBox->setCurrentText(setupFile->getSectionValue("OnAccess","OnAccessDenyOnError"));
                            break;
                case 7 : ui->accExtraScanningComboBox->setCurrentText(setupFile->getSectionValue("OnAccess","OnAccessExtraScanning"));
                            break;
                case 8 : ui->accRetryAttemptsSpinBox->setValue(setupFile->getSectionDoubleValue("OnAccess","OnAccessRetryAttempts"));
                            break;
                case 9 : ui->extendedDetectionInfoComboBox->setCurrentText(setupFile->getSectionValue("OnAccess","ExtendedDetection"));
                            break;
                case 10: ui->infectedFilesComboBox->setCurrentIndex(setupFile->getSectionIntValue("OnAccess",keywords.at(i)));
                            break;
            }
        } else {
            switch (i) {
                case 0 : setupFile->setSectionValue("OnAccess","LogFileMaxSize","1");
                         clamdConf->setSingleLineValue("LogFileMaxSize","1");
                            break;
                case 1 : setupFile->setSectionValue("OnAccess",keywords.at(i),"yes");
                         clamdConf->setSingleLineValue("LogTime","yes");
                            break;
                case 2 : setupFile->setSectionValue("OnAccess",keywords.at(i),"yes");
                         clamdConf->setSingleLineValue("LogRotate","yes");
                            break;
                case 3 : setupFile->setSectionValue("OnAccess",keywords.at(i),"10");
                         clamdConf->setSingleLineValue("OnAccessMaxFileSize","10M");
                            break;
                case 4 : setupFile->setSectionValue("OnAccess",keywords.at(i),"10");
                         clamdConf->setSingleLineValue("OnAccessMaxThreads","10");
                            break;
                case 5 : setupFile->setSectionValue("OnAccess",keywords.at(i),"yes");
                         clamdConf->setSingleLineValue("OnAccessPrevention","yes");
                            break;
                case 6 : setupFile->setSectionValue("OnAccess",keywords.at(i),"no");
                         clamdConf->setSingleLineValue("OnAccessDenyOnError","no");
                            break;
                case 7 : setupFile->setSectionValue("OnAccess",keywords.at(i),"yes");
                         clamdConf->setSingleLineValue("OnAccessExtraScanning","yes");
                            break;
                case 8 : setupFile->setSectionValue("OnAccess",keywords.at(i),"0");
                         clamdConf->setSingleLineValue("OnAccessRetryAttempts","0");
                            break;
                case 9 : setupFile->setSectionValue("OnAccess",keywords.at(i),"yes");
                         clamdConf->setSingleLineValue("ExtendedDetection","yes");
                            break;
                case 10: break;
            }
        }
    }

    ui->restartClamdPushButton->setVisible(false);
    slot_updateClamdConf();

    parameters << "clamd";
    clamdLocationProcess->start("whereis",parameters);

    parameters.clear();
    parameters << "clamonacc";
    clamonaccLocationProcess->start("whereis",parameters);

    initprocessrunning = false;
}

void clamdManager::slot_updateClamdConf(){
    QStringList path;
    QString logPath = QDir::homePath() + "/.clamav-gui/clamd.log";
    QFile checkFile(logPath);

    if (checkFile.exists() == false) {
        if (checkFile.open(QIODevice::WriteOnly|QIODevice::Text)){
            checkFile.close();
        }
    } else {
        slot_logFileContentChanged();
    }

    path << logPath;

    clamdLogWatcher = new QFileSystemWatcher(path,this);
    connect(clamdLogWatcher,SIGNAL(fileChanged(QString)),this,SLOT(slot_logFileContentChanged()));

    setupFileHandler * helperHandler = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini");

    freshclamConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/freshclam.conf");

    clamdConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/clamd.conf");
    clamdConf->clearSetupFile();
    clamdConf->setSingleLineValue("DatabaseDirectory", freshclamConf->getSingleLineValue("DatabaseDirectory"));
    clamdConf->setSingleLineValue("LogSyslog", freshclamConf->getSingleLineValue("LogSyslog"));
    clamdConf->setSingleLineValue("LogFacility", freshclamConf->getSingleLineValue("LogFacility"));
    clamdConf->setSingleLineValue("PidFile", "/tmp/clamd.pid");
    clamdConf->setSingleLineValue("ExtendedDetectionInfo",setupFile->getSectionValue("OnAccess","ExtendedDetection"));
    clamdConf->setSingleLineValue("LocalSocket", QDir::homePath() + "/.clamav-gui/clamd-socket");
    clamdConf->setSingleLineValue("LogFile",QDir::homePath() + "/.clamav-gui/clamd.log");
    clamdConf->setSingleLineValue("LocalSocketGroup", "users");
    clamdConf->setSingleLineValue("TCPAddr", "127.0.0.1");
    clamdConf->setSingleLineValue("TCPAddr", "::1");
    clamdConf->setSingleLineValue("LogFileMaxSize",setupFile->getSectionValue("OnAccess", "LogFileMaxSize")+"M");
    clamdConf->setSingleLineValue("LogTime",setupFile->getSectionValue("OnAccess", "LogTimes"));
    clamdConf->setSingleLineValue("LogRotate",setupFile->getSectionValue("OnAccess", "LogRotate"));
    clamdConf->setSingleLineValue("OnAccessMaxFileSize",setupFile->getSectionValue("OnAccess", "OnAccessMaxFileSize")+"M");
    clamdConf->setSingleLineValue("OnAccessMaxThreads",setupFile->getSectionValue("OnAccess", "OnAccessMaxThreads"));
    clamdConf->setSingleLineValue("OnAccessPrevention",setupFile->getSectionValue("OnAccess", "OnAccessPrevention"));
    clamdConf->setSingleLineValue("OnAccessDenyOnError",setupFile->getSectionValue("OnAccess", "OnAccessDenyOnError"));
    clamdConf->setSingleLineValue("OnAccessExtraScanning",setupFile->getSectionValue("OnAccess", "OnAccessExtraScanning"));
    clamdConf->setSingleLineValue("OnAccessRetryAttempts",setupFile->getSectionValue("OnAccess", "OnAccessRetryAttempts"));
    clamdConf->setSingleLineValue("OnAccessExcludeUname","root");
    clamdConf->setSingleLineValue("OnAccessExcludeUID","0");

    QStringList watchList = setupFile->getKeywords("Clamonacc");
    foreach (QString entry, watchList) {
        clamdConf->addSingleLineValue("OnAccessIncludePath",entry);
    }

    QStringList SOKeywords;
    SOKeywords << "--alert-broken-media<equal>[yn]" << "--alert-broken<equal>[yn]" << "--alert-encrypted-archive<equal>[yn]" << "--alert-encrypted-doc<equal>[yn]";
    SOKeywords << "--alert-encrypted<equal>[yn]" << "--alert-exceeds-max<equal>[yn]" << "--alert-macros<equal>[yn]" << "--alert-partition-intersection<equal>[yn]";
    SOKeywords << "--alert-phishing-cloak<equal>[yn]" << "--alert-phishing-ssl<equal>[yn]" << "--allmatch<equal>[yn]" << "--bytecode-unsigned<equal>[yn]";
    SOKeywords << "--cross-fs<equal>[yn]" << "--debug" << "--detect-pua<equal>[yn]" << "--detect-structured<equal>[yn]" << "--heuristic-alerts<equal>[yn]";
    SOKeywords << "--heuristic-scan-precedence<equal>[yn]" << "--leave-temps<equal>[yn]" << "--nocerts" << "--official-db-only<equal>[yn]" << "--phishing-scan-urls<equal>[yn]";
    SOKeywords << "--phishing-sigs<equal>[yn]" << "--scan-archive<equal>[yn]" << "--scan-elf<equal>[yn]" << "--scan-html<equal>[yn]" << "--scan-hwp3<equal>[yn]";
    SOKeywords << "--scan-mail<equal>[yn]" << "--scan-ole2<equal>[yn]" << "--scan-pe<equal>[yn]" << "--scan-swf<equal>[yn]" << "--scan-xmldocs<equal>[yn]" << "--verbose";
    QStringList SOSwitches;
    SOSwitches << "AlertBrokenMedia" << "AlertBrokenExecutables" << "AlertEncryptedArchive" << "AlertEncryptedDoc" << "AlertEncrypted" << "AlertExceedsMax";
    SOSwitches << "AlertOLE2Macros" << "AlertPartitionIntersection" << "AlertPhishingCloak" << "AlertPhishingSSLMismatch" << "AllowAllMatchScan" << "BytecodeUnsigned";
    SOSwitches << "CrossFilesystems" << "Debug" << "DetectPUA" << "StructuredDataDetection" << "HeuristicAlerts" << "HeuristicScanPrecedence" << "LeaveTemporaryFiles";
    SOSwitches << "DisableCertCheck" << "OfficialDatabaseOnly" << "PhishingScanURLs" << "PhishingSignatures" << "ScanArchive" << "ScanELF" << "ScanHTML" << "ScanHWP3";
    SOSwitches << "ScanMail" << "ScanOLE2" << "ScanPDF" << "ScanSWF" << "ScanXMLDOCS" << "LogVerbose";

    for (int i = 0; i < SOKeywords.length(); i++){
        if (SOKeywords.at(i).indexOf("[yn]") != -1){
            QString tempkey = SOKeywords.at(i);
            tempkey = tempkey.mid(0,tempkey.indexOf("<equal>"));
            if (helperHandler->keywordExists("SelectedOptions",tempkey + "<equal>yes") == true) clamdConf->setSingleLineValue(SOSwitches.at(i), "yes");
            if (helperHandler->keywordExists("SelectedOptions",tempkey + "<equal>no") == true) clamdConf->setSingleLineValue(SOSwitches.at(i), "no");
        } else {
            if (helperHandler->keywordExists("SelectedOptions",SOKeywords.at(i)) == true) clamdConf->setSingleLineValue(SOSwitches.at(i), "yes");
        }
    }

    if ((helperHandler->sectionExists("REGEXP_and_IncludeExclude")) && (helperHandler->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").indexOf("checked|") == 0 )) clamdConf->addSingleLineValue("ExcludePath",helperHandler->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").mid(8));
    if ((helperHandler->sectionExists("REGEXP_and_IncludeExclude")) && (helperHandler->getSectionValue("REGEXP_and_IncludeExclude","DontScanFileNamesMatchingRegExp").indexOf("checked|") == 0 )) clamdConf->addSingleLineValue("ExcludePath",helperHandler->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").mid(8));
    if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","EnablePUAOptions") == true) {
        QStringList PUAKeywords;
        PUAKeywords << "LoadPUAPacked" << "LoadPUAPWTool" << "LoadPUANetTool" << "LoadPUAP2P" << "LoadPUAIRC" << "LoadPUARAT" << "LoadPUANetToolSpy" << "LoadPUAServer";
        PUAKeywords << "LoadPUAScript" << "LoadPUAAndr" << "LoadPUAJava" << "LoadPUAOsx" << "LoadPUATool" << "LoadPUAUnix" << "LoadPUAWin";

        QStringList PUASwitches;
        PUASwitches << "Packed" << "PWTool" << "NetTool" << "P2P" << "IRC" << "RAT" << "NetToolSpy" << "Server" << "Script" << "Andr" << "Java" << "Osx" << "Tool" << "Unix" << "Win";

        for (int i = 0; i < PUAKeywords.length(); i++){
            if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude",PUAKeywords.at(i)) == true) clamdConf->addSingleLineValue("IncludePUA",PUASwitches.at(i));
        }
    }
    if (helperHandler->getSectionValue("ScanLimitations","Files larger than this will be skipped and assumed clean").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxFileSize",helperHandler->getSectionValue("ScanLimitations","Files larger than this will be skipped and assumed clean").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","The maximum amount of data to scan for each container file").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxScanSize",helperHandler->getSectionValue("ScanLimitations","The maximum amount of data to scan for each container file").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","The maximum number of files to scan for each container file").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxFiles",helperHandler->getSectionValue("ScanLimitations","The maximum number of files to scan for each container file").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Max Scan-Time").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxScanTime",helperHandler->getSectionValue("ScanLimitations","Max Scan-Time").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum directory recursion level").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxRecursion",helperHandler->getSectionValue("ScanLimitations","Maximum directory recursion level").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum size file to check for embedded PE").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxEmbeddedPE",helperHandler->getSectionValue("ScanLimitations","Maximum size file to check for embedded PE").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum size of HTML file to normalize").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxHTMLNormalize",helperHandler->getSectionValue("ScanLimitations","Maximum size of HTML file to normalize").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum size of normalized HTML file to scan").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxHTMLNoTags",helperHandler->getSectionValue("ScanLimitations","Maximum size of normalized HTML file to scan").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum size of script file to normalize").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxScriptNormalize",helperHandler->getSectionValue("ScanLimitations","Maximum size of script file to normalize").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum size zip to type reanalyze").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxZipTypeRcg",helperHandler->getSectionValue("ScanLimitations","Maximum size zip to type reanalyze").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum number of partitions in disk image to be scanned").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxPartitions",helperHandler->getSectionValue("ScanLimitations","Maximum number of partitions in disk image to be scanned").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum number of icons in PE file to be scanned").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxIconsPE",helperHandler->getSectionValue("ScanLimitations","Maximum number of icons in PE file to be scanned").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Max recursion to HWP3 parsing function").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxRecHWP3",helperHandler->getSectionValue("ScanLimitations","Max recursion to HWP3 parsing function").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Max calls to PCRE match function").indexOf("checked|") == 0) clamdConf->setSingleLineValue("PCREMatchLimit",helperHandler->getSectionValue("ScanLimitations","Max calls to PCRE match function").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Max recursion calls to the PCRE match function").indexOf("checked|") == 0) clamdConf->setSingleLineValue("PCRERecMatchLimit",helperHandler->getSectionValue("ScanLimitations","Max recursion calls to the PCRE match function").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Max PCRE file size").indexOf("checked|") == 0) clamdConf->setSingleLineValue("PCREMaxFileSize",helperHandler->getSectionValue("ScanLimitations","Max PCRE file size").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Structured CC Count").indexOf("checked|") == 0) clamdConf->setSingleLineValue("StructuredMinCreditCardCount",helperHandler->getSectionValue("ScanLimitations","Structured CC Count").mid(8));
    if ((helperHandler->getSectionValue("ScanLimitations","Structured CC Mode").indexOf("checked|") == 0) && (helperHandler->getSectionValue("ScanLimitations","Structured CC Mode").right(1) == "1")) clamdConf->setSingleLineValue("StructuredCCOnly","yes");
    if (helperHandler->getSectionValue("ScanLimitations","Structured SSN Count").indexOf("checked|") == 0) clamdConf->setSingleLineValue("StructuredMinSSNCount",helperHandler->getSectionValue("ScanLimitations","Structured SSN Count").mid(8));
    if ((helperHandler->getSectionValue("ScanLimitations","Structured SSN Format").indexOf("checked|") == 0) && (helperHandler->getSectionValue("ScanLimitations","Structured SSN Format").right(1) == "0")) clamdConf->setSingleLineValue("StructuredSSNFormatNormal","yes");
    if ((helperHandler->getSectionValue("ScanLimitations","Structured SSN Format").indexOf("checked|") == 0) && (helperHandler->getSectionValue("ScanLimitations","Structured SSN Format").right(1) == "1")) clamdConf->setSingleLineValue("StructuredSSNFormatStripped","yes");
    if ((helperHandler->getSectionValue("ScanLimitations","Structured SSN Format").indexOf("checked|") == 0) && (helperHandler->getSectionValue("ScanLimitations","Structured SSN Format").right(1) == "2")) {
        clamdConf->setSingleLineValue("StructuredSSNFormatNormal","yes");
        clamdConf->setSingleLineValue("StructuredSSNFormatStripped","yes");
    }
    if (helperHandler->getSectionValue("ScanLimitations","Bytecode timeout in milliseconds").indexOf("checked|") == 0 ) clamdConf->setSingleLineValue("BytecodeTimeout",helperHandler->getSectionValue("ScanLimitations","Bytecode timeout in milliseconds").mid(8));
}

void clamdManager::slot_logFileContentChanged(){
    QFile file(QDir::homePath() + "/.clamav-gui/clamd.log");
    QString content;
    QString checkString;

    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    content = stream.readAll();
    file.close();

    QStringList lines =  content.split("\n");
    foreach (QString line, lines) {
        if ((line.indexOf("/") == 0) && (line.indexOf("FOUND") == line.length() - 5)) checkString = line;
    }

    if ((checkString != lastFound) && (checkString != "")) {
        lastFound = checkString;
        emit setBallonMessage(2,tr("WARNING"),lastFound);
    }

    ui->clamdLogPlainTextEdit->clear();
    ui->clamdLogPlainTextEdit->insertPlainText(content);
    ui->clamdLogPlainTextEdit->ensureCursorVisible();
}

void clamdManager::slot_clamdStartStopButtonClicked()
{
    QStringList monitorings = setupFile->getKeywords("Clamonacc");
    if (monitorings.length() > 0) {
        QStringList parameters;
        QFile pidFile("/tmp/clamd.pid");
        QString clamonaccOptions;
        int value = setupFile->getSectionIntValue("OnAccess","InfectedFiles");
        if (value == 1) clamonaccOptions = " --copy=" + QDir::homePath() + "/.clamav-gui/quarantine";
        if (value == 2) clamonaccOptions = " --move=" + QDir::homePath() + "/.clamav-gui/quarantine";
        if (value == 3) clamonaccOptions = " --remove=";

        if (checkClamdRunning() == false) {
            slot_updateClamdConf();

            ui->clamdIconLabel->setMovie(new QMovie(":/icons/icons/gifs/spinning_segments_small.gif"));
            ui->clamdIconLabel->movie()->start();

            clamdLogWatcher->removePath(QDir::homePath() + "/.clamav-gui/clamd.log");
            QFile logFile(QDir::homePath() + "/.clamav-gui/clamd.log");
            if (logFile.exists() == true) logFile.remove();
            logFile.open(QIODevice::ReadWrite);
            QTextStream stream(&logFile);
            stream << "";
            logFile.close();
            clamdLogWatcher->addPath(QDir::homePath() + "/.clamav-gui/clamd.log");

            ui->clamdLogPlainTextEdit->clear();
            ui->startStopClamdPushButton->setText(tr("  Clamd starting. Please wait!"));

            QFile startclamdFile(QDir::homePath() + "/.clamav-gui/startclamd.sh");
            if (startclamdFile.exists() == true) startclamdFile.remove();
            if (startclamdFile.open(QIODevice::Text|QIODevice::ReadWrite)){
                QString logFile = clamdConf->getSingleLineValue("LogFile");
                QTextStream stream(&startclamdFile);
                stream << "#!/bin/bash\n" << clamdLocation + " 2> " + logFile + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf && " + clamonaccLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf -l " + QDir::homePath() + "/.clamav-gui/clamd.log" + clamonaccOptions;
                startclamdFile.close();
                startclamdFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup);
            }
            parameters << QDir::homePath() + "/.clamav-gui/startclamd.sh";
            if (sudoGUI == "") sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");
            startClamdProcess->start(sudoGUI,parameters);
            setupFile->setSectionValue("Clamd","Status","starting up ...");
            emit systemStatusChanged();
        } else {
            setupFile->setSectionValue("Clamd","Status","shutting down ...");
            emit systemStatusChanged();
            pidFile.open(QIODevice::ReadOnly);
            QTextStream stream(&pidFile);
            QString pid = stream.readLine();
            pidFile.close();
            ui->startStopClamdPushButton->setText(tr("  Stopping Clamd. Please wait!"));
            QFile stopclamdFile(QDir::homePath() + "/.clamav-gui/stopclamd.sh");
            if (stopclamdFile.exists() == true) stopclamdFile.remove();
            if (stopclamdFile.open(QIODevice::Text|QIODevice::ReadWrite)){
                QTextStream stream(&stopclamdFile);
                stream << "#!/bin/bash\n/bin/kill -sigterm " + pid + " && kill -9 " + clamonaccPid;
                stopclamdFile.close();
                stopclamdFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup);
            }
            parameters << QDir::homePath() + "/.clamav-gui/stopclamd.sh";
            if (sudoGUI == "") sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");
            killProcess->start(sudoGUI,parameters);
        }
        ui->startStopClamdPushButton->setEnabled(false);
        ui->monitoringAddButton->setEnabled(false);
        ui->monitoringDelButton->setEnabled(false);
    } else {
        clamdStartupCounter = 0;
        QMessageBox::warning(this,tr("WARNING"),tr("Clamd and Clamonacc can not be launched. First you have to add at least one folder for monitoring!"));
    }
}

void clamdManager::slot_startClamdProcessFinished(int exitCode, QProcess::ExitStatus exitStatus){
    if ((exitCode != 0) || (exitStatus == QProcess::CrashExit))clamdStartupCounter = 0;
    ui->clamdIconLabel->clear();
    ui->clamdIconLabel->setPixmap(QPixmap(":/icons/icons/onaccess.png"));
    ui->clamdIconLabel_2->clear();;
    ui->clamdIconLabel_2->setPixmap(QPixmap(":/icons/icons/options.png"));

    if (checkClamdRunning() == false) {
        clamdPidWatcher->removePath("/tmp/clamd.pid");

        ui->startStopClamdPushButton->setStyleSheet("background-color:red;color:yellow");
        ui->startStopClamdPushButton->setText(tr("  Clamd not running - Start Clamd"));
        ui->startStopClamdPushButton->setIcon(QIcon(":/icons/icons/startclamd.png"));

        setupFile->setSectionValue("Clamd","ClamdPid","n/a");
        setupFile->setSectionValue("Clamd","ClamonaccPid","n/a");
        setupFile->setSectionValue("Clamd","Status","not running");

        emit systemStatusChanged();

        if (clamdStartupCounter > 0) {
            if (waitForFreshclam == false) clamdStartupCounter--;
            startDelayTimer->start(2500);
        }
    } else {
        clamdStartupCounter = 0;

        clamdPidWatcher->addPath("/tmp/clamd.pid");

        QFile pidFile("/tmp/clamd.pid");
        pidFile.open(QIODevice::ReadOnly);
        QTextStream stream(&pidFile);
        QString pid = stream.readLine();
        pid = pid.replace("\n","");

        setupFile->setSectionValue("Clamd","ClamdPid",pid);
        setupFile->setSectionValue("Clamd","Status","is running");

        pidFile.close();

        emit systemStatusChanged();

        ui->startStopClamdPushButton->setStyleSheet("background-color:green;color:yellow");
        ui->startStopClamdPushButton->setText(tr("  Clamd running - Stop Clamd"));
        ui->startStopClamdPushButton->setIcon(QIcon(":/icons/icons/stopclamd.png"));

        slot_logFileContentChanged();

        QStringList parameters;
        parameters << "-s" << "clamonacc";
        findclamonaccProcess->start("pidof",parameters);
    }

    clamdRestartInProgress = false;

    ui->startStopClamdPushButton->setEnabled(true);
    ui->monitoringAddButton->setEnabled(true);
    ui->monitoringDelButton->setEnabled(true);
    ui->restartClamdPushButton->setVisible(false);
}

void clamdManager::slot_killClamdProcessFinished(){
    ui->clamdIconLabel->clear();
    ui->clamdIconLabel->setPixmap(QPixmap(":/icons/icons/onaccess.png"));
    ui->clamdIconLabel_2->clear();;
    ui->clamdIconLabel_2->setPixmap(QPixmap(":/icons/icons/options.png"));

    if (checkClamdRunning() == false) {
        clamdPidWatcher->removePath("/tmp/clamd.pid");
        ui->startStopClamdPushButton->setStyleSheet("background-color:red;color:yellow");
        ui->startStopClamdPushButton->setText(tr("  Clamd not running - Start Clamd"));
        ui->startStopClamdPushButton->setIcon(QIcon(":/icons/icons/startclamd.png"));

        setupFile->setSectionValue("Clamd","ClamdPid","n/a");
        setupFile->setSectionValue("Clamd","ClamonaccPid","n/a");
        setupFile->setSectionValue("Clamd","Status","shut down");

        emit systemStatusChanged();
    } else {
        clamdPidWatcher->addPath("/tmp/clamd.pid");

        ui->startStopClamdPushButton->setStyleSheet("background-color:green;color:yellow");
        ui->startStopClamdPushButton->setText(tr("  Clamd running - Stop Clamd"));
        ui->startStopClamdPushButton->setIcon(QIcon(":/icons/icons/stopclamd.png"));
    }

    ui->startStopClamdPushButton->setEnabled(true);
    ui->monitoringAddButton->setEnabled(true);
    ui->monitoringDelButton->setEnabled(true);
}

void clamdManager::slot_findclamonaccProcessFinished(int rc){
    if (rc == 0) {
        clamonaccPid = findclamonaccProcess->readAllStandardOutput();
        clamonaccPid = clamonaccPid.replace("\n","");
        setupFile->setSectionValue("Clamd","ClamonaccPid",clamonaccPid);
        emit systemStatusChanged();
    } else {
        setupFile->setSectionValue("Clamd","ClamonaccPid","n/a");
        emit systemStatusChanged();
    }

    if ((setupFile->keywordExists("Clamd","StartClamdOnStartup") == true) && (setupFile->getSectionBoolValue("Clamd","StartClamdOnStartup") == true)) {
        ui->startClamdOnStartupCheckBox->setChecked(setupFile->getSectionBoolValue("Clamd","StartClamdOnStartup"));
    }
}

void clamdManager::slot_startDelayTimerExpired(){
    if (checkClamdRunning() == false) {
        emit setActiveTab();
        slot_clamdStartStopButtonClicked();
    }
}

void clamdManager::slot_waitForFreshclamStarted(){
    waitForFreshclam = false;

    if ((startup == true) && (setupFile->keywordExists("Clamd","StartClamdOnStartup") == true) && (setupFile->getSectionBoolValue("Clamd","StartClamdOnStartup") == true)) {
        ui->startClamdOnStartupCheckBox->setChecked(setupFile->getSectionBoolValue("Clamd","StartClamdOnStartup"));

        clamdStartupCounter = 5;
        if (startup == true) startDelayTimer->start(2500);
        startup = false;
    }
}

void clamdManager::slot_processWatcherExpired(){
    QString clamdPid = setupFile->getSectionValue("Clamd","ClamdPid");
    QString clamonaccPid = setupFile->getSectionValue("Clamd","ClamonaccPid");

    QDir checkDir;
    if ( clamdPid != "n/a") {
        if (checkDir.exists("/proc/" + clamdPid) == false) {
            setupFile->setSectionValue("Clamd","ClamdPid","n/a");

            emit systemStatusChanged();

            slot_killClamdProcessFinished();
        }
    }

    if ( clamonaccPid != "n/a") {
        if (checkDir.exists("/proc/" + clamonaccPid) == false) {
            setupFile->setSectionValue("Clamd","ClamonaccPid","n/a");
            emit systemStatusChanged();
        }
    }
}


void clamdManager::slot_pidWatcherTriggered(){
    QFile pidFile("/tmp/clamd.pid");
    if ((pidFile.exists() == false) && (clamdRestartInProgress == false)){
        clamdPidWatcher->removePath("/tmp/clamd.pid");

        ui->startStopClamdPushButton->setStyleSheet("background-color:red;color:yellow");
        ui->startStopClamdPushButton->setText(tr("  Clamd not running - Start Clamd"));
        ui->startStopClamdPushButton->setIcon(QIcon(":/icons/icons/startclamd.png"));
        ui->startStopClamdPushButton->setEnabled(true);
        ui->monitoringAddButton->setEnabled(true);
        ui->monitoringDelButton->setEnabled(true);

        setupFile->setSectionValue("Clamd","ClamdPid","n/a");
        setupFile->setSectionValue("Clamd","ClamonaccPid","n/a");
        setupFile->setSectionValue("Clamd","Status","not running");

        emit systemStatusChanged();
    }
}

void clamdManager::slot_clamdLocationProcessFinished(){
    QString searchstring = "clamd:";
    QString output = clamdLocationProcess->readAll();
    if (output == searchstring + "\n") {
        ui->startStopClamdPushButton->setEnabled(false);
        ui->monitoringAddButton->setEnabled(false);
        ui->monitoringDelButton->setEnabled(false);

        setupFile->setSectionValue("Clamd","ClamdLocation","n/a");

        emit systemStatusChanged();
    } else {
        clamdLocation = trimLocationOutput(output);

        setupFile->setSectionValue("Clamd","ClamdLocation",clamdLocation);

        emit systemStatusChanged();
    }
}

void clamdManager::slot_clamonaccLocationProcessFinished(){
    QString searchstring = "clamonacc:";
    QString output = clamonaccLocationProcess->readAll();
    if (output == searchstring + "\n") {
        ui->startStopClamdPushButton->setEnabled(false);
        ui->monitoringAddButton->setEnabled(false);
        ui->monitoringDelButton->setEnabled(false);

        setupFile->setSectionValue("Clamd","ClamonaccLocation","n/a");

        emit systemStatusChanged();
    } else {
        clamonaccLocation = trimLocationOutput(output);
        setupFile->setSectionValue("Clamd","ClamonaccLocation",clamonaccLocation);

        emit systemStatusChanged();

        QStringList parameters;
        parameters << "-s" << "clamonacc";
        findclamonaccProcess->start("pidof",parameters);
    }
}

void clamdManager::slot_startClamdOnStartupCheckBoxClicked(){
    setupFile->setSectionValue("Clamd","StartClamdOnStartup",ui->startClamdOnStartupCheckBox->isChecked());
}

void clamdManager::slot_monitoringAddButtonClicked(){
    QString path = QFileDialog::getExistingDirectory(this,tr("Directory to monitor"),QDir::homePath());
    if (path != "") {
        if (ui->monitoringComboBox->findText(path) == -1) {
            ui->monitoringComboBox->addItem(path);
            setupFile->setSectionValue("Clamonacc",path,"under monitoring");
            clamdConf->addSingleLineValue("OnAccessIncludePath",path);

            if (checkClamdRunning() == true) restartClamonacc();
        } else {
            QMessageBox::warning(this,tr("WARNING"),tr("Path already under monitoring"));
        }
    }
}

void clamdManager::slot_monitoringDelButtonClicked(){
    QString entry = ui->monitoringComboBox->currentText();
    if (entry != "") {
        if (QMessageBox::information(this,tr("Remove Folder from monitoring"),tr("Path: ") + entry + "\n" + tr("Do you want to remove the folder from the monitoring list?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes) {
            setupFile->removeKeyword("Clamonacc",entry);
            ui->monitoringComboBox->removeItem(ui->monitoringComboBox->currentIndex());
            clamdConf->removeSingleLine("OnAccessIncludePath",entry);
            if (checkClamdRunning() == true) restartClamonacc();
        }
    }
}

void clamdManager::slot_restartClamonaccProcessFinished(){
    QStringList parameters;
    parameters << "-s" << "clamonacc";
    findclamonaccProcess->start("pidof",parameters);
}

void clamdManager::slot_restartClamdButtonClicked(){
    QFile pidFile("/tmp/clamd.pid");
    pidFile.open(QIODevice::ReadOnly);
    QTextStream stream(&pidFile);
    QString pid = stream.readLine();
    pidFile.close();

    setupFile->setSectionValue("Clamd","ClamdPid",pid);

    QString clamonaccOptions;
    int value = setupFile->getSectionIntValue("OnAccess","InfectedFiles");
    if (value == 1) clamonaccOptions = " --copy=" + QDir::homePath() + "/.clamav-gui/quarantine";
    if (value == 2) clamonaccOptions = " --move=" + QDir::homePath() + "/.clamav-gui/quarantine";
    if (value == 3) clamonaccOptions = " --remove=";

    clamdRestartInProgress = true;

    ui->startStopClamdPushButton->setEnabled(false);
    ui->monitoringAddButton->setEnabled(false);
    ui->monitoringDelButton->setEnabled(false);
    ui->startStopClamdPushButton->setText(tr("  Clamd restarting. Please wait!"));
    ui->clamdIconLabel_2->setMovie(new QMovie(":/icons/icons/gifs/spinning_segments_small.gif"));
    ui->clamdIconLabel_2->movie()->start();

    QStringList parameters;

    QFile startclamdFile(QDir::homePath() + "/.clamav-gui/startclamd.sh");
    startclamdFile.remove();
    if (startclamdFile.open(QIODevice::Text|QIODevice::ReadWrite)){
        QTextStream stream(&startclamdFile);
        stream << "#!/bin/bash\n" << "kill -sigterm " + pid + " && kill -9 " + clamonaccPid + " && sleep 20 && " + clamdLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf && " + clamonaccLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf -l " + QDir::homePath() + "/.clamav-gui/clamd.log" + clamonaccOptions;
        startclamdFile.close();
        startclamdFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup);
    }
    parameters << QDir::homePath() + "/.clamav-gui/startclamd.sh";

    if (sudoGUI == "") sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");

    startClamdProcess->start(sudoGUI,parameters);

    setupFile->setSectionValue("Clamd","Status","starting up ...");
    setupFile->setSectionValue("Clamd","ClamdPid","n/a");
    setupFile->setSectionValue("Clamd","ClamonaccPid","n/a");

    emit systemStatusChanged();
}

void clamdManager::slot_clamdSettingsChanged(){
    if (initprocessrunning == false) {
        setupFile->setSectionValue("OnAccess","ExtendedDetectionInfo",ui->extendedDetectionInfoComboBox->currentText());
        setupFile->setSectionValue("OnAccess","LogFileMaxSize",QString::number(ui->accLogFileMaxSizeSpinBox->value()));
        setupFile->setSectionValue("OnAccess","LogTimes",ui->accLogTimesComboBox->currentText());
        setupFile->setSectionValue("OnAccess","LogRotate",ui->accLogRotateComboBox->currentText());
        setupFile->setSectionValue("OnAccess","OnAccessMaxFileSize",QString::number(ui->onAccessMaxFileSizeSpinBox->value()));
        setupFile->setSectionValue("OnAccess","OnAccessMaxThreads",ui->onAccessMaxThreadsSpinBox->value());
        setupFile->setSectionValue("OnAccess","OnAccessPrevention",ui->onAccessPreventionComboBox->currentText());
        setupFile->setSectionValue("OnAccess","OnAccessDenyOnError",ui->onAccessDenyOnErrorComboBox->currentText());
        setupFile->setSectionValue("OnAccess","OnAccessExtraScanning",ui->accExtraScanningComboBox->currentText());
        setupFile->setSectionValue("OnAccess","OnAccessRetryAttempts",ui->accRetryAttemptsSpinBox->value());
        setupFile->setSectionValue("OnAccess","InfectedFiles",ui->infectedFilesComboBox->currentIndex());

        clamdConf->setSingleLineValue("LogFileMaxSize",setupFile->getSectionValue("OnAccess", "LogFileMaxSize")+"M");
        clamdConf->setSingleLineValue("LogTime",setupFile->getSectionValue("OnAccess", "LogTimes"));
        clamdConf->setSingleLineValue("LogRotate",setupFile->getSectionValue("OnAccess", "LogRotate"));
        clamdConf->setSingleLineValue("OnAccessMaxFileSize",setupFile->getSectionValue("OnAccess", "OnAccessMaxFileSize")+"M");
        clamdConf->setSingleLineValue("OnAccessMaxThreads",setupFile->getSectionValue("OnAccess", "OnAccessMaxThreads"));
        clamdConf->setSingleLineValue("OnAccessPrevention",setupFile->getSectionValue("OnAccess", "OnAccessPrevention"));
        clamdConf->setSingleLineValue("OnAccessDenyOnError",setupFile->getSectionValue("OnAccess", "OnAccessDenyOnError"));
        clamdConf->setSingleLineValue("OnAccessExtraScanning",setupFile->getSectionValue("OnAccess", "OnAccessExtraScanning"));
        clamdConf->setSingleLineValue("OnAccessRetryAttempts",setupFile->getSectionValue("OnAccess", "OnAccessRetryAttempts"));

        if (checkClamdRunning() == true) {
            ui->restartClamdPushButton->setVisible(true);
            ui->restartClamdPushButton->setStyleSheet("background-color:green");
        }
    }
}

void clamdManager::restartClamonacc(){
    QStringList parameters;

    QFile restartclamdFile(QDir::homePath() + "/.clamav-gui/restartclamd.sh");
    restartclamdFile.remove();

    if (restartclamdFile.open(QIODevice::Text|QIODevice::ReadWrite)){
        QTextStream stream(&restartclamdFile);
        stream << "#!/bin/bash\n/bin/kill -9 " + clamonaccPid + " && " + clamonaccLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf -l " + QDir::homePath() + "/.clamav-gui/clamd.log";
        restartclamdFile.close();
        restartclamdFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup);
    }

    parameters << QDir::homePath() + "/.clamav-gui/restartclamd.sh";

    if (sudoGUI == "") sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");

    startClamdProcess->start(sudoGUI,parameters);

    setupFile->setSectionValue("Clamd","Status","shutting down ...");

    emit systemStatusChanged();
}

bool clamdManager::checkClamdRunning()
{
    bool rc = false;
    QProcess checkPIDProcess;
    QStringList parameters;

    parameters << "clamd";

    int pid = checkPIDProcess.execute("pidof",parameters);
    if (pid == 0) {
        rc = true;
        QFile pidFile("/tmp/clamd.pid");
        pidFile.open(QIODevice::ReadOnly);
        QTextStream stream(&pidFile);
        QString pidString = stream.readLine();
        pidString = pidString.replace("\n","");
        setupFile->setSectionValue("Clamd","ClamdPid",pidString);
        setupFile->setSectionValue("Clamd","Status","is running");
        pidFile.close();

        emit systemStatusChanged();
    } else {
        setupFile->setSectionValue("Clamd","ClamdPid","n/a");
        setupFile->setSectionValue("Clamd","Status","not running");

        emit systemStatusChanged();
    }

    return rc;
}
