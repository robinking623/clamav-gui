#include "clamav_gui.h"
#include "ui_clamav_gui.h"

clamav_gui::clamav_gui(QWidget *parent) : QWidget(parent), ui(new Ui::clamav_gui){
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    QString m_path = QDir::homePath() + "/.clamav-gui/settings.ini";
    bool m_createDefaultSettings = false;
    error = false;
    guisudoapp = "pkexec";
//*****************************************************************************
//creating service Menu
//*****************************************************************************
    QDir m_tempDir;
    QString m_serviceMenuPath = "";
    if (m_tempDir.exists(QDir::homePath() + "/.local/share/kservices5/ServiceMenus") == true) m_serviceMenuPath = QDir::homePath() + "/.local/share/kservices5/ServiceMenus";
    if ((m_serviceMenuPath == "") && (m_tempDir.exists(QDir::homePath() + "/.local/share/kio/servicemenus") == true)) m_serviceMenuPath = QDir::homePath() + "/.local/share/kio/servicemenus";
    if (m_serviceMenuPath == "") m_serviceMenuPath = m_serviceMenuPath = QDir::homePath() + "/.local/share/kservices5/ServiceMenus";
    if (m_tempDir.exists(m_serviceMenuPath + "/scanWithClamAV-GUI.desktop") == false) {
        if (m_tempDir.exists(m_serviceMenuPath) == false) m_tempDir.mkdir(m_serviceMenuPath);
        setupFileHandler * serviceFile = new setupFileHandler(m_serviceMenuPath + "/scanWithClamAV-GUI.desktop",this);
        serviceFile->setSectionValue("Desktop Entry","Type","Service");
        serviceFile->setSectionValue("Desktop Entry","ServiceTypes","KonqPopupMenu/Plugin");
        serviceFile->setSectionValue("Desktop Entry","MimeType","all/all;");
        serviceFile->setSectionValue("Desktop Entry","Actions","scan;");
        serviceFile->setSectionValue("Desktop Entry","Icon","clamav-gui");
        serviceFile->setSectionValue("Desktop Entry","X-KDE-Priority","TopLevel");
        serviceFile->setSectionValue("Desktop Entry","X-KDE-StartupNotify","false");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu","Scan with ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu[de]","Scannen mit ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu[da_DK]","Scannen med ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu[es_ES]","Analizar con ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu[us]","Scan with ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu[gb]","Scan with ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu[pt]","Investigar com ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu[br]","Investigar com ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu[pt_BR]","Investigar com ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu[fr]","Scanner avec ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu[it]","Scansione con ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry","NO-X-KDE-Submenu[uk]","Сканування за допомогою ClamAV-GUI");

        serviceFile->setSectionValue("Desktop Action scan","Name","scan");
        serviceFile->setSectionValue("Desktop Action scan","Name[de]","Scannen mit ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan","Name[es_ES]","Analizar con ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan","Name[us]","Scan with ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan","Name[gb]","Scan with ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan","Name[pt]","Investigar com ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan","Name[br]","Investigar com ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan","Name[fr]","Scanner avec ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan","Name[it]","Scansione con ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan","Name[uk]","Сканування за допомогою ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan","Icon","clamav-gui");
        serviceFile->setSectionValue("Desktop Action scan","Exec","clamav-gui --scan %F");
    }
//*****************************************************************************

//*****************************************************************************
// Creating default direcotry structur in HOME
//*****************************************************************************
    if (m_tempDir.exists(QDir::homePath() + "/.clamav-gui") == false){
        m_tempDir.mkdir(QDir::homePath() + "/.clamav-gui");
        m_createDefaultSettings = true;
    }
    if (m_tempDir.exists(QDir::homePath() + "/.clamav-gui/quarantine") == false) m_tempDir.mkdir(QDir::homePath() + "/.clamav-gui/quarantine");
    if (m_tempDir.exists(QDir::homePath() + "/.clamav-gui/signatures") == false) m_tempDir.mkdir(QDir::homePath() + "/.clamav-gui/signatures");
    if (m_tempDir.exists(QDir::homePath() + "/.clamav-gui/profiles") == false) m_tempDir.mkdir(QDir::homePath() + "/.clamav-gui/profiles");
    if (m_tempDir.exists(QDir::homePath() + "/.clamav-gui/logs") == false) m_tempDir.mkdir(QDir::homePath() + "/.clamav-gui/logs");
    QFile::setPermissions(QDir::homePath() + "/.clamav-gui/signatures",QFileDevice::ReadOwner|QFileDevice::ReadUser|QFileDevice::ReadGroup|QFileDevice::ReadOther|QFileDevice::WriteOwner|QFileDevice::WriteUser|QFileDevice::WriteGroup|QFileDevice::WriteOther|QFileDevice::ExeOwner|QFileDevice::ExeUser|QFileDevice::ExeGroup|QFileDevice::ExeOther);
//*****************************************************************************

//_____________________________________________________________________________________________________________________________________
// For UALinux
// If the settings.ini in the home folder of the user is not present a predefined version is been copied into the folder.
//_____________________________________________________________________________________________________________________________________
    QFile m_destChecker;
    QFile m_sourceChecker;
    QDir destDir;
    if (m_destChecker.exists(QDir::homePath() + "/.clamav-gui/settings.ini") == false) {
        destDir.mkdir(QDir::homePath() + "/.clamav-gui");
        if (m_sourceChecker.exists("/etc/clamav-gui/settings.ini") == true) QFile::copy("/etc/clamav-gui/settings.ini",QDir::homePath() + "/.clamav-gui/settings.ini");
    }
//______________________________________________________________________________________________________________________________________

    if (m_createDefaultSettings == true){
        setupFile = new setupFileHandler(m_path,this);
        if ((m_tempDir.exists("/var/lib/clamav") == true) && (m_destChecker.exists("/var/lib/clamav/freshclam.dat") == true)){
            setupFile->setSectionValue("Directories","LoadSupportedDBFiles","m_checked|/var/lib/clamav");
        } else {
            setupFile->setSectionValue("Directories","LoadSupportedDBFiles","m_checked|" + QDir::homePath() + "/.clamav-gui/signatures");
        }
        if (setupFile->keywordExists("Directories","TmpFile") == false) setupFile->setSectionValue("Directories","TmpFile","m_checked|/tmp");
        if (setupFile->keywordExists("Directories","ScanReportToFile") == false) setupFile->setSectionValue("Directories","ScanReportToFile","m_checked|" + QDir::homePath() + "/.clamav-gui/logs/report-scan.log");
        if (setupFile->keywordExists("Directories","MoveInfectedFiles") == false) setupFile->setSectionValue("Directories","MoveInfectedFiles","not m_checked|" + QDir::homePath()+"/.clamav-gui/quarantine");
        if (setupFile->keywordExists("Directories","CopyInfectedFiles") == false) setupFile->setSectionValue("Directories","CopyInfectedFiles","not m_checked|" + QDir::homePath()+"/.clamav-gui/quarantine");
        if (setupFile->keywordExists("Setup","language") == false) setupFile->setSectionValue("Setup","language",2);
    } else {
        setupFile = new setupFileHandler(m_path,this);
    }

    scanProcess = new QProcess(this);
    connect(scanProcess,SIGNAL(readyReadStandardError()),this,SLOT(slot_scanProcessHasErrOutput()));
    connect(scanProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(slot_scanProcessHasStdOutput()));
    connect(scanProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(slot_scanProcessFinished(int,QProcess::ExitStatus)));
    if (setupFile->getSectionBoolValue("Settings","ShowHideDropZone") == true) createDropZone();

    mainWindowTimer = new QTimer(this);
    connect(mainWindowTimer,SIGNAL(timeout()),this,SLOT(slot_mainWinTimerTimeout()));
    mainWindowTimer->setSingleShot(true);
    mainWindowTimer->start(250);

    createTrayIcon();
    trayIcon->setIcon(QIcon(":/icons/extra/icon32/clamav-gui.png"));
    trayIcon->show();
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(slot_systemTrayIconActivated(QSystemTrayIcon::ActivationReason)));

    ui->tabWidget->removeTab(0);

    scannerTab = new scanTab(this);
    connect(scannerTab,SIGNAL(requestDropZoneVisible()),this,SLOT(slot_showDropZone()));
    connect(scannerTab,SIGNAL(triggerScanRequest(QStringList)),this,SLOT(slot_scanRequest(QStringList)));
    connect(scannerTab,SIGNAL(abortScan()),this,SLOT(slot_abortScan()));
    connect(this,SIGNAL(setScannerForm(bool)),scannerTab,SLOT(slot_enableForm(bool)));
    ui->tabWidget->addTab(scannerTab,QIcon(":/icons/icons/Clam.png"),tr("Scan"));

    optionTab = new optionsDialog(this);
    ui->tabWidget->addTab(optionTab,QIcon(":/icons/icons/options.png"),tr("Options"));

    profileManagerTab = new ProfileManager(this);
    ui->tabWidget->addTab(profileManagerTab,QIcon(":/icons/icons/profilemanager.png"),tr("Profile Manager"));

    schedulerTab = new scheduler(this);
    ui->tabWidget->addTab(schedulerTab,QIcon(":/icons/icons/scheduler.png"),tr("Scheduler"));

    logTab = new logViewerObject(this);
    ui->tabWidget->addTab(logTab,QIcon(":/icons/icons/includeexclude.png"),tr("Logs"));

    freshclamTab = new freshclamsetter(this);
    connect(freshclamTab,SIGNAL(quitApplication()),this,SLOT(slot_quitApplication()));
    ui->tabWidget->addTab(freshclamTab,QIcon(":/icons/icons/freshclam.png"),tr("FreshClam"));

    clamdTab = new clamdManager(this);
    ui->tabWidget->addTab(clamdTab,QIcon(":/icons/icons/onaccess.png"),tr("Clamd"));

    setUpTab = new setupTab(this);
    ui->tabWidget->addTab(setUpTab,QIcon(":/icons/icons/setup.png"),tr("Setup"));

    infoTab = new infoDialog(this);
    ui->tabWidget->addTab(infoTab,QIcon(":/icons/icons/information.png"),tr("About"));

    ui->tabWidget->setTabShape(QTabWidget::Rounded);

    connect(freshclamTab,SIGNAL(setBallonMessage(int,QString,QString)),this,SLOT(slot_setTrayIconBalloonMessage(int,QString,QString)));
    connect(freshclamTab,SIGNAL(disableUpdateButtons()),freshclamTab,SLOT(slot_disableUpdateButtons()));
    connect(freshclamTab,SIGNAL(disableUpdateButtons()),scannerTab,SLOT(slot_disableScanButton()));
    connect(freshclamTab,SIGNAL(disableUpdateButtons()),schedulerTab,SLOT(slot_disableScheduler()));
    connect(freshclamTab,SIGNAL(reportError()),this,SLOT(slot_errorReporter()));
    connect(freshclamTab,SIGNAL(updateDatabase()),this,SLOT(slot_updateDatabase()));
    connect(freshclamTab,SIGNAL(freshclamStarted()),clamdTab,SLOT(slot_waitForFreshclamStarted()));
    connect(freshclamTab,SIGNAL(systemStatusChanged()),setUpTab,SLOT(slot_updateSystemInfo()));
    connect(clamdTab,SIGNAL(setBallonMessage(int,QString,QString)),this,SLOT(slot_setTrayIconBalloonMessage(int,QString,QString)));
    connect(clamdTab,SIGNAL(setActiveTab()),this,SLOT(slot_startclamd()));
    connect(clamdTab,SIGNAL(systemStatusChanged()),setUpTab,SLOT(slot_updateSystemInfo()));
    connect(profileManagerTab,SIGNAL(triggerProfilesChanged()),schedulerTab,SLOT(slot_updateProfiles()));
    connect(profileManagerTab,SIGNAL(triggerProfilesChanged()),logTab,SLOT(slot_profilesChanged()));
    connect(schedulerTab,SIGNAL(triggerScanJob(QString,QStringList)),this,SLOT(slot_receiveScanJob(QString,QStringList)));
    connect(schedulerTab,SIGNAL(logChanged()),logTab,SLOT(slot_profilesChanged()));
    connect(optionTab,SIGNAL(databasePathChanged(QString)),freshclamTab,SLOT(slot_dbPathChanged(QString)));
    connect(optionTab,SIGNAL(updateDatabase()),this,SLOT(slot_updateDatabase()));
    connect(optionTab,SIGNAL(updateClamdConf()),clamdTab,SLOT(slot_updateClamdConf()));
    connect(optionTab,SIGNAL(systemStatusChanged()),setUpTab,SLOT(slot_updateSystemInfo()));
    connect(setUpTab,SIGNAL(switchActiveTab(int)),this,SLOT(slot_switchActiveTab(int)));
    connect(setUpTab,SIGNAL(sendSystemInfo(QString)),this,SLOT(slot_receiveVersionInformation(QString)));
    connect(this,SIGNAL(scanJobFinished()),logTab,SLOT(slot_profilesChanged()));
    connect(this,SIGNAL(startDatabaseUpdate()),freshclamTab,SLOT(slot_updateNowButtonClicked()));
    connect(setUpTab,SIGNAL(logHighlightingChanged(bool)),clamdTab,SLOT(slot_add_remove_highlighter(bool)));
    connect(setUpTab,SIGNAL(logHighlightingChanged(bool)),scannerTab,SLOT(slot_add_remove_highlighter(bool)));
    connect(setUpTab,SIGNAL(logHighlightingChanged(bool)),logTab,SLOT(slot_add_remove_highlighter(bool)));
    connect(setUpTab,SIGNAL(logHighlightingChanged(bool)),freshclamTab,SLOT(slot_add_remove_highlighter(bool)));
    connect(setUpTab,SIGNAL(logHighlightingChanged(bool)),profileManagerTab,SLOT(monochromeModeChanged(bool)));

    ui->tabWidget->setCurrentIndex(0);

    logoTimer = new QTimer(this);
    logoTimer->setSingleShot(true);
    connect(logoTimer,SIGNAL(timeout()),this,SLOT(slot_logoTimerTimeout()));

    showLogoTimer = new QTimer(this);
    showLogoTimer->setSingleShot(true);
    connect(showLogoTimer,SIGNAL(timeout()),this,SLOT(slot_showLogoTimerTimeout()));
    showLogoTimer->start(250);

    sudoGUIProcess = new QProcess(this);
    connect(sudoGUIProcess,SIGNAL(finished(int)),this,SLOT(slot_sudoGUIProcessFinished()));
    QStringList m_parameters;
    m_parameters << "pkexec";
    sudoGUIProcess->start("whereis",m_parameters);
}

