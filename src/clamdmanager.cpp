#include "clamdmanager.h"
#include "ui_clamdmanager.h"
#define css_red "background-color:red;color:yellow"
#define css_green "background-color:green;color:yellow"
#define css_mono "background-color:#404040;color:white"

clamdManager::clamdManager(QWidget *parent) : QWidget(parent), ui(new Ui::clamdManager){
    ui->setupUi(this);
    startup = true;
    monochrome = false;
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
    dirsUnderMonitoring = 0;
    logHighlighter = NULL;

    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini", this);
    sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");

    QFile clamdConfFile(QDir::homePath() + "/.clamav-gui/clamd.conf");
    clamdConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/clamd.conf", this);
    if (clamdConfFile.exists() == false) {        
        QString value = setupFile->getSectionValue("Directories","LoadSupportedDBFiles");
        if (value.indexOf("checked|") == 0) clamdConf->addSingleLineValue("DatabaseDirectory",value.mid(value.indexOf("|") + 1));
        clamdConf->setSingleLineValue("LogSyslog","no");
        clamdConf->setSingleLineValue("LogFacility","LOG_LOCAL6");
        clamdConf->setSingleLineValue("PidFile","/tmp/clamd.pid");
        clamdConf->setSingleLineValue("ExtendedDetectionInfo","yes");
        clamdConf->setSingleLineValue("LocalSocket",QDir::homePath() + "/.clamav-gui/clamd-socket");
        clamdConf->setSingleLineValue("LogFile",QDir::homePath() + "/.clamav-gui/clamd.log");
        clamdConf->setSingleLineValue("LocalSocketGroup","users");
        clamdConf->setSingleLineValue("TCPAddr","127.0.0.1");
        clamdConf->setSingleLineValue("TCPAddr","::1");
        clamdConf->setSingleLineValue("LogFileMaxSize","1M");
        clamdConf->setSingleLineValue("LogTime","yes");
        clamdConf->setSingleLineValue("LogRotate","yes");
        clamdConf->setSingleLineValue("OnAccessMaxFileSize","10M");
        clamdConf->setSingleLineValue("OnAccessMaxThreads","10");
        clamdConf->setSingleLineValue("OnAccessPrevention","yes");
        clamdConf->setSingleLineValue("OnAccessDenyOnError","no");
        clamdConf->setSingleLineValue("OnAccessExtraScanning","yes");
        clamdConf->setSingleLineValue("OnAccessRetryAttempts","0");
        clamdConf->setSingleLineValue("OnAccessExcludeUname","root");
        clamdConf->setSingleLineValue("OnAccessExcludeUID","0");
    } 

    QStringList parameters;
    QStringList monitorings = setupFile->getKeywords("Clamonacc");
    dirsUnderMonitoring = monitorings.length();

    ui->monitoringComboBox->addItems(monitorings);

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
        ui->startStopClamdPushButton->setStyleSheet(selectColor("green"));
        ui->startStopClamdPushButton->setText(tr("  Clamd running - Stop clamd"));
        ui->startStopClamdPushButton->setIcon(QIcon(":/icons/icons/stopclamd.png"));
        clamdPidWatcher->addPath("/tmp/clamd.pid");
    } else {
        ui->startStopClamdPushButton->setStyleSheet(selectColor("red"));
    }

    clamdRestartInProgress = false;

    /*keywords << "LogFileMaxSize" << "LogTimes" << "LotRotate" << "OnAccessMaxFileSize" << "OnAccessMaxThreads" << "OnAccessPrevention" ;
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
    }*/

    ui->restartClamdPushButton->setVisible(false);
    slot_updateClamdConf();

    parameters << "clamd";
    clamdLocationProcess->start("whereis",parameters);

    parameters.clear();
    parameters << "clamonacc";
    clamonaccLocationProcess->start("whereis",parameters);

    initprocessrunning = false;
    initClamdConfElements();
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

    freshclamConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/freshclam.conf", this);
    QStringList watchList = setupFile->getKeywords("Clamonacc");
    foreach (QString entry, watchList) {
        clamdConf->addSingleLineValue("OnAccessIncludePath",entry);
    }
