#include "firstrunwindow.h"
#include "ui_firstrunwindow.h"

firstRunWindow::firstRunWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::firstRunWindow)
{
    ui->setupUi(this);

    processInit = true;
    createServiceMenu();
    createBaseDirStructur();

    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini");
    createInitialSettings();
    createClamdConfFile();


    initCommands << "whereis" << "whereis" << "whereis" << "whereis" << "whereis" << "whereis" << "whereis" << "whoami" << "groups" << "man";
    initParameters << "freshclam" << "clamd" << "clamonacc" << "clamscan" << "clamdscan" << "pkexec" << "kdesu" << "" << "" << "clamd.conf";
    initIndex = 0;

    initProcess = new QProcess(this);
    connect(initProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(slot_initProcessFinished()));

    processParameters << initParameters.at(initIndex);
    initProcess->start(initCommands.at(initIndex),processParameters);

    processInit = false;
}

firstRunWindow::~firstRunWindow()
{
    delete ui;
}

void firstRunWindow::slot_initProcessFinished()
{
    QString rc = initProcess->readAll().trimmed();
    QStringList elements = rc.split(" ");

    switch (initIndex) {
        case 0 :
            if (elements.count() == 3) {
                ui->freshclamSourceLabel->setText(elements.at(1));
                ui->freshclamStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
                setupFile->setSectionValue("FreshclamSettings","FreshclamLocation",elements.at(1));
            }
            break;
        case 1 :
            if (elements.count() == 3) {
                ui->clamdSourceLabel->setText(elements.at(1));
                ui->clamdStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
                setupFile->setSectionValue("Clamd","ClamdLocation",elements.at(1));
                setupFile->setSectionValue("Clamd","StartClamdOnStartup",false);
            }
            break;
        case 2 :
            if (elements.count() == 3) {
                ui->clamonaccSourceLabel->setText(elements.at(1));
                ui->clamonaccStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
                setupFile->setSectionValue("Clamd","ClamonaccLocation",elements.at(1));
            }
            break;
        case 3 :
            if (elements.count() == 3) {
                ui->clamscanSourceLabel->setText(elements.at(1));
                ui->clamscanStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
            }
            break;
        case 4 :
            if (elements.count() == 3) {
                ui->clamdscanSourceLabel->setText(elements.at(1));
                ui->clamdscanStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
            }
            break;
        case 5 :
            if (elements.count() == 3) {
                ui->sudoGUISourceLabel->setText(elements.at(1));
                ui->sudoGUIStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
                initIndex = 6;
            }
            break;
        case 6 :
            if (elements.count() == 3) {
                ui->sudoGUISourceLabel->setText(elements.at(1));
                ui->sudoGUIStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
            }
            break;
        case 7 :
            if (rc != "") {
                ui->applicationUserLabel->setText("Application User : " + elements.at(0));
                ui->applicationUserStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
            }
            break;
        case 8 :
            if (elements.count() > 0) {
                ui->applicationGroupLabel->setText("Application Group : " + elements.at(0));
                ui->applicationGroupStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
            }
            break;
        case 9 :
            QFile file(QDir::homePath() + "/.clamav-gui/clamd.conf.man");
            if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
                QTextStream stream(&file);
                stream << rc;
                Qt::endl(stream);
                file.close();
            }
            break;
    }

    if (initIndex < initCommands.size()-1) {
        switch (initIndex) {
            case 0:
                setupFile->setSectionValue("FreshclamSettings", "FreshclamLocation", elements.at(1));
                break;
            case 1:
            case 2:
                break;
            case 3:
                if (rc == "") {
                    QMessageBox::warning(this, "ERROR", "Clamav is missing. Please install!", QMessageBox::Ok);
                    emit quitApplication();
                }
                break;
            case 4:
                break;
            case 5:
            case 6:
                if (elements.count() == 3) {
                    setupFile->setSectionValue("RequiredApplications",initParameters.at(initIndex),elements.at(1));
                    setupFile->setSectionValue("Settings","SudoGUI",elements.at(1));
                } else {
                    setupFile->setSectionValue("RequiredApplications",initParameters.at(initIndex),"n/a");
                    setupFile->setSectionValue("Settings","SudoGUI","n/a");
                }
                break;
            case 7:
            case 8:
                setupFile->setSectionValue("RequiredApplications","Group",ui->applicationGroupLabel->text());
                break;
            case 9:
                setupFile->setSectionValue("RequiredApplication","User",ui->applicationUserLabel->text());
                break;
        }

        initIndex++;
        processParameters.clear();
        if (initParameters.at(initIndex) != "") {
            processParameters << initParameters.at(initIndex);
            initProcess->start(initCommands.at(initIndex),processParameters);
        } else {
            processParameters << initParameters.at(initIndex);
            initProcess->start(initCommands.at(initIndex),QStringList());
        }
    }
}