clamav_gui::~clamav_gui(){
    delete ui;
}

void clamav_gui::slot_receiveVersionInformation(QString info) {
    ui->frame->setVersionLabel(info);
}

void clamav_gui::closeEvent(QCloseEvent *event){
    if (error == true) {
        qApp->quit();
    } else {
        if (this->isVisible() == true) slot_hideWindow();
        event->ignore();
    }
}

void clamav_gui::changeEvent(QEvent * event){
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized() == true) {
            slot_setMainWindowState(false);
        }
    }
}

void clamav_gui::createTrayIcon(){
    actionShowHideMainWindow = new QAction(QIcon(":/icons/icons/showhide.png"),tr("Show/Hide MainWindow"),this);
    connect(actionShowHideMainWindow,SIGNAL(triggered()),this,SLOT(slot_actionShowHideMainWindowTriggered()));

    actionShowHideDropZone = new QAction(QIcon(":/icons/icons/showhide.png"),tr("Show/Hide DropZone"),this);
    connect(actionShowHideDropZone,SIGNAL(triggered()),this,SLOT(slot_actionShowHideDropZoneTriggered()));

    actionQuit = new QAction(QIcon(":/icons/icons/application-exit.png"),tr("Quit"),this);
    connect(actionQuit,SIGNAL(triggered()),qApp,SLOT(quit()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->setStyleSheet("background:lightgray");
    trayIconMenu->addAction(actionShowHideMainWindow);
    trayIconMenu->addAction(actionShowHideDropZone);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(actionQuit);
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void clamav_gui::slot_actionShowHideMainWindowTriggered(){
    if (setupFile->getSectionBoolValue("Settings","ShowHideMainWindow") == true) {
        slot_setMainWindowState(false);
        setupFile->setSectionValue("Settings","ShowHideMainWindow",false);
    } else {
        slot_setMainWindowState(true);
        setupFile->setSectionValue("Settings","ShowHideMainWindow",true);
    }
}

void clamav_gui::slot_systemTrayIconActivated(QSystemTrayIcon::ActivationReason reason){
    if (reason == QSystemTrayIcon::MiddleClick){
        slot_actionShowHideDropZoneTriggered();
    } else {
        if (reason == QSystemTrayIcon::Trigger){
            slot_actionShowHideMainWindowTriggered();
        }
    }
}

void clamav_gui::slot_setMainWindowState(bool state){
    if (state == true){
        this->showMaximized();
        this->activateWindow();
        setupFile->setSectionValue("Settings","ShowHideMainWindow",true);
    } else {
        if (this->isVisible() == true) this->hide();
        setupFile->setSectionValue("Settings","ShowHideMainWindow",false);
    }
}

void clamav_gui::slot_actionShowHideDropZoneTriggered(){
    if (setupFile->getSectionBoolValue("Settings","ShowHideDropZone") == false){
        setupFile->setSectionValue("Settings","ShowHideDropZone",true);
        createDropZone();
    } else {
        setupFile->setSectionValue("Settings","ShowHideDropZone",false);
        dropZone->close();
    }
}

void clamav_gui::slot_hideWindow(){
    slot_setMainWindowState(false);
}

void clamav_gui::createDropZone(){

    if (setupFile->getSectionBoolValue("Settings","ShowHideMainWindow") == true) this->hide();

    dropZone = new clamav_ctrl();
    connect(dropZone,SIGNAL(scanRequest(QStringList)),this,SLOT(slot_scanRequest(QStringList)));
    dropZone->show();

    if (setupFile->getSectionBoolValue("Settings","ShowHideMainWindow") == true) this->show();
}

void clamav_gui::slot_scanRequest(QStringList scanObjects){
QStringList m_selectedOptions = setupFile->getKeywords("SelectedOptions");
QStringList m_scanLimitations = setupFile->getKeywords("ScanLimitations");
QStringList m_directoryOptions = setupFile->getKeywords("Directories");
QStringList m_parameters;
QStringList m_keywords;
QStringList m_switches;
QString moveDirectory = optionTab->getMoveDirectory();;
QString m_copyDirectory = optionTab->getCopyDirectory();
QString m_checked;
QString m_option;
QString m_value;
QString m_temp;
bool m_useclamdscan = false;
bool m_monochrome = setupFile->getSectionBoolValue("Setup","DisableLogHighlighter");
QString m_color;

    emit setScannerForm(false);
    m_monochrome == true?m_color="#404040;m_color:white":m_color="#ffff00";
    scannerTab->setStatusBarMessage(tr("Scanning started ......."),m_color);

    if (setupFile->getSectionValue("Clamd","Status") == "is running") {
        switch (setupFile->getSectionIntValue("Clamd","ClamdScanMultithreading")) {
            case 0 : m_useclamdscan = false;
                        break;
            case 1 : m_useclamdscan = true;
                        break;
            case 2 : m_useclamdscan = false;
                        break;
            case 3 : m_useclamdscan = true;
                        break;
            case 4 : if (QMessageBox::question(this,tr("Use ClamdScan"),tr("Perform scanning using clamdscan instead of clamscan?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes) {
                        m_useclamdscan = true;
                     } else {
                        m_useclamdscan = false;
                     }
        }
    } else {
        m_useclamdscan = false;
    }

    if (m_useclamdscan == true) {
        m_temp = "clamdscan --config-file " + QDir::homePath() + "/.clamav-gui/clamd.conf --multiscan --fdpass";
        m_parameters << "--config-file" << QDir::homePath() + "/.clamav-gui/clamd.conf" << "--multiscan" << "--fdpass";

        for (int i = 0; i < scanObjects.count(); i++){
            m_parameters << scanObjects.at(i);
            m_temp = m_temp + " " + scanObjects.at(i);
        }

        slot_setMainWindowState(true);

        if (setupFile->getSectionBoolValue("Settings","ShowHideDropZone") == true){
            dropZone->close();
        }

        ui->tabWidget->setCurrentIndex(0);

        scannerTab->clearLogMessage();
        scannerTab->setStatusMessage(m_temp+char(13));

        scanProcess->start("clamdscan",m_parameters);

    } else {

        if (scannerTab->recursivChecked() == true){
            m_parameters << "-r";
        }

        switch (scannerTab->getVirusFoundComboBoxValue()) {
        case 0:
            break;
        case 1: m_parameters << "--remove=yes";
            break;
        case 2: if (moveDirectory != "") m_parameters << "--move=" + moveDirectory;
            break;
        case 3: if (moveDirectory != "") m_parameters << "--copy=" + m_copyDirectory;
            break;
        }

        for (int i = 0; i < m_selectedOptions.count(); i++){
            m_parameters << m_selectedOptions.at(i).left(m_selectedOptions.indexOf("|")).replace("<equal>","=");
        }

        m_keywords << "TmpFile" << "MoveInfectedFiles" << "CopyInfectedFiles" << "SCanFileFromFiles" << "FollowDirectorySymLinks" << "FollowFileSymLinks";
        m_switches << "--tempdir=" << "--move=" << "--copy=" << "--file-list=" << "--follow-dir-symlinks=" << "--follow-file-symlinks=";
        // Directory Options
        for (int i = 0; i < m_directoryOptions.count(); i++){
            m_option = m_directoryOptions.at(i);
            m_value = setupFile->getSectionValue("Directories",m_option);
            m_checked = m_value.left(m_value.indexOf("|"));
            m_value = m_value.mid(m_value.indexOf("|") + 1);


            if ((m_checked == "m_checked") && (m_value != "")) {
                if (m_option == "LoadSupportedDBFiles") m_parameters << "--database=" + m_value;
                if (m_option == "ScanReportToFile") {
                    m_parameters << "--log=" + m_value;
                    if (m_value != "") {
                        QFile file(m_value);
                        if (file.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text)){
                            QTextStream stream(&file);
                            stream << "\n<Scanning startet> " << QDateTime::currentDateTime().toString("yyyy/M/d - hh:mm");
                            file.close();
                        }
                    }
                }
                if (m_keywords.indexOf(m_option) != -1) m_parameters << m_switches.at(m_keywords.indexOf(m_option)) + m_value;
            }
        }

        // Scan Limitations
        QStringList m_SLKeywords;
        QStringList m_SLSwitches;
        m_SLKeywords << "Files larger than this will be skipped and assumed clean" << "The maximum amount of data to scan for each container file";
        m_SLKeywords << "The maximum number of files to scan for each container file" << "Maximum archive recursion level for container file";
        m_SLKeywords << "Maximum directory recursion level" << "Maximum size file to check for embedded PE" << "Maximum size of HTML file to normalize";
        m_SLKeywords << "Maximum size of normalized HTML file to scan" << "Maximum size of script file to normalize" << "Maximum size zip to type reanalyze";
        m_SLKeywords << "Maximum number of partitions in disk image to be scanned" << "Maximum number of icons in PE file to be scanned";
        m_SLKeywords << "Number of seconds to wait for waiting a response back from the stats server" << "Bytecode timeout in milliseconds";
        m_SLKeywords << "Collect and print execution statistics" << "Structured SSN Format" << "Structured SSN Count" << "Structured CC Count" << "Structured CC Mode";
        m_SLKeywords << "Max Scan-Time" << "Max recursion to HWP3 parsing function" << "Max calls to PCRE match function" << "Max recursion calls to the PCRE match function";
        m_SLKeywords << "Max PCRE file size" << "Database outdated if older than x days";
        m_SLSwitches << "--max-filesize=" << "--max-scansize=" << "--max-files=" << "--max-recursion=" << "--max-dir-recursion=";
        m_SLSwitches << "--max-embeddedpe=" << "--max-htmlnormalize=" << "--max-htmlnotags=" << "--max-scriptnormalize=" << "--max-ziptypercg=";
        m_SLSwitches << "--max-partitions=" << "--max-iconspe=" << "--stats-timeout=" << "--bytecode-timeout=" << "--statistics=";
        m_SLSwitches << "--structured-ssn-format=" << "--structured-ssn-count=" << "--structured-cc-count=" << "--structured-cc-mode=" << "--max-scantime=";
        m_SLSwitches << "--max-rechwp3=" << "--pcre-match-limit=" << "--pcre-recmatch-limit=" << "--pcre-max-filesize=" << "--fail-if-cvd-older-than=";
        for (int i = 0; i < m_scanLimitations.count(); i++){
            m_option = m_scanLimitations.at(i);
            m_value = setupFile->getSectionValue("ScanLimitations",m_option);
            m_checked = m_value.left(m_value.indexOf("|"));
            m_value = m_value.mid(m_value.indexOf("|") + 1);
            if (m_checked == "m_checked"){
                for (int i = 0; i < m_SLKeywords.length(); i++) {
                    if (m_option == m_SLKeywords.at(i)){
                        m_parameters << m_SLSwitches.at(i) + m_value;
                    }
                }
            }
        }

        // REGEXP and Include Exclude Options
        m_value = setupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanFileNamesMatchingRegExp");
        m_checked = m_value.left(m_value.indexOf("|"));
        m_value = m_value.mid(m_value.indexOf("|") + 1);
        if (m_checked == "m_checked") m_parameters << "--exclude=" + m_value;

        m_value = setupFile->getSectionValue("REGEXP_and_IncludeExclude","DontScanDirectoriesMatchingRegExp");
        m_checked = m_value.left(m_value.indexOf("|"));
        m_value = m_value.mid(m_value.indexOf("|") + 1);
        if (m_checked == "m_checked") m_parameters << "--exclude-dir=" + m_value;

        m_value = setupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanFileNamesMatchingRegExp");
        m_checked = m_value.left(m_value.indexOf("|"));
        m_value = m_value.mid(m_value.indexOf("|") + 1);
        if (m_checked == "m_checked") m_parameters << "--include=" + m_value;

        m_value = setupFile->getSectionValue("REGEXP_and_IncludeExclude","OnlyScanDirectoriesMatchingRegExp");
        m_checked = m_value.left(m_value.indexOf("|"));
        m_value = m_value.mid(m_value.indexOf("|") + 1);
        if (m_checked == "m_checked") m_parameters << "--include-dir=" + m_value;

        if (setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude","EnablePUAOptions") == true){
            QStringList m_keywords;
            QStringList m_switches;
            m_keywords << "LoadPUAPacked" << "LoadPUAPWTool" << "LoadPUANetTool" << "LoadPUAP2P" << "LoadPUAIRC" << "LoadPUARAT" << "LoadPUANetToolSpy";
            m_keywords << "LoadPUAServer" << "LoadPUAScript" << "LoadPUAAndr" << "LoadPUAJava" << "LoadPUAOsx" << "LoadPUATool" << "LoadPUAUnix" << "LoadPUAWin";
            m_switches << "--include-pua=Packed" << "--include-pua=PWTool" << "--include-pua=NetTool" << "--include-pua=P2P" << "--include-pua=IRC" << "--include-pua=RAT";
            m_switches << "--include-pua=NetToolSpy" << "--include-pua=Server" << "--include-pua=Script" << "--include-pua=Andr" << "--include-pua=Java";
            m_switches << "--include-pua=Osx" << "--include-pua=Tool" << "--include-pua=Unix" << "--include-pua=Win";
            for (int i = 0; i < m_keywords.length(); i++) {
                if (setupFile->getSectionBoolValue("REGEXP_and_IncludeExclude",m_keywords.at(i)) == true) m_parameters << m_switches.at(i);
            }
        }

        m_temp = "clamscan ";
        QString m_path = m_temp;
        m_path = setupFile->getSectionValue("Directories","LoadSupportedDBFiles");
        m_path = m_path.mid(m_path.indexOf("|")+1);
        if (m_path.indexOf("not m_checked") != - 1) m_temp = m_temp + "-d " + m_path;
        for (int i = 0; i < m_parameters.count(); i++){
            m_temp = m_temp + " " + m_parameters.at(i);
        }

        // m_parameters << "-d" << m_path;
        for (int i = 0; i < scanObjects.count(); i++){
            m_parameters << scanObjects.at(i);
            m_temp = m_temp + " " + scanObjects.at(i);
        }

        slot_setMainWindowState(true);

        if (setupFile->getSectionBoolValue("Settings","ShowHideDropZone") == true){
            dropZone->close();
        }

        ui->tabWidget->setCurrentIndex(0);

        scannerTab->clearLogMessage();
        scannerTab->setStatusMessage(m_temp+char(13));

        scanProcess->start("clamscan",m_parameters);
    }
}

void clamav_gui::slot_mainWinTimerTimeout(){
    if (setupFile->getSectionBoolValue("Settings","ShowHideMainWindow") == true) {
        slot_setMainWindowState(true);
    } else {
        slot_setMainWindowState(false);
    }
}

void clamav_gui::slot_scanProcessHasStdOutput(){
QString output = scanProcess->readAllStandardOutput();
    scannerTab->setStatusMessage(output);
}

void clamav_gui::slot_scanProcessHasErrOutput(){
QString output = scanProcess->readAllStandardError();
    scannerTab->setStatusMessage(output);
}

void clamav_gui::slot_scanProcessFinished(int exitCode,QProcess::ExitStatus status){
Q_UNUSED(exitCode);
Q_UNUSED(status);
QStringList m_parameters;
bool m_monochrome = setupFile->getSectionBoolValue("Setup","DisableLogHighlighter");
QString m_color;

    if (status == QProcess::CrashExit) {
        m_monochrome == true?m_color="#404040;m_color:white":m_color="#ff0000";
        scannerTab->setStatusBarMessage(tr("Scan-Process aborted ......"),m_color);
        trayIcon->showMessage(tr("Scan-Status"),tr("Scan Process aborted ....."),QSystemTrayIcon::Warning,5000);
    } else {
        if (exitCode == 0) {
            m_monochrome == true?m_color="#404040;m_color:white":m_color="#00ff00";
            scannerTab->setStatusBarMessage(tr("Scan-Process finished ...... no Virus found!"),m_color);
            trayIcon->showMessage(tr("Scan-Status"),tr("Scan Process finished ..... no virus found!"),QSystemTrayIcon::Information,5000);
        } else
            if (exitCode == 1) {
                m_monochrome == true?m_color="#404040;m_color:white":m_color="#ff0000";
                scannerTab->setStatusBarMessage(tr("Scan-Process finished ...... Virus found!"),m_color);
                trayIcon->showMessage(tr("Scan-Status"),tr("Scan Process finished ..... a virus was found!"),QSystemTrayIcon::Critical,5000);
            } else {
                m_monochrome == true?m_color="#404040;m_color:white":m_color="#ff0000";
                scannerTab->setStatusBarMessage(tr("Scan-Process finished ...... an error occured!"),m_color);
                trayIcon->showMessage(tr("Scan-Status"),tr("Scan Process finished ..... an error occurred!"),QSystemTrayIcon::Warning,5000);
            }
    }

    emit setScannerForm(true);

    if (setupFile->getSectionBoolValue("Settings","ShowHideDropZone") == true){
        createDropZone();
    }

    emit scanJobFinished();
}

void clamav_gui::slot_abortScan(){
bool m_monochrome = setupFile->getSectionBoolValue("Setup","DisableLogHighlighter");
QString m_color;

    m_monochrome == true?m_color="#404040;m_color:white":m_color="#ff0000";
    scannerTab->setStatusMessage(tr("Scan-Process aborted!"));
    scannerTab->setStatusBarMessage(tr("Scan-Process aborted!"),m_color);

    if (scanProcess->state() == QProcess::Running) scanProcess->kill();
}

void clamav_gui::slot_showDropZone(){
    if (setupFile->getSectionBoolValue("Settings","ShowHideDropZone") == false){
        setupFile->setSectionValue("Settings","ShowHideDropZone",true);
        createDropZone();
    }
}

void clamav_gui::slot_receiveScanJob(QString name, QStringList m_parameters){
scheduleScanObject * m_scanObject = new scheduleScanObject(0,name,m_parameters);

    m_scanObject->setWindowTitle(name);
    m_scanObject->setWindowIcon(QIcon(":/icons/icons/media.png"));
    connect(m_scanObject,SIGNAL(sendStatusReport(int,QString,QString)),this,SLOT(slot_setTrayIconBalloonMessage(int,QString,QString)));
    connect(m_scanObject,SIGNAL(scanProcessFinished()),logTab,SLOT(slot_profilesChanged()));
    connect(m_scanObject,SIGNAL(closeRequest(QWidget*)),this,SLOT(slot_closeRequest(QWidget*)));
    m_scanObject->setModal(false);
    m_scanObject->show();
}

void clamav_gui::slot_setTrayIconBalloonMessage(int status,QString title,QString message){
    switch (status){
    case 0  :   trayIcon->showMessage(title,message,QSystemTrayIcon::Information,5000);
                    break;
    case 1  :   trayIcon->showMessage(title,message,QSystemTrayIcon::Warning);
                    break;
    case 2  :   trayIcon->showMessage(title,message,QSystemTrayIcon::Critical);
                    break;
    }

}

void clamav_gui::slot_closeRequest(QWidget * dialog){
    delete dialog;
}

void clamav_gui::slot_logoTimerTimeout(){
    startLogoLabel->hide();
    if (setupFile->keywordExists("Setup","WindowState")) {
        if (setupFile->getSectionValue("Setup","WindowState") == "minimized") this->close();
        if (setupFile->getSectionValue("Setup","WindowState") == "maximized") {
            this->showMaximized();
        }
    } else {
        this->showMaximized();
    }
}

void clamav_gui::slot_showLogoTimerTimeout(){
    QScreen * m_screen = QGuiApplication::primaryScreen();
    QRect m_screenGeometry = m_screen->geometry();

    startLogoLabel = new QLabel(0);
    startLogoLabel->setStyleSheet("background:transparent");
    startLogoLabel->setAttribute(Qt::WA_TranslucentBackground);
    startLogoLabel->setWindowFlags(Qt::FramelessWindowHint);
    startLogoLabel->setPixmap(QPixmap(":/icons/icons/startlogo.png"));
    startLogoLabel->setGeometry((m_screenGeometry.width()-400) / 2, (m_screenGeometry.height() - 300) / 2, 400,300);
    startLogoLabel->show();
    startLogoLabel->raise();

    logoTimer->start(2500);
}

void clamav_gui::slot_errorReporter()
{
    error = true;
}

void clamav_gui::slot_updateDatabase()
{
    ui->tabWidget->setCurrentIndex(5);
    emit startDatabaseUpdate();
}

void clamav_gui::slot_startclamd(){
    ui->tabWidget->setCurrentIndex(6);
}

void clamav_gui::slot_sudoGUIProcessFinished()
{
    QStringList m_parameters;

    QString m_sudoGUIOutput = sudoGUIProcess->readAll();

    if (guisudoapp == "pkexec") {
        if (m_sudoGUIOutput != "pkexec:\n") {
            QStringList m_values = m_sudoGUIOutput.split(" ");
            if (m_values.size() > 1) {
                if (m_values.length() > 0) setupFile->setSectionValue("Settings","SudoGUI",m_values[1]);
            } else {
                guisudoapp = "kdesu";
                m_parameters << "kdesu";
                sudoGUIProcess->start("whereis",m_parameters);
            }
        }
    }

    if (guisudoapp == "kdesu") {
        if (m_sudoGUIOutput != "kdesu:\n") {
            QStringList m_values = m_sudoGUIOutput.split(" ");
            if (m_values.size() > 1) {
                if (m_values.length() > 0) setupFile->setSectionValue("Settings","SudoGUI",m_values[1]);
            } else {
                QMessageBox::warning(this,tr("WARNING"),tr("Neither \"pkexec\" nor \"kdesu\" is installed. Please install at least one of this to apps!"));
            }
        }
    }
}

void clamav_gui::slot_switchActiveTab(int index)
{
    ui->tabWidget->setCurrentIndex(index);
}

void clamav_gui::slot_quitApplication()
{
    qApp->quit();
}