// obsolete ---------
    /*QStringList SOKeywords;
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
            if (setupFile->keywordExists("SelectedOptions",tempkey + "<equal>yes") == true) clamdConf->setSingleLineValue(SOSwitches.at(i), "yes");
            if (setupFile->keywordExists("SelectedOptions",tempkey + "<equal>no") == true) clamdConf->setSingleLineValue(SOSwitches.at(i), "no");
        } else {
            if (setupFile->keywordExists("SelectedOptions",SOKeywords.at(i)) == true) clamdConf->setSingleLineValue(SOSwitches.at(i), "yes");
        }
    } ----- */

    if ((setupFile->sectionExists("REGEXP_and_IncludeExclude")) && (setupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").indexOf("checked|") == 0 )) clamdConf->addSingleLineValue("ExcludePath",setupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").mid(8));
    if ((setupFile->sectionExists("REGEXP_and_IncludeExclude")) && (setupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanFileNamesMatchingRegExp").indexOf("checked|") == 0 )) clamdConf->addSingleLineValue("ExcludePath",setupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").mid(8));
    if (setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","EnablePUAOptions") == true) {
        QStringList PUAKeywords;
        PUAKeywords << "LoadPUAPacked" << "LoadPUAPWTool" << "LoadPUANetTool" << "LoadPUAP2P" << "LoadPUAIRC" << "LoadPUARAT" << "LoadPUANetToolSpy" << "LoadPUAServer";
        PUAKeywords << "LoadPUAScript" << "LoadPUAAndr" << "LoadPUAJava" << "LoadPUAOsx" << "LoadPUATool" << "LoadPUAUnix" << "LoadPUAWin";

        QStringList PUASwitches;
        PUASwitches << "Packed" << "PWTool" << "NetTool" << "P2P" << "IRC" << "RAT" << "NetToolSpy" << "Server" << "Script" << "Andr" << "Java" << "Osx" << "Tool" << "Unix" << "Win";

        for (int i = 0; i < PUAKeywords.length(); i++){
            if (setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude",PUAKeywords.at(i)) == true) clamdConf->addSingleLineValue("IncludePUA",PUASwitches.at(i));
        }
    }
    /*if (setupFile->getSectionValue("ScanLimitations","Files larger than this will be skipped and assumed clean").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxFileSize",setupFile->getSectionValue("ScanLimitations","Files larger than this will be skipped and assumed clean").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","The maximum amount of data to scan for each container file").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxScanSize",setupFile->getSectionValue("ScanLimitations","The maximum amount of data to scan for each container file").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","The maximum number of files to scan for each container file").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxFiles",setupFile->getSectionValue("ScanLimitations","The maximum number of files to scan for each container file").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Max Scan-Time").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxScanTime",setupFile->getSectionValue("ScanLimitations","Max Scan-Time").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Maximum directory recursion level").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxRecursion",setupFile->getSectionValue("ScanLimitations","Maximum directory recursion level").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Maximum size file to check for embedded PE").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxEmbeddedPE",setupFile->getSectionValue("ScanLimitations","Maximum size file to check for embedded PE").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Maximum size of HTML file to normalize").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxHTMLNormalize",setupFile->getSectionValue("ScanLimitations","Maximum size of HTML file to normalize").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Maximum size of normalized HTML file to scan").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxHTMLNoTags",setupFile->getSectionValue("ScanLimitations","Maximum size of normalized HTML file to scan").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Maximum size of script file to normalize").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxScriptNormalize",setupFile->getSectionValue("ScanLimitations","Maximum size of script file to normalize").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Maximum size zip to type reanalyze").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxZipTypeRcg",setupFile->getSectionValue("ScanLimitations","Maximum size zip to type reanalyze").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Maximum number of partitions in disk image to be scanned").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxPartitions",setupFile->getSectionValue("ScanLimitations","Maximum number of partitions in disk image to be scanned").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Maximum number of icons in PE file to be scanned").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxIconsPE",setupFile->getSectionValue("ScanLimitations","Maximum number of icons in PE file to be scanned").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Max recursion to HWP3 parsing function").indexOf("checked|") == 0) clamdConf->setSingleLineValue("MaxRecHWP3",setupFile->getSectionValue("ScanLimitations","Max recursion to HWP3 parsing function").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Max calls to PCRE match function").indexOf("checked|") == 0) clamdConf->setSingleLineValue("PCREMatchLimit",setupFile->getSectionValue("ScanLimitations","Max calls to PCRE match function").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Max recursion calls to the PCRE match function").indexOf("checked|") == 0) clamdConf->setSingleLineValue("PCRERecMatchLimit",setupFile->getSectionValue("ScanLimitations","Max recursion calls to the PCRE match function").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Max PCRE file size").indexOf("checked|") == 0) clamdConf->setSingleLineValue("PCREMaxFileSize",setupFile->getSectionValue("ScanLimitations","Max PCRE file size").mid(8));
    if (setupFile->getSectionValue("ScanLimitations","Structured CC Count").indexOf("checked|") == 0) clamdConf->setSingleLineValue("StructuredMinCreditCardCount",setupFile->getSectionValue("ScanLimitations","Structured CC Count").mid(8));
    if ((setupFile->getSectionValue("ScanLimitations","Structured CC Mode").indexOf("checked|") == 0) && (setupFile->getSectionValue("ScanLimitations","Structured CC Mode").right(1) == "1")) clamdConf->setSingleLineValue("StructuredCCOnly","yes");
    if (setupFile->getSectionValue("ScanLimitations","Structured SSN Count").indexOf("checked|") == 0) clamdConf->setSingleLineValue("StructuredMinSSNCount",setupFile->getSectionValue("ScanLimitations","Structured SSN Count").mid(8));
    if ((setupFile->getSectionValue("ScanLimitations","Structured SSN Format").indexOf("checked|") == 0) && (setupFile->getSectionValue("ScanLimitations","Structured SSN Format").right(1) == "0")) clamdConf->setSingleLineValue("StructuredSSNFormatNormal","yes");
    if ((setupFile->getSectionValue("ScanLimitations","Structured SSN Format").indexOf("checked|") == 0) && (setupFile->getSectionValue("ScanLimitations","Structured SSN Format").right(1) == "1")) clamdConf->setSingleLineValue("StructuredSSNFormatStripped","yes");
    if ((setupFile->getSectionValue("ScanLimitations","Structured SSN Format").indexOf("checked|") == 0) && (setupFile->getSectionValue("ScanLimitations","Structured SSN Format").right(1) == "2")) {
        clamdConf->setSingleLineValue("StructuredSSNFormatNormal","yes");
        clamdConf->setSingleLineValue("StructuredSSNFormatStripped","yes");
    }
    if (setupFile->getSectionValue("ScanLimitations","Bytecode timeout in milliseconds").indexOf("checked|") == 0 ) clamdConf->setSingleLineValue("BytecodeTimeout",setupFile->getSectionValue("ScanLimitations","Bytecode timeout in milliseconds").mid(8));*/
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

    QStringList parameters;
    QFile pidFile("/tmp/clamd.pid");
    QString clamonaccOptions;

    if (setupFile->getSectionValue("Directories","MoveInfectedFiles").indexOf("checked") == 0) {
        QString path = setupFile->getSectionValue("Directories","MoveInfectedFiles");
        path = path.mid(path.indexOf("|") + 1);
        clamonaccOptions = " --move=" + path;
    }

    if (setupFile->getSectionValue("Directories","CopyInfectedFiles").indexOf("checked") == 0) {
        QString path = setupFile->getSectionValue("Directories","CopyInfectedFiles");
        path = path.mid(path.indexOf("|") + 1);
        clamonaccOptions = clamonaccOptions + " --copy=" + path;
    }

    if (setupFile->getSectionValue("Directories","ScanReportToFile").indexOf("checked") == 0) {
        QString path = setupFile->getSectionValue("Directories","ScanReportToFile");
        path = path.mid(path.indexOf("|") + 1);
        clamonaccOptions = clamonaccOptions + " --log=" + path;
    }

    if (setupFile->keywordExists("Directories","--verbose") == true) clamonaccOptions += " --verbose";
    if (setupFile->keywordExists("SelectedOptions","--remove<equal>yes") == true) clamonaccOptions += "--remove";
    if (setupFile->keywordExists("SelectedOptions","--allmatch<equal>yes") == true) clamonaccOptions += "--allmatch";

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
            if (monitorings.length() > 0) {
                stream << "#!/bin/bash\n" << clamdLocation + " 2> " + logFile + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf && " + clamonaccLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf -l " + QDir::homePath() + "/.clamav-gui/clamd.log" + clamonaccOptions;
            } else  {
                stream << "#!/bin/bash\n" << clamdLocation + " 2> " + logFile + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf";
                setupFile->setSectionValue("Clamd","Status2","n/a");
            }
            startclamdFile.close();
            startclamdFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup);
        }
        parameters << QDir::homePath() + "/.clamav-gui/startclamd.sh";
        if (sudoGUI == "") sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");
        startClamdProcess->start(sudoGUI,parameters);
        setupFile->setSectionValue("Clamd","Status","starting up ...");
        if (dirsUnderMonitoring > 0) setupFile->setSectionValue("Clamd","Status2","starting up ...");
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
            if (clamonaccPid != "n/a") {
                stream << "#!/bin/bash\n/bin/kill -sigterm " + pid + " && kill -9 " + clamonaccPid;
            } else {
                stream << "#!/bin/bash\n/bin/kill -sigterm " + pid;
            }
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
}