void firstRunWindow::slot_sddComboBoxChanged()
{
    if (processInit == false) {
        setupFileHandler * m_clamdConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/clamd.conf");
        setupFileHandler * m_freshclamConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/freshclam.conf");
        if (ui->signatureDatabaseDirectoryComboBox->currentText() == QDir::homePath() + "/.clamav-gui/signatures") {
            setupFile->setSectionValue("Directories", "LoadSupportedDBFiles", "checked|" + QDir::homePath() + "/.clamav-gui/signatures");
            setupFile->setSectionValue("FreshClam","runasroot",false);
            m_clamdConf->setSingleLineValue("DatabaseDirectory",QDir::homePath() + "/.clamav-gui/signatures");
            m_freshclamConf->setSingleLineValue("DatabaseDirectory", QDir::homePath() + "/.clamav-gui/signatures");
        } else  {
            setupFile->setSectionValue("Directories", "LoadSupportedDBFiles", "checked|" + ui->signatureDatabaseDirectoryComboBox->currentText());
            setupFile->setSectionValue("FreshClam","runasroot",true);
            m_clamdConf->setSingleLineValue("DatabaseDirectory",ui->signatureDatabaseDirectoryComboBox->currentText());
            m_freshclamConf->setSingleLineValue("DatabaseDirectory",ui->signatureDatabaseDirectoryComboBox->currentText());
        }
        emit settingChanged();
        delete m_clamdConf;
        delete m_freshclamConf;
    }
}

void firstRunWindow::slot_done()
{
    emit doneit();
    this->accept();
}

void firstRunWindow::createBaseDirStructur()
{
    //*****************************************************************************
    //creating service Menu
    //*****************************************************************************
    QDir dir(QDir::homePath());
    dir.mkpath(QDir::homePath() + "/.clamav-gui");
    dir.mkpath(QDir::homePath() + "/.clamav-gui/logs");
    dir.mkpath(QDir::homePath() + "/.clamav-gui/profiles");
    dir.mkpath(QDir::homePath() + "/.clamav-gui/quarantine");
    dir.mkpath(QDir::homePath() + "/.clamav-gui/signatures");
    QFile::setPermissions(QDir::homePath() + "/.clamav-gui/signatures",
                          QFileDevice::ReadOwner  | QFileDevice::ReadUser  | QFileDevice::ReadGroup  | QFileDevice::ReadOther  |
                          QFileDevice::WriteOwner | QFileDevice::WriteUser | QFileDevice::WriteGroup | QFileDevice::WriteOther |
                          QFileDevice::ExeOwner   | QFileDevice::ExeUser   | QFileDevice::ExeGroup   | QFileDevice::ExeOther);
    //_____________________________________________________________________________________________________________________________________
    // For UALinux
    // If the settings.ini in the home folder of the user is not present a predefined version is been copied into the folder.
    //_____________________________________________________________________________________________________________________________________
    if ((!QFileInfo::exists(QDir::homePath() + "/.clamav-gui/settings.ini")) && (!QFile::exists(QDir::homePath() + "/.clamav-gui/settings.ini"))) {
        dir.mkdir(QDir::homePath() + "/.clamav-gui");

        if (QFile::exists("/etc/clamav-gui/settings.ini"))
            QFile::copy("/etc/clamav-gui/settings.ini", QDir::homePath() + "/.clamav-gui/settings.ini");
    }
    //______________________________________________________________________________________________________________________________________

    ui->initialDirectoryStructureStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
}

void firstRunWindow::createServiceMenu()
{
    //*****************************************************************************
    //creating service Menu
    //*****************************************************************************
    QString serviceMenuPath;
    if (QFileInfo::exists(QDir::homePath() + "/.local/share/kservices5/ServiceMenus"))
        serviceMenuPath = QDir::homePath() + "/.local/share/kservices5/ServiceMenus";

    if (serviceMenuPath.isEmpty() && QFileInfo::exists(QDir::homePath() + "/.local/share/kio/servicemenus"))
        serviceMenuPath = QDir::homePath() + "/.local/share/kio/servicemenus";

    if (serviceMenuPath.isEmpty())
        serviceMenuPath = serviceMenuPath = QDir::homePath() + "/.local/share/kservices5/ServiceMenus";

    if (!QFileInfo::exists(serviceMenuPath + "/scanWithClamAV-GUI.desktop")) {
        if (!QFileInfo::exists(serviceMenuPath)) {
            QDir dir(serviceMenuPath);
            dir.mkpath(serviceMenuPath);
        }
        setupFileHandler* serviceFile = new setupFileHandler(serviceMenuPath + "/scanWithClamAV-GUI.desktop", this);
        serviceFile->setSectionValue("Desktop Entry", "Type", "Service");
        serviceFile->setSectionValue("Desktop Entry", "ServiceTypes", "KonqPopupMenu/Plugin");
        serviceFile->setSectionValue("Desktop Entry", "MimeType", "all/all;");
        serviceFile->setSectionValue("Desktop Entry", "Actions", "scan;");
        serviceFile->setSectionValue("Desktop Entry", "Icon", "clamav-gui");
        serviceFile->setSectionValue("Desktop Entry", "X-KDE-Priority", "TopLevel");
        serviceFile->setSectionValue("Desktop Entry", "X-KDE-StartupNotify", "false");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu", "Scan with ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu[de]", "Scannen mit ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu[da_DK]", "Scannen med ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu[es_ES]", "Analizar con ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu[us]", "Scan with ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu[gb]", "Scan with ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu[pt]", "Investigar com ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu[br]", "Investigar com ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu[pt_BR]", "Investigar com ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu[fr]", "Scanner avec ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu[it]", "Scansione con ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Entry", "NO-X-KDE-Submenu[uk]", "Сканування за допомогою ClamAV-GUI");

        serviceFile->setSectionValue("Desktop Action scan", "Name", "scan");
        serviceFile->setSectionValue("Desktop Action scan", "Name[de]", "Scannen mit ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan", "Name[es_ES]", "Analizar con ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan", "Name[us]", "Scan with ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan", "Name[gb]", "Scan with ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan", "Name[pt]", "Investigar com ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan", "Name[br]", "Investigar com ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan", "Name[fr]", "Scanner avec ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan", "Name[it]", "Scansione con ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan", "Name[uk]", "Сканування за допомогою ClamAV-GUI");
        serviceFile->setSectionValue("Desktop Action scan", "Icon", "clamav-gui");
        serviceFile->setSectionValue("Desktop Action scan", "Exec", "clamav-gui --scan %F");
        delete serviceFile;
    }
    //*****************************************************************************

    ui->dolphinContestMenuStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
}

void firstRunWindow::createInitialSettings()
{
    if ((QFileInfo::exists("/var/lib/clamav")) && (QFile::exists("/var/lib/clamav/freshclam.dat"))) {
        if (setupFile->keywordExists("Directories","LoadSupportedDBFiles") == false)
            setupFile->setSectionValue("Directories", "LoadSupportedDBFiles", "checked|/var/lib/clamav");

        if (setupFile->keywordExists("FreshClam","runasroot") == false)
            setupFile->setSectionValue("FreshClam","runasroot",true);

        ui->signatureDatabaseDirectoryComboBox->addItem("/var/lib/clamav");
        ui->signatureDatabaseDirectoryComboBox->addItem(QDir::homePath() + "/.clamav-gui/signatures");
    }
    else {
        if (setupFile->keywordExists("Directories","LoadSupportedDBFiles") == false)
            setupFile->setSectionValue("Directories", "LoadSupportedDBFiles", "checked|" + QDir::homePath() + "/.clamav-gui/signatures");

        setupFile->setSectionValue("FreshClam","runasroot",false);
        if (setupFile->keywordExists("FreshClam","runasroot") == false)

            ui->signatureDatabaseDirectoryComboBox->addItem("/.clamav-gui/signatures");
    }
    if (setupFile->keywordExists("Directories", "TmpFile") == false)
        setupFile->setSectionValue("Directories", "TmpFile", "checked|/tmp");

    if (setupFile->keywordExists("Directories", "ScanReportToFile") == false)
        setupFile->setSectionValue("Directories", "ScanReportToFile", "checked|" + QDir::homePath() + "/.clamav-gui/logs/report-scan.log");

    if (setupFile->keywordExists("Directories", "MoveInfectedFiles") == false)
        setupFile->setSectionValue("Directories", "MoveInfectedFiles", "not checked|" + QDir::homePath() + "/.clamav-gui/quarantine");

    if (setupFile->keywordExists("Directories", "CopyInfectedFiles") == false)
        setupFile->setSectionValue("Directories", "CopyInfectedFiles", "not checked|" + QDir::homePath() + "/.clamav-gui/quarantine");

    if (setupFile->keywordExists("Setup", "language") == false)
        setupFile->setSectionValue("Setup", "language", 2);

    if (setupFile->keywordExists("FreshClam","UpdatesPerDay") == false)
        setupFile->setSectionValue("FreshClam","UpdatesPerDay",12);

    if (setupFile->keywordExists("FreshClam","DataBaseToUpdate") == false)
        setupFile->setSectionValue("FreshClam","DataBaseToUpdate",0);

    if (setupFile->keywordExists("Freshclam","StartDaemon") == false)
        setupFile->setSectionValue("Freshclam","StartDaemon",false);

    if (setupFile->keywordExists("Setup","DisableLogHighlighter") == false)
        setupFile->setSectionValue("Setup","DisableLogHighlighter",false);

    if (setupFile->keywordExists("Setup","WindowState") == false)
        setupFile->setSectionValue("Setup","WindowState","maximized");

    ui->initialSettingsFileStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
}