void clamdManager::slot_startClamdProcessFinished(int exitCode, QProcess::ExitStatus exitStatus){
    if ((exitCode != 0) || (exitStatus == QProcess::CrashExit))clamdStartupCounter = 0;
    ui->clamdIconLabel->clear();
    ui->clamdIconLabel->setPixmap(QPixmap(":/icons/icons/onaccess.png"));
    ui->clamdIconLabel_2->clear();;
    ui->clamdIconLabel_2->setPixmap(QPixmap(":/icons/icons/options.png"));

    if (checkClamdRunning() == false) {
        clamdPidWatcher->removePath("/tmp/clamd.pid");
        ui->startStopClamdPushButton->setStyleSheet(selectColor("red"));
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
        ui->startStopClamdPushButton->setStyleSheet(selectColor("green"));
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
        ui->startStopClamdPushButton->setStyleSheet(selectColor("red"));
        ui->startStopClamdPushButton->setText(tr("  Clamd not running - Start Clamd"));
        ui->startStopClamdPushButton->setIcon(QIcon(":/icons/icons/startclamd.png"));

        setupFile->setSectionValue("Clamd","ClamdPid","n/a");
        setupFile->setSectionValue("Clamd","ClamonaccPid","n/a");
        setupFile->setSectionValue("Clamd","Status","shut down");
        if (dirsUnderMonitoring > 0) setupFile->setSectionValue("Clamd","Status2","shut down");

        emit systemStatusChanged();
    } else {
        clamdPidWatcher->addPath("/tmp/clamd.pid");
        ui->startStopClamdPushButton->setStyleSheet(selectColor("green"));
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
        clamonaccPid = "";
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

void clamdManager::slot_add_remove_highlighter(bool state)
{
    if (state == true) {
      if (logHighlighter != NULL) {
        delete logHighlighter;
        logHighlighter = NULL;
      }
      monochrome = true;
      ui->clamonaccLabel->setStyleSheet("padding:3px");
      ui->clamdLabel_3->setStyleSheet("padding:3px");
    } else {
         if (logHighlighter == NULL) {
             logHighlighter = new highlighter(ui->clamdLogPlainTextEdit->document());
         } else {
             delete logHighlighter;
             logHighlighter = new highlighter(ui->clamdLogPlainTextEdit->document());
         }
         monochrome = false;
         ui->clamonaccLabel->setStyleSheet("background-color:#c0c0c0;color:black;padding:3px");
         ui->clamdLabel_3->setStyleSheet("background-color:#c0c0c0;color:black;padding:3px");
    }
    checkClamdRunning();
}


void clamdManager::slot_pidWatcherTriggered(){
    QFile pidFile("/tmp/clamd.pid");
    if ((pidFile.exists() == false) && (clamdRestartInProgress == false)){
        clamdPidWatcher->removePath("/tmp/clamd.pid");
        ui->startStopClamdPushButton->setStyleSheet(selectColor("red"));
        ui->startStopClamdPushButton->setText(tr("  Clamd not running - Start Clamd"));
        ui->startStopClamdPushButton->setIcon(QIcon(":/icons/icons/startclamd.png"));
        ui->startStopClamdPushButton->setEnabled(true);
        ui->monitoringAddButton->setEnabled(true);
        ui->monitoringDelButton->setEnabled(true);

        setupFile->setSectionValue("Clamd","ClamdPid","n/a");
        setupFile->setSectionValue("Clamd","ClamonaccPid","n/a");
        if (dirsUnderMonitoring > 0) setupFile->setSectionValue("Clamd","Status2","not Running");
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
            dirsUnderMonitoring++;
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
            dirsUnderMonitoring--;
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
    clamonaccOptions = " --copy=" + QDir::homePath() + "/.clamav-gui/quarantine";

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
        if (dirsUnderMonitoring > 0) {
            stream << "#!/bin/bash\n" << "kill -sigterm " + pid + " && kill -9 " + clamonaccPid + " && sleep 20 && " + clamdLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf && " + clamonaccLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf -l " + QDir::homePath() + "/.clamav-gui/clamd.log" + clamonaccOptions;
        } else {
            stream << "#!/bin/bash\n" << "kill -sigterm " + pid + " && sleep 20 && " + clamdLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf";
        }
        startclamdFile.close();
        startclamdFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup);
    }
    parameters << QDir::homePath() + "/.clamav-gui/startclamd.sh";

    if (sudoGUI == "") sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");

    startClamdProcess->start(sudoGUI,parameters);

    setupFile->setSectionValue("Clamd","Status","starting up ...");
    if (dirsUnderMonitoring > 0) setupFile->setSectionValue("Clamd","Status2","starting up ...");
    setupFile->setSectionValue("Clamd","ClamdPid","n/a");
    setupFile->setSectionValue("Clamd","ClamonaccPid","n/a");

    emit systemStatusChanged();
}

void clamdManager::slot_clamdSettingsChanged(){
    if (initprocessrunning == false) {
        if (checkClamdRunning() == true) {
            ui->restartClamdPushButton->setVisible(true);
            ui->restartClamdPushButton->setStyleSheet(selectColor("green"));
        }
    }
}

void clamdManager::restartClamonacc(){
    QStringList parameters;

    QFile restartclamdFile(QDir::homePath() + "/.clamav-gui/restartclamd.sh");
    restartclamdFile.remove();

    if (restartclamdFile.open(QIODevice::Text|QIODevice::ReadWrite)){
        QTextStream stream(&restartclamdFile);
        if (clamonaccPid != "") {
            if (dirsUnderMonitoring > 0) {
                stream << "#!/bin/bash\n/bin/kill -9 " + clamonaccPid + " && " + clamonaccLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf -l " + QDir::homePath() + "/.clamav-gui/clamd.log";
            } else {
                stream << "#!/bin/bash\n/bin/kill -9 " + clamonaccPid;
            }
        } else {
            if (dirsUnderMonitoring > 0) {
                stream << "#!/bin/bash\n" + clamonaccLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf -l " + QDir::homePath() + "/.clamav-gui/clamd.log";
            }
        }
        restartclamdFile.close();
        restartclamdFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup);
    }

    parameters << QDir::homePath() + "/.clamav-gui/restartclamd.sh";

    if (sudoGUI == "") sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");

    startClamdProcess->start(sudoGUI,parameters);

    setupFile->setSectionValue("Clamd","Status","shutting down ...");

    emit systemStatusChanged();
}