void firstRunWindow::createClamdConfFile()
{
    QFile clamdConfFile(QDir::homePath() + "/.clamav-gui/clamd.conf");

    if (clamdConfFile.exists() == false) {
        m_clamdConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/clamd.conf", this);
        QString value = setupFile->getSectionValue("Directories", "LoadSupportedDBFiles");
        if (value.indexOf("checked|") == 0)
            m_clamdConf->addSingleLineValue("DatabaseDirectory", value.mid(value.indexOf("|") + 1));
        m_clamdConf->setSingleLineValue("LogSyslog", "no");
        m_clamdConf->setSingleLineValue("LogFacility", "LOG_LOCAL6");
        m_clamdConf->setSingleLineValue("PidFile", "/tmp/clamd.pid");
        m_clamdConf->setSingleLineValue("ExtendedDetectionInfo", "yes");
        m_clamdConf->setSingleLineValue("LocalSocket", QDir::homePath() + "/.clamav-gui/clamd-socket");
        m_clamdConf->setSingleLineValue("LogFile", QDir::homePath() + "/.clamav-gui/clamd.log");
        m_clamdConf->setSingleLineValue("LocalSocketGroup", "users");
        m_clamdConf->setSingleLineValue("TCPAddr", "127.0.0.1");
        m_clamdConf->addSingleLineValue("TCPAddr", "::1");
        m_clamdConf->setSingleLineValue("LogFileMaxSize", "1M");
        m_clamdConf->setSingleLineValue("LogTime", "yes");
        m_clamdConf->setSingleLineValue("LogRotate", "yes");
        m_clamdConf->setSingleLineValue("OnAccessMaxFileSize", "10M");
        m_clamdConf->setSingleLineValue("OnAccessMaxThreads", "10");
        m_clamdConf->setSingleLineValue("OnAccessPrevention", "yes");
        m_clamdConf->setSingleLineValue("OnAccessDenyOnError", "no");
        m_clamdConf->setSingleLineValue("OnAccessExtraScanning", "yes");
        m_clamdConf->setSingleLineValue("OnAccessRetryAttempts", "0");
        m_clamdConf->setSingleLineValue("OnAccessExcludeUname", "root");
        m_clamdConf->setSingleLineValue("OnAccessExcludeUID", "0");
    }

    ui->clamdConfStatusLabel->setPixmap(QPixmap(":/icons/icons/create.png"));
}

void firstRunWindow::createFreshclamConfFile()
{
    if (QFile::exists(QDir::homePath() + "/.clamav-gui/freshclam.conf") == false) {
        QFile freshclamConfFile(QDir::homePath() + "/.clamav-gui/freshclam.conf");
        freshclamConfFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);

        m_freshclamConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/freshclam.conf", this);
        QString value = setupFile->getSectionValue("Directories", "LoadSupportedDBFiles");
        if (value.indexOf("checked|") == 0)
            m_freshclamConf->setSingleLineValue("DatabaseDirectory", value.mid(value.indexOf("|") + 1));

        m_freshclamConf->setSingleLineValue("LogSyslog", "no");
        m_freshclamConf->setSingleLineValue("LogFacility", "LOG_LOCAL6");
        m_freshclamConf->setSingleLineValue("LogTime", "no");
        m_freshclamConf->setSingleLineValue("LogRotate", "no");
        m_freshclamConf->setSingleLineValue("PidFile", "/tmp/freshclam.pid");
        m_freshclamConf->setSingleLineValue("DatabaseOwner", "clamav");
        m_freshclamConf->setSingleLineValue("DatabaseMirror", "database.clamav.net");
        m_freshclamConf->setSingleLineValue("LogVerbose", "no");
        m_freshclamConf->setSingleLineValue("Checks", "12");
    }
}