bool clamdManager::checkClamdRunning(){
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
        ui->startStopClamdPushButton->setStyleSheet(selectColor("green"));
        pidFile.close();

        emit systemStatusChanged();
    } else {
        setupFile->setSectionValue("Clamd","ClamdPid","n/a");
        setupFile->setSectionValue("Clamd","Status","not running");
        ui->startStopClamdPushButton->setStyleSheet(selectColor("red"));
        emit systemStatusChanged();
    }

    return rc;
}

QString clamdManager::selectColor(QString color)
{
    QString rc = "";

    if (monochrome == true) {
        rc = css_mono;
    } else {
        if (color == "mono") rc = css_mono;
        if (color == "red") rc = css_red;
        if (color == "green") rc = css_green;
    }

    return rc;
}

void clamdManager::initClamdConfElements(){

    if (setupFile->getSectionBoolValue("Setup","DisableLogHighlighter") == false) {
      logHighlighter = new highlighter(ui->clamdLogPlainTextEdit->document());
      monochrome = false;
      ui->startStopClamdPushButton->setStyleSheet(selectColor("red"));
      ui->clamonaccLabel->setStyleSheet("background-color:#c0c0c0;color:black;padding:3px");
      ui->clamdLabel_3->setStyleSheet("background-color:#c0c0c0;color:black;padding:3px");
    } else {
      monochrome = true;
      ui->startStopClamdPushButton->setStyleSheet(selectColor("mono"));
      ui->clamonaccLabel->setStyleSheet("padding:3px");
      ui->clamdLabel_3->setStyleSheet("padding:3px");
    }

    QStringList clamdConfElements;
    //    clamdConfElements << "2OnAccessIncludePath STRING|This option specifies a directory (including all files and directories inside it), which should be scanned on access. This option can be used multiple times. Default: disabled|";
    //    clamdConfElements << "2OnAccessExcludePath STRING|This option allows excluding directories from on-access scanning. It can be used multiple times. Default: disabled|";
    clamdConfElements << "2OnAccessExcludeRootUID BOOL|With this option you can exclude the root UID (0). Processes run under root will be able to access all files without triggering scans or permission denied events. Note that if clamd cannot check the uid of the process that generated an on-access scan event (e.g., because OnAccessPrevention was not enabled, and the process already exited), clamd will perform a scan.   Thus,  setting OnAccessExcludeRootUID is not guaranteed to prevent every access by the root user from triggering a scan (unless OnAccessPrevention is enabled). Default: no|yes,no,no";
    //    clamdConfElements << "2OnAccessExcludeUID NUMBER|With this option you can exclude specific UIDs. Processes with these UIDs will be able to access all files without triggering scans or permission denied events. This option can be used multiple times (one per line). Note: using a value of 0 on any line will disable this option entirely. To exclude the root UID (0) please enable the OnAccessExcludeRootUID option. Also note that if clamd cannot check the uid of the process that generated an on-access scan event (e.g., because OnAccessPrevention was not enabled, and the process already exited), clamd will perform a scan.  Thus, set- ting OnAccessExcludeUID is not guaranteed to prevent every access by the specified uid from triggering a scan (unless OnAccessPrevention is enabled). Default: disabled|0,1024,0";
    //    clamdConfElements << "2OnAccessExcludeUname STRING|This option allows exclusions via user names when using the on-access scanning client. It can be used multiple times, and has the same potential race condition limitations of the OnAccessExcludeUID option. Default: disabled|";
    clamdConfElements << "2OnAccessMaxFileSize SIZE|Files larger than this value will not be scanned in on access. Default: 5M|0,10485760,5242880";
    clamdConfElements << "2OnAccessMaxThreads NUMBER|Max number of scanning threads to allocate to the OnAccess thread pool at startup. These threads are the ones responsible for creating a connection with the daemon and kicking off scanning after an  event  has  been  pro- cessed. To prevent clamonacc from consuming all clamd's resources keep this lower than clamd's max threads. Default: 5|0,10,5";
    clamdConfElements << "2OnAccessCurlTimeout NUMBER|Max amount of time (in milliseconds) that the OnAccess client should spend for every connect, send, and receive attempt when communicating with clamd via curl. Default: 5000 (5 seconds)|0,10000,5000";
    clamdConfElements << "2OnAccessMountPath STRING|Specifies a mount point (including all files and directories under it), which should be scanned on access. This option can be used multiple times. Default: disabled|";
    clamdConfElements << "2OnAccessDisableDDD BOOL|Disables the dynamic directory determination system which allows for recursively watching include paths. Default: no|yes,no,no";
    clamdConfElements << "2OnAccessPrevention BOOL|Enables fanotify blocking when malicious files are found. Default: disabled|yes,no,no";
    clamdConfElements << "2OnAccessRetryAttempts NUMBER|Number of times the OnAccess client will retry a failed scan due to connection problems (or other issues). Default: 0|0,5,0";
    clamdConfElements << "2OnAccessDenyOnError BOOL|When  using  prevention,  if this option is turned on, any errors that occur during  scanning will result in the event attempt being denied. This could potentially lead to unwanted system behaviour with certain configurations, so the client defaults this to off and prefers allowing access events in case of scan or connection error. Default: no|yes,no,no";
    clamdConfElements << "2OnAccessExtraScanning BOOL|Toggles extra scanning and notifications when a file or directory is created or moved. Requires the  DDD system to kick-off extra scans. Default: no|yes,no,no";
//    clamdConfElements << "1LogFile STRING|Save all reports to a log file. Default: disabled|disabled";
    clamdConfElements << "1LogFileUnlock BOOL|By default the log file is locked for writing and only a single daemon process can write to it. This option disables the lock. Default: no|yes,no,no";
    clamdConfElements << "1LogFileMaxSize SIZE|Maximum size of the log file. Value of 0 disables the limit. Default: 1048576|0,10485760,1048576";
    clamdConfElements << "1LogTime BOOL|Log time for each message. Default: no|yes,no,no";
    clamdConfElements << "1LogClean BOOL|Log all clean files. Useful in debugging but drastically increases the log size. Default: no|yes,no,no";
    clamdConfElements << "1LogSyslog BOOL|Use the system logger (can work together with LogFile). Default: no|yes,no,no";
    clamdConfElements << "1LogFacility STRING|Type of syslog messages. Please refer to 'man syslog' for facility names. (LOG_LOCAL6, LOG_MAIL), Default: LOG_LOCAL6|LOG_LOCAL6,LOG_MAIL,LOG_LOCAL6";
    clamdConfElements << "1LogVerbose BOOL|Enable verbose logging. Default: no|yes,no,no";
    clamdConfElements << "1LogRotate BOOL|Rotate log file. Requires LogFileMaxSize option set prior to this option. Default: no|yes,no,no";
    clamdConfElements << "1ExtendedDetectionInfo BOOL|Log additional information about the infected file, such as its size and hash, together with the virus name. Default: no|yes,no,no";
//    clamdConfElements << "1PidFile STRING|Write the daemon's pid to the specified file. Default: disabled|yes,no,no";
    clamdConfElements << "1TemporaryDirectory STRING|This option allows you to change the default temporary directory. Default: /tmp|/tmp";
    clamdConfElements << "1DatabaseDirectory STRING|This option allows you to change the default database directory. If you enable it, please make sure it points to the same directory in both clamd and freshclam. Default: /usr/local/share/clamav|/usr/local/share/clamav";
    clamdConfElements << "1OfficialDatabaseOnly BOOL|Only load the official signatures published by the ClamAV project. Default: no|yes,no,no";
    clamdConfElements << "1FailIfCvdOlderThan NUMBER|Return with a nonzero error code if the virus database is older than the specified number of days. Default: -1|-1,30,-1";
//    clamdConfElements << "1LocalSocket STRING|Path to a local (Unix) socket the daemon will listen on. Default: disabled|";
//    clamdConfElements << "1LocalSocketGroup STRING|Sets the group ownership on the unix socket. Default: |";
    clamdConfElements << "1LocalSocketMode STRING|Sets the permissions on the unix socket to the specified mode. Default: 660|660";
    clamdConfElements << "1FixStaleSocket BOOL|Remove stale socket after unclean shutdown. Default: yes|yes,no,yes";
    clamdConfElements << "1TCPSocket NUMBER|TCP port number the daemon will listen on. Default: disabled|0,65535,0";
    clamdConfElements << "1TCPAddr STRING|By default clamd binds to INADDR_ANY. This option allows you to restrict the TCP address and provide some degree of protection from the outside world. This option can be specified multiple times in order to listen on multiple IPs. IPv6 is now supported. Default: disabled|";
    clamdConfElements << "1MaxConnectionQueueLength NUMBER|Maximum length the queue of pending connections may grow to. Default: 200|0,500,200";
    clamdConfElements << "1StreamMaxLength SIZE|Close the STREAM session when the data size limit is exceeded. The value should match your MTA's limit for the maximum attachment size. Default: 100M|0,1048576000,104857600";
    clamdConfElements << "1StreamMinPort NUMBER|The STREAM command uses an FTP-like protocol. This option sets the lower boundary for the port range. Default: 1024|0,4096,1024";
    clamdConfElements << "1StreamMaxPort NUMBER|This option sets the upper boundary for the port range. Default: 2048|0,4096,2048";
    clamdConfElements << "1MaxThreads NUMBER|Maximum number of threads running at the same time. Default: 10|0,20,10";
    clamdConfElements << "1ReadTimeout NUMBER|This option specifies the time (in seconds) after which clamd should timeout if a client doesn't provide any data. Default: 120|0,300,120";
    clamdConfElements << "1CommandReadTimeout NUMBER|This option specifies the time (in seconds) after which clamd should timeout if a client doesn't provide any initial command after connecting.  The default is set to 30 to avoid timeouts with TCP sockets  when  processing large messages.  If using a Unix socket, the value can be changed to 5.  Note: the timeout for subsequents commands, and/or data chunks is specified by ReadTimeout. Default: 30|0,60,30";
    clamdConfElements << "1SendBufTimeout NUMBER|This option specifies how long to wait (in milliseconds) if the send buffer is full.  Keep this value low to prevent clamd hanging. Default: 500|0,1000,500";
    clamdConfElements << "1MaxQueue NUMBER|Maximum number of queued items (including those being processed by MaxThreads threads).  It is recommended to have this value at least twice MaxThreads if possible. WARNING: you shouldn't increase this too much to avoid running out of file descriptors, the following condition should hold: MaxThreads*MaxRecursion + MaxQueue - MaxThreads + 6 < RLIMIT_NOFILE.  RLIMIT_NOFILE is the maximum number of open file descriptors (usually 1024), set by ulimit -n. Default: 100|0,2048,100";
    clamdConfElements << "1IdleTimeout NUMBER|This option specifies how long (in seconds) the process should wait for a new job. Default: 30|0,60,30";
//    clamdConfElements << "2ExcludePath REGEX|Don't scan files and directories matching REGEX. This directive can be used multiple times. Default: disabled|";
    clamdConfElements << "1MaxDirectoryRecursion NUMBER|Maximum depth directories are scanned at. Default: 15|0,30,15";
    clamdConfElements << "1FollowDirectorySymlinks BOOL|Follow directory symlinks. Default: no|yes,no,no";
    clamdConfElements << "1CrossFilesystems BOOL|Scan files and directories on other filesystems. Default: yes|yes,no,yes";
    clamdConfElements << "1FollowFileSymlinks BOOL|Follow regular file symlinks. Default: no|yes,no,no";
    clamdConfElements << "1SelfCheck NUMBER|This option specifies the time intervals (in seconds) in which clamd should perform a database check. Default: 600|0,1200,600";
    clamdConfElements << "1ConcurrentDatabaseReload BOOL|Enable non-blocking (multi-threaded/concurrent) database reloads. This feature will temporarily load a second scanning engine while scanning continues using the first engine. Once loaded, the new engine  takes  over.  The old  engine  is removed as soon as all scans using the old engine have completed. This feature requires more RAM, so this option is provided in case users are willing to block scans during reload in exchange for lower RAM requirements. Default: yes|yes,no,yes";
    clamdConfElements << "1VirusEvent COMMAND|Execute a command when virus is found.  Use the following environment variables to identify the file and virus names: - $CLAM_VIRUSEVENT_FILENAME - $CLAM_VIRUSEVENT_VIRUSNAME In the command string, '%v' will also  be  replaced  with  the  virus name.  Note: The '%f' filename format character has been disabled and will no longer be replaced with the file name, due to command injection security concerns.  Use the 'CLAM_VIRUSEVENT_FILENAME' environment variable instead.  For the same reason, you should NOT use the environment variables in the command directly, but should use it carefully from your executed script. Default: disabled|";
    clamdConfElements << "1ExitOnOOM BOOL|Stop daemon when libclamav reports out of memory condition. Default: no|yes,no,no";
    clamdConfElements << "1AllowAllMatchScan BOOL|Permit use of the ALLMATCHSCAN command. Default: yes|yes,no,yes";
    clamdConfElements << "1Foreground BOOL|Don't fork into background. Default: no|yes,no,no";
    clamdConfElements << "1Debug BOOL|Enable debug messages from libclamav. Default: no|yes,no,no";
    clamdConfElements << "1LeaveTemporaryFiles BOOL|Do not remove temporary files (for debugging purpose). Default: no GenerateMetadataJson BOOL Record metadata about the file being scanned.  Scan metadata is useful for file analysis purposes and for debugging scan behavior.  The JSON metadata will be printed after the scan is complete if Debug is enabled. A metadata.json file will be written to the scan temp directory if LeaveTemporaryFiles is enabled. Default: no|yes,no,no";
    clamdConfElements << "1User STRING|Run the daemon as a specified user (the process must be started by root). Default: disabled|root";
    clamdConfElements << "1Bytecode BOOL|With this option enabled ClamAV will load bytecode from the database. It is highly recommended you keep this option turned on, otherwise you may miss detections for many new viruses. Default: yes|yes,no,yes";
    clamdConfElements << "1BytecodeSecurity STRING|Set bytecode security level. Possible values: TrustSigned - trust bytecode loaded from signed .c[lv]d files and insert runtime safety checks for bytecode loaded from other sources, Paranoid - don't trust any bytecode, insert runtime checks for all. Recommended: TrustSigned, because bytecode in .cvd files already has these checks. (TrustSigned, Paranoid) Default: TrustSigned|TrustSigned,Paranoid,TrustSigned";
    clamdConfElements << "1BytecodeTimeout NUMBER|Set bytecode timeout in milliseconds. Default: 10000|0,100000,10000";
    clamdConfElements << "1BytecodeUnsigned BOOL|Allow loading bytecode from outside digitally signed .c[lv]d files.  **Caution**: You should NEVER run bytecode signatures from untrusted sources.  Doing so may result in arbitrary code execution. Default: no|yes,no,no";
    clamdConfElements << "1BytecodeMode STRING|Set bytecode execution mode. Possible values: Auto - automatically choose JIT if possible, fallback to interpreter ForceJIT - always choose JIT, fail if not possible ForceInterpreter - always choose interpreter Test - run with both JIT and interpreter and compare results. Make all failures fatal. Default: Auto|Auto,ForceJIT,ForceInterpreter,Test,Auto";
    clamdConfElements << "1DetectPUA BOOL|Detect Possibly Unwanted Applications. Default: no|yes,no,no";
//    clamdConfElements << "1ExcludePUA CATEGORY|Exclude a specific PUA category. This directive can be used multiple times. See https://docs.clamav.net/faq/faq-pua.html for the complete list of PUA categories. Default: disabled|";
//    clamdConfElements << "1IncludePUA CATEGORY|Only include a specific PUA category. This directive can be used multiple times. See https://docs.clamav.net/faq/faq-pua.html for the complete list of PUA categories. Default: disabled|";
    clamdConfElements << "1HeuristicAlerts BOOL|In some cases (eg. complex malware, exploits in graphic files, and others), ClamAV uses special algorithms to provide accurate detection. This option controls the algorithmic detection. Default: yes|yes,no,yes";
    clamdConfElements << "1HeuristicScanPrecedence BOOL|Allow  heuristic  match  to  take  precedence. When enabled, if a heuristic scan (such as phishingScan) detects a possible virus/phishing it will stop scanning immediately. Recommended, saves CPU scan-time. When disabled, virus/phishing detected by heuristic scans will be reported only at the end of a scan. If an archive contains both a heuristically detected virus/phishing, and a real malware, the real malware will be reported. Keep  this disabled  if  you intend to handle \"*.Heuristics.*\" viruses  differently from \"real\" malware. If a non-heuristically-detected virus (signature-based) is found first, the scan is interrupted immediately, regardless of this config option. Default: no|yes,no,no";
    clamdConfElements << "1ScanPE BOOL|PE stands for Portable Executable - it's an executable file format used in all 32 and 64-bit versions of Windows operating systems. This option allows ClamAV to perform a deeper analysis of executable files and it's  also required for decompression of popular executable packers such as UPX. If you turn off this option, the original files will still be scanned, but without additional processing. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanELF BOOL|Executable and Linking Format is a standard format for UN*X executables. This option allows you to control the scanning of ELF files. If you turn off this option, the original files will still be scanned, but without additional processing. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanMail BOOL|Enable scanning of mail files. If you turn off this option, the original files will still be scanned, but without parsing individual messages/attachments. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanPartialMessages BOOL|Scan RFC1341 messages split over many emails. You will need to periodically clean up $TemporaryDirectory/clamav-partial directory. WARNING: This option may open your system to a DoS attack. Never use it on loaded servers. Default: no|yes,no,no";
    clamdConfElements << "1PhishingSignatures BOOL|Enable email signature-based phishing detection. Default: yes|yes,no,yes";
    clamdConfElements << "1PhishingScanURLs BOOL|Enable URL signature-based phishing detection (Heuristics.Phishing.Email.*) Default: yes|yes,no,yes";
    clamdConfElements << "1StructuredDataDetection BOOL|Enable the DLP module. Default: no|yes,no,no";
    clamdConfElements << "1StructuredMinCreditCardCount NUMBER|This option sets the lowest number of Credit Card numbers found in a file to generate a detect. Default: 3|0,9,3";
    clamdConfElements << "1StructuredCCOnly BOOL|With this option enabled the DLP module will search for valid Credit Card0umbers only. Debit and Private Label cards will not be searched. Default: no|yes,no,no";
    clamdConfElements << "1StructuredMinSSNCount NUMBER|This option sets the lowest number of Social Security Numbers found in a file to generate a detect. Default: 3|0,9,3";
    clamdConfElements << "1StructuredSSNFormatNormal BOOL|With this option enabled the DLP module will search for valid SSNs formatted as xxx-yy-zzzz. Default: yes|yes,no,yes";
    clamdConfElements << "1StructuredSSNFormatStripped BOOL|With this option enabled the DLP module will search for valid SSNs formatted as xxxyyzzzz. Default: no|yes,no,no";
    clamdConfElements << "1ScanHTML BOOL|Perform HTML/JavaScript/ScriptEncoder normalisation and decryption. If you turn off this option, the original files will still be scanned, but without additional processing. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanOLE2 BOOL|This option enables scanning of OLE2 files, such as Microsoft Office documents and .msi files. If you turn off this option, the original files will still be scanned, but without additional processing. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanPDF BOOL|This option enables scanning within PDF files. If you turn off this option, the original files will still be scanned, but without additional processing. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanSWF BOOL|This option enables scanning within SWF files. If you turn off this option, the original files will still be scanned, but without decoding and additional processing. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanXMLDOCS BOOL|This option enables scanning xml-based document files supported by libclamav. If you turn off this option, the original files will still be scanned, but without additional processing. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanHWP3 BOOL|This option enables scanning HWP3 files. If you turn off this option, the original files will still be scanned, but without additional processing. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanOneNote BOOL|This option enables scanning OneNote files. If you turn off this option, the original files will still be scanned, but without additional processing. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanArchive BOOL|Scan within archives and compressed files. If you turn off this option, the original files will still be scanned, but without unpacking and additional processing. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanImage BOOL|This option enables scanning of image (graphics). If you turn off this option, the original files will still be scanned, but without unpacking and additional processing. Default: yes|yes,no,yes";
    clamdConfElements << "1ScanImageFuzzyHash BOOL|This option enables detection by calculating a fuzzy hash of image (graphics) files. Signatures using image fuzzy hashes typically match files and documents by identifying images embedded or attached to those files. If you turn off this option, then some files may no longer be detected. Default: yes|yes,no,yes";
    clamdConfElements << "1AlertBrokenExecutables BOOL|Alert on broken executable files (PE & ELF). Default: no|yes,no,no";
    clamdConfElements << "1AlertBrokenMedia BOOL|Alert on broken graphics files (JPEG, TIFF, PNG, GIF). Default: no|yes,no,no";
    clamdConfElements << "1AlertEncrypted BOOL|Alert on encrypted archives and documents (encrypted .zip, .7zip, .rar, .pdf). Default: no|yes,no,no";
    clamdConfElements << "1AlertEncryptedArchive BOOL|Alert on encrypted archives (encrypted .zip, .7zip, .rar). Default: no|yes,no,no";
    clamdConfElements << "1AlertEncryptedDoc BOOL|Alert on encrypted documents (encrypted .pdf). Default: no|yes,no,no";
    clamdConfElements << "1AlertOLE2Macros BOOL|Alert on OLE2 files containing VBA macros (Heuristics.OLE2.ContainsMacros). Default: no|yes,no,no";
    clamdConfElements << "1AlertExceedsMax BOOL|When AlertExceedsMax is set, files exceeding the MaxFileSize, MaxScanSize, or MaxRecursion limit will be flagged with the virus name starting with \"Heuristics.Limits.Exceeded\". Default: no|yes,no,no";
    clamdConfElements << "1AlertPhishingSSLMismatch BOOL|Alert on emails containing SSL mismatches in URLs (might lead to false positives!). Default: no|yes,no,no";
    clamdConfElements << "1AlertPhishingCloak BOOL|Alert on emails containing cloaked URLs (might lead to some false positives). Default: no|yes,no,no";
    clamdConfElements << "1AlertPartitionIntersection BOOL|Alert on raw DMG image files containing partition intersections. Default: no|yes,no,no";
    clamdConfElements << "1DisableCache BOOL|This option allows you to disable the caching feature of the engine. By default, the engine will store an MD5 in a cache of any files that are not flagged as virus or that hit limits checks. Warning: Disabling the cache will have a negative performance impact on large scans. Default: no|yes,no,no";
    clamdConfElements << "1CacheSize NUMBER|This option allows you to set the number of entries the cache can store. The value should be a square number or will be rounded up to the nearest square number. Default: 65536|0,655360,65536";
    clamdConfElements << "1ForceToDisk BOOL|This option causes memory or nested map scans to dump the content to disk. If you turn on this option, more data is written to disk and is available when the leave-temps option is enabled at the cost of more disk writes. Default: no|yes,no,no";
    clamdConfElements << "1MaxScanTime SIZE|This  option  sets the maximum amount of time a scan may take to complete. The value is in milliseconds. The value of 0 disables the limit. WARNING: disabling this limit or setting it too high may result allow scanning of certain files to lock up the scanning process/threads resulting in a Denial of Service. Default: 120000|0,240000,120000";
    clamdConfElements << "1MaxScanSize SIZE|Sets the maximum amount of data to be scanned for each input file. Archives and other containers are recursively extracted and scanned up to this value. The size of an archive plus the sum of the sizes of all files within archive count toward the scan size. For example, a 1M uncompressed archive containing a single 1M inner file counts as 2M toward the max scan size. Warning: disabling this limit or setting it too high may result in severe damage to the system. Default: 400M|0,1048576000,419430400";
    clamdConfElements << "1MaxFileSize SIZE|Files larger than this limit won't be scanned. Affects the input file itself as well as files contained inside it (when the input file is an archive, a document or some other kind of container).  Warning:  disabling  this limit or setting it too high may result in severe damage to the system. Technical design limitations prevent ClamAV from scanning files greater than 2 GB at this time. Default: 100M|0,1048576000,104857600";
    clamdConfElements << "1MaxRecursion NUMBER|Nested  archives are scanned recursively, e.g. if a Zip archive contains a RAR file, all files within it will also be scanned. This options specifies how deeply the process should be continued. Warning: setting this limit too high may result in severe damage to the system. Default: 17|0,34,17";
    clamdConfElements << "1MaxFiles NUMBER|Number of files to be scanned within an archive, a document, or any other kind of container. Warning: disabling this limit or setting it too high may result in severe damage to the system. Default: 10000|0,20000,10000";
    clamdConfElements << "1MaxEmbeddedPE SIZE|This option sets the maximum size of a file to check for embedded PE. Files larger than this value will skip the additional analysis step. Negative values are not allowed. Default: 40M|0,41943040,104857600";
    clamdConfElements << "1MaxHTMLNormalize SIZE|This option sets the maximum size of a HTML file to normalize. HTML files larger than this value will not be normalized or scanned. Negative values are not allowed. Default: 40M|0,41943040,104857600";
    clamdConfElements << "1MaxHTMLNoTags SIZE|This option sets the maximum size of a normalized HTML file to scan. HTML files larger than this value after normalization will not be scanned. Negative values are not allowed. Default: 8M|0,16777216,8388608";
    clamdConfElements << "1MaxScriptNormalize SIZE|This option sets the maximum size of a script file to normalize. Script content larger than this value will not be normalized or scanned. Negative values are not allowed. Default: 20M|0,41943040,20971520";
    clamdConfElements << "1MaxZipTypeRcg SIZE|This option sets the maximum size of a ZIP file to reanalyze type recognition. ZIP files larger than this value will skip the step to potentially reanalyze as PE. Negative values are not allowed. WARNING: setting this limit too high may result in severe damage or impact performance. Default: 1M|0,10485760,1048576";
    clamdConfElements << "1MaxPartitions SIZE|This option sets the maximum number of partitions of a raw disk image to be scanned. Raw disk images with more partitions than this value will have up to the value partitions scanned. Negative values are not allowed. WARNING: setting this limit too high may result in severe damage or impact performance. Default: 50|0,70,50";
    clamdConfElements << "1MaxIconsPE SIZE|This option sets the maximum number of icons within a PE to be scanned. PE files with more icons than this value will have up to the value number icons scanned. Negative values are not allowed. WARNING: setting this limit too high may result in severe damage or impact performance.|0,10485760,1048576";
    clamdConfElements << "1MaxRecHWP3 NUMBER|This option sets the maximum recursive calls to HWP3 parsing function. HWP3 files using more than this limit will be terminated and alert the user. Scans will be unable to scan any HWP3 attachments if the recursive limit is reached. Negative values are not allowed. WARNING: setting this limit too high may result in severe damage or impact performance.|0,30,15";
    clamdConfElements << "1PCREMatchLimit NUMBER|This option sets the maximum calls to the PCRE match function during an instance of regex matching. Instances using more than this limit will be terminated and alert the user but the scan will continue. For more information on match_limit, see the PCRE documentation. Negative values are not allowed. WARNING: setting this limit too high may severely impact performance. Default: 10000|0,100000,10000";
    clamdConfElements << "1PCRERecMatchLimit NUMBER|This option sets the maximum recursive calls to the PCRE match function during an instance of regex matching. Instances using more than this limit will be terminated and alert the user but the scan will continue. For more information on match_limit_recursion, see the PCRE documentation. Negative values are not allowed and values > PCREMatchLimit are superfluous. WARNING: setting this limit too high may severely impact performance. Default: 2000|0,4000,2000";
    clamdConfElements << "1PCREMaxFileSize SIZE|This option sets the maximum filesize for which PCRE subsigs will be executed. Files exceeding this limit will not have PCRE subsigs executed unless a subsig is encompassed to a smaller buffer. Negative values are not allowed. Setting this value to zero disables the limit. WARNING: setting this limit too high or disabling it may severely impact performance. Default: 100M|0,1048576000,104857600";
    clamdConfElements << "1DisableCertCheck BOOL|Disable authenticode certificate chain verification in PE files. Default: no|yes,no,no";

// STRING, CATEGORY, COMMAND, REGEX ----> checkbox + lineedit
// NUMBER, SIZE ----> checkbox + spinbox
// BOOL ----> checkbox + combobox [yes,no,....] "scanoption"

    QString keyword;
    QString keywordHelper;
    QString label;
    QString optionValues;
    QString element;
    QString group;
    QString container;
    bool checked = false;
    clamdConfStringOption * stringOption;
    clamdconfspinboxoption* spinboxOption;
    clamdconfcomboboxoption * comboboxOption;

    for (int i = 0; i < clamdConfElements.length(); i++) {
        element = clamdConfElements.at(i);

        QStringList values = element.split("|");
        keywordHelper = values.at(0);
        label = values.at(1);
        optionValues = values.at(2);

        QStringList splitter = keywordHelper.split(" ");
        keyword = splitter.at(0);
        container = keyword.left(1);
        keyword = keyword.mid(1);
        group = splitter.at(1);

        if (optionValues == "disabled") optionValues = "";

        if ((group == "STRING") || (group == "CATEGORY") || (group == "COMMAND") || (group == "REGEX")){
            if (optionValues.indexOf(",") == -1) {
                stringOption = new clamdConfStringOption(this,keyword,checked,label,optionValues);
                connect(stringOption,SIGNAL(settingChanged()),this,SLOT(slot_clamdSettingsChanged()));
                container != "2"?ui->layout1->addWidget(stringOption):ui->layout2->addWidget(stringOption);
            } else {
                comboboxOption = new clamdconfcomboboxoption(this,keyword,checked,label,optionValues);
                connect(comboboxOption,SIGNAL(settingChanged()),this,SLOT(slot_clamdSettingsChanged()));
                container != "2"?ui->layout1->addWidget(comboboxOption):ui->layout2->addWidget(comboboxOption);
            }
        }

        if ((group == "NUMBER") || (group == "SIZE")) {
            spinboxOption = new clamdconfspinboxoption(this,keyword,checked,label,optionValues);
            connect(spinboxOption,SIGNAL(settingChanged()),this,SLOT(slot_clamdSettingsChanged()));
            container != "2"?ui->layout1->addWidget(spinboxOption):ui->layout2->addWidget(spinboxOption);
        }

        if (group == "BOOL") {
            comboboxOption = new clamdconfcomboboxoption(this,keyword,checked,label,optionValues);
            connect(comboboxOption,SIGNAL(settingChanged()),this,SLOT(slot_clamdSettingsChanged()));
            container != "2"?ui->layout1->addWidget(comboboxOption):ui->layout2->addWidget(comboboxOption);
        }
    }
}
