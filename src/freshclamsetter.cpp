#include "freshclamsetter.h"
#include "ui_freshclamsetter.h"
#define css_green "background-color:green;color:yellow"
#define css_red "background-color:red;color:yellow"
#define css_mono "background-color:#404040;color:white"

freshclamsetter::freshclamsetter(QWidget *parent) : QWidget(parent), ui(new Ui::freshclamsetter){
QDir m_tempDir;

    ui->setupUi(this);

    startup = true;

    lockFreshclamConf = true;

    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    updateLogFileWatcher = NULL;
    updateLogHighLighter = NULL;
    monochrome = true;
    ui->groupBox->setStyleSheet("");
    if (setupFile->getSectionBoolValue("Setup","DisableLogHighlighter") == false) {
      monochrome = false;
      updateLogHighLighter = new highlighter(ui->logPlainText->document());
      freshclamLogHighLighter = new highlighter(ui->deamonLogText->document());
      ui->groupBox->setStyleSheet(css_mono);
      ui->updateInfoText->setStyleSheet(css_mono);
    }

    sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");

    QFile m_freshclamConfFile(QDir::homePath() + "/.clamav-gui/freshclam.conf");
    m_freshclamConfFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner);

    freshclamConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/freshclam.conf",this);

    updater = new QProcess(this);
    connect(updater,SIGNAL(readyReadStandardError()),this,SLOT(slot_updaterHasOutput()));
    connect(updater,SIGNAL(readyReadStandardOutput()),this,SLOT(slot_updaterHasOutput()));
    connect(updater,SIGNAL(finished(int)),this,SLOT(slot_updaterFinished(int)));

    startDeamonProcess = new QProcess(this);
    connect(startDeamonProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this, SLOT(slot_startDeamonProcessFinished(int,QProcess::ExitStatus)));

    pidFileWatcher = new QFileSystemWatcher(this);
    connect(pidFileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(slot_fileSystemWatcherTriggered()));

    logFileWatcher = new QFileSystemWatcher(this);
    connect(logFileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(slot_logFileWatcherTriggered()));

    updateLogFileWatcher = new QFileSystemWatcher(this);
    connect(updateLogFileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(slot_updateFileWatcherTriggered()));

    ps_process = new QProcess;
    connect(ps_process,SIGNAL(finished(int)),this,SLOT(slot_ps_processFinished(int)));

    freshclamStartupCounter = 5;
    startDelayTimer = new QTimer(this);
    startDelayTimer->setSingleShot(true);
    connect(startDelayTimer,SIGNAL(timeout()),this,SLOT(slot_startDelayTimerExpired()));

    QFile m_file(QDir::homePath() + "/.clamav-gui/update.log");
    if (m_tempDir.exists(QDir::homePath() + "/.clamav-gui/update.log") == true) {
        slot_updateFileWatcherTriggered();
        m_file.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ReadOther|QFileDevice::WriteOther);
        updateLogFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/update.log");
    } else {
        if (m_file.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text)){
            QTextStream stream(&m_file);
            stream << "";
            m_file.close();
            slot_updateFileWatcherTriggered();
            updateLogFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/update.log");
            m_file.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ReadOther|QFileDevice::WriteOther);
        }
    }

    QFile m_fileFreshclamLog(QDir::homePath() + "/.clamav-gui/freshclam.log");
    if (m_tempDir.exists(QDir::homePath() + "/.clamav-gui/freshclam.log") == true) {
        m_fileFreshclamLog.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ReadOther|QFileDevice::WriteOther);
        logFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/freshclam.log");
    } else {
        if (m_fileFreshclamLog.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text)){
            QTextStream stream(&m_fileFreshclamLog);
            stream << "";
            m_fileFreshclamLog.close();
            logFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/freshclam.log");
            m_fileFreshclamLog.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ReadOther|QFileDevice::WriteOther);
        }
    }

    processWatcher = new QTimer(this);
    connect(processWatcher,SIGNAL(timeout()),this,SLOT(slot_processWatcherExpired()));
    processWatcher->start(5000);

    checkDaemonRunning();

    initFreshclamSettings();

    setUpdaterInfo();
}

freshclamsetter::~freshclamsetter(){
    delete ui;
}

void freshclamsetter::slot_updateNowButtonClicked(){
QStringList m_parameters;

    busyLabel = new progressDialog(this);
    busyLabel->setStyleSheet("background-color: rgba(192,192,192, 200);");
    busyLabel->setGeometry((this->width() - 300) / 2,(this->height() - 160) / 2,300,160);
    busyLabel->show();
    busyLabel->setText(tr("Update process startet ....."));

    setForm(false);

    if (setupFile->getSectionBoolValue("FreshClam","runasroot") == true){
        QStringList m_databaseToUpdate;
        m_databaseToUpdate << "all" << "main" << "daily" << "bytecode";
        QString m_whatDB = "";
        QString m_para;
        if (setupFile->getSectionIntValue("FreshClam","DataBaseToUpdate") > 0) m_whatDB = " --update-db=" + m_databaseToUpdate[setupFile->getSectionIntValue("FreshClam","DataBaseToUpdate")];
        if ((setupFile->getSectionValue("Directories","LoadSupportedDBFiles") != "") && (setupFile->getSectionValue("Directories","LoadSupportedDBFiles").indexOf("not checked") == -1)){
            m_para = setupFile->getSectionValue("FreshclamSettings","FreshclamLocation") + " --show-progress --datadir=" + setupFile->getSectionValue("Directories","LoadSupportedDBFiles").mid(setupFile->getSectionValue("Directories","LoadSupportedDBFiles").indexOf("|")+1) + " 2>&1 > " + QDir::homePath() + "/.clamav-gui/update.log" + " --config-file=" + QDir::homePath() + "/.clamav-gui/freshclam.conf" + m_whatDB;
        } else {
            m_para = setupFile->getSectionValue("FreshclamSettings","FreshclamLocation") + " 2>&1 > " + QDir::homePath() + "/.clamav-gui/update.log" + " --show-progress --config-file=" + QDir::homePath() + "/.clamav-gui/freshclam.conf" + m_whatDB;
        }
        QFile m_startfreshclamFile(QDir::homePath() + "/.clamav-gui/startfreshclam.sh");
        m_startfreshclamFile.remove();
        if (m_startfreshclamFile.open(QIODevice::Text|QIODevice::ReadWrite)){
            QTextStream m_stream(&m_startfreshclamFile);
            m_stream << "#!/bin/bash\n" << m_para;
            m_startfreshclamFile.close();
            m_startfreshclamFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup);
        }
        m_parameters << QDir::homePath() + "/.clamav-gui/startfreshclam.sh";
        if (sudoGUI == "") sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");
        updater->start(sudoGUI,m_parameters);
    } else {
        QStringList m_databaseToUpdate;
        m_databaseToUpdate << "all" << "main" << "daily" << "bytecode";
        QString m_databaseDir = setupFile->getSectionValue("Directories","LoadSupportedDBFiles");
        QString m_checked = m_databaseDir.mid(0,m_databaseDir.indexOf("|"));
        m_databaseDir = m_databaseDir.mid(m_databaseDir.indexOf("|") + 1);
        if ((m_databaseDir != "") && (m_checked == "checked")) m_parameters << "--datadir=" + m_databaseDir;
        if (setupFile->getSectionIntValue("FreshClam","DataBaseToUpdate") > 0) m_parameters << "--update-db=" + m_databaseToUpdate[setupFile->getSectionIntValue("FreshClam","DataBaseToUpdate")];
        m_parameters << "-l" << QDir::homePath() + "/.clamav-gui/update.log";
        m_parameters << "--show-progress";
        m_parameters << "--config-file" << QDir::homePath() + "/.clamav-gui/freshclam.conf";
        updater->start(setupFile->getSectionValue("FreshclamSettings","FreshclamLocation") ,m_parameters);
    }
    updateLogFileWatcher->removePath(QDir::homePath() + "/.clamav-gui/update.log");
    updateLogFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/update.log");
}


void freshclamsetter::slot_startStopDeamonButtonClicked(){
QStringList m_parameters;
    if (pidFile == ""){
        if (logFile != "") logFileWatcher->removePath(logFile);
        pidFile = freshclamConf->getSingleLineValue("PidFile");
        logFile = QDir::homePath() + "/.clamav-gui/freshclam.log";
        logFileWatcher->addPath(logFile);
        if (setupFile->getSectionBoolValue("FreshClam","runasroot") == true){
            if (startup == false) {
                QString para = setupFile->getSectionValue("FreshclamSettings","FreshclamLocation") + " -d -l " + logFile + " --config-file=" + QDir::homePath() + "/.clamav-gui/freshclam.conf";
                QFile m_startfreshclamFile(QDir::homePath() + "/.clamav-gui/startfreshclam.sh");
                m_startfreshclamFile.remove();
                if (m_startfreshclamFile.open(QIODevice::Text|QIODevice::ReadWrite)){
                    QTextStream m_stream(&m_startfreshclamFile);
                    m_stream << "#!/bin/bash\n" << para;
                    m_startfreshclamFile.close();
                    m_startfreshclamFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup);
                }
                m_parameters << QDir::homePath() + "/.clamav-gui/startfreshclam.sh";
                if (sudoGUI == "") sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");
                startDeamonProcess->start(sudoGUI,m_parameters);
            } else {
                startDelayTimer->start(2500);
            }
        } else {
            m_parameters << "-d";
            m_parameters << "-l" << logFile;
            m_parameters << "--config-file" << QDir::homePath() + "/.clamav-gui/freshclam.conf";
            startDeamonProcess->start(setupFile->getSectionValue("FreshclamSettings","FreshclamLocation"),m_parameters);
        }
    } else {
        QFile m_tempFile(pidFile);
        QString m_pidString;
        if (m_tempFile.exists() == true) {
            m_tempFile.open(QIODevice::ReadOnly);
            QTextStream m_stream(&m_tempFile);
            m_pidString = m_stream.readLine();
        }
        if (setupFile->getSectionBoolValue("FreshClam","runasroot") == true) {
          QString m_para = "/bin/kill -sigterm " + m_pidString + " && rm " + pidFile;
          QFile m_stopfreshclamFile(QDir::homePath() + "/.clamav-gui/stopfreshclam.sh");
          m_stopfreshclamFile.remove();
          if (m_stopfreshclamFile.open(QIODevice::Text|QIODevice::ReadWrite)){
              QTextStream m_stream(&m_stopfreshclamFile);
              m_stream << "#!/bin/bash\n" << m_para;
              m_stopfreshclamFile.close();
              m_stopfreshclamFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup);
          }
          m_parameters << QDir::homePath() + "/.clamav-gui/stopfreshclam.sh";
          if (sudoGUI == "") sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");
          QProcess::execute(sudoGUI,m_parameters);
          checkDaemonRunning();
        } else {
          m_parameters << "-sigterm" << m_pidString;
          QProcess::execute("kill",m_parameters);
          checkDaemonRunning();
        }
    }
}

void freshclamsetter::checkDaemonRunning(){
    QStringList m_ps_parameters;
    m_ps_parameters << "-s" << "freshclam";
    ps_process->start("pidof",m_ps_parameters);
}

void freshclamsetter::slot_ps_processFinished(int rc){
    QFile m_tempFile;

    if (rc == 0) {
        pidFile = freshclamConf->getSingleLineValue("PidFile");
        logFile = QDir::homePath() + "/.clamav-gui/freshclam.log";
    } else {
        pidFile = "";
        logFile = "";
    }

    if ((pidFile != "") && (m_tempFile.exists(pidFile) == true)){
        freshclamStartupCounter = 0;

        emit freshclamStarted();
        ui->startStopDeamonButton->setText(tr("Deamon running - stop deamon"));
        ui->startStopDeamonButton->setStyleSheet(selectColor("green"));
        ui->updateNowButton->setStyleSheet(selectColor("green"));
/*        if (monochrome == false) ui->frame->setStyleSheet("background-color:#c0c0c0;"); else ui->frame->setStyleSheet("");*/
        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/Clam.png"));

        setupFile->setSectionValue("Freshclam","Started",true);

        pidFileWatcher->addPath(pidFile);

        slot_setFreshclamsettingsFrameState(false);

        QFile m_tempFile(pidFile);
        QString m_pidString;
        if (m_tempFile.exists() == true) {
            m_tempFile.open(QIODevice::ReadOnly);
            QTextStream m_stream(&m_tempFile);
            m_pidString = m_stream.readLine();
        }
        setupFile->setSectionValue("Freshclam","Pid",m_pidString);
        emit systemStatusChanged();
    } else {
        pidFile = "";

        ui->startStopDeamonButton->setText(tr("Deamon not running - start deamon"));
        ui->startStopDeamonButton->setStyleSheet(selectColor("red"));
        ui->updateNowButton->setStyleSheet(selectColor("green"));

//        if (monochrome == false) ui->frame->setStyleSheet("background-color:#c0c0c0;"); else ui->frame->setStyleSheet("");
        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/Clam.png"));

        setupFile->setSectionValue("Freshclam","Started",false);
        setupFile->setSectionValue("Freshclam","Pid","n/a");

        slot_setFreshclamsettingsFrameState(true);

        if ((startup == true) && (setupFile->getSectionBoolValue("Freshclam","StartDaemon") == true)) {
            freshclamStartupCounter--;
            if (freshclamStartupCounter > 0) startDelayTimer->start(2500);
        } else {
            if (startup == true) emit freshclamStarted();
        }
    }

    if ((logFile != "") && (m_tempFile.exists(logFile) == true)){
        logFileWatcher->addPath(logFile);
        slot_logFileWatcherTriggered();
    } else {
        logFile = QDir::homePath() + "/.clamav-gui/freshclam.log";
        if (m_tempFile.exists(logFile) == true){
          logFileWatcher->addPath(logFile);
          slot_logFileWatcherTriggered();
        } else {
          QFile m_file(QDir::homePath() + "/.clamav-gui/freshclam.log");
          if (m_file.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text)){
              QTextStream m_stream(&m_file);
              m_stream << "";
              m_file.close();
              logFileWatcher->addPath(logFile);
              slot_logFileWatcherTriggered();
          }
        }
    }
    startup = false;
}

void freshclamsetter::slot_disableUpdateButtons(){
    setForm(false);
}

void freshclamsetter::slot_startDelayTimerExpired(){
QStringList m_parameters;
    if (setupFile->getSectionBoolValue("FreshClam","runasroot") == true) {
        QString m_para = setupFile->getSectionValue("FreshclamSettings","FreshclamLocation") + " -d -l " + logFile + " --config-file=" + QDir::homePath() + "/.clamav-gui/freshclam.conf";
        QFile m_startfreshclamFile(QDir::homePath() + "/.clamav-gui/startfreshclam.sh");
        m_startfreshclamFile.remove();
        if (m_startfreshclamFile.open(QIODevice::Text|QIODevice::ReadWrite)){
            QTextStream m_stream(&m_startfreshclamFile);
            m_stream << "#!/bin/bash\n" << m_para;
            m_startfreshclamFile.close();
            m_startfreshclamFile.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup);
        }
        m_parameters << QDir::homePath() + "/.clamav-gui/startfreshclam.sh";
        if (sudoGUI == "") sudoGUI = setupFile->getSectionValue("Settings","SudoGUI");
        startDeamonProcess->start(sudoGUI,m_parameters);
    } else {
        m_parameters << "-d";
        m_parameters << "-l" << logFile;
        m_parameters << "--config-file" << QDir::homePath() + "/.clamav-gui/freshclam.conf";
        startDeamonProcess->start(setupFile->getSectionValue("FreshclamSettings","FreshclamLocation"),m_parameters);
        startup = false;
        emit freshclamStarted();
    }
}

void freshclamsetter::slot_updaterFinished(int rc){
    delete busyLabel;
    QString m_rcstring = updater->readAll();

    if (rc == 0) {
        emit setBallonMessage(0,tr("INFO"),tr("Update-Process finished"));
        setUpdaterInfo();
    } else {
        emit setBallonMessage(1,tr("WARNING"),tr("Update-Process failed!\nRead log-messages for possible reason."));
    }
    setForm(true);
}

void freshclamsetter::slot_fileSystemWatcherTriggered(){
QDir m_tempDir;

    if ((pidFile != "") && (m_tempDir.exists(pidFile) == true)){
        ui->startStopDeamonButton->setText(tr("Deamon running - stop deamon"));
        ui->startStopDeamonButton->setStyleSheet(selectColor("green"));
        ui->updateNowButton->setStyleSheet("green");

//        if (monochrome == false) ui->frame->setStyleSheet("background-color:#c0c0c0;"); else ui->frame->setStyleSheet("");

        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/Clam.png"));
        pidFileWatcher->addPath(pidFile);
    } else {
        pidFile = "";
        ui->startStopDeamonButton->setText(tr("Deamon not running - start deamon"));
        ui->startStopDeamonButton->setStyleSheet(selectColor("red"));
        ui->updateNowButton->setStyleSheet(selectColor("green"));
//        if (monochrome == false) ui->frame->setStyleSheet("background-color:#c0c0c0;"); else ui->frame->setStyleSheet("");

        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/freshclam.png"));
        setupFile->setSectionValue("Freshclam","Pid","n/a");
        emit systemStatusChanged();
    }
}

void freshclamsetter::slot_clearLogButtonClicked(){
QFile m_file(QDir::homePath() + "/.clamav-gui/update.log");

    m_file.remove();
    m_file.open(QIODevice::ReadWrite);
    QTextStream m_stream(&m_file);
    m_stream << "";
    m_file.close();
    ui->logPlainText->setPlainText("");
}

void freshclamsetter::slot_logFileWatcherTriggered(){
QFile m_file(logFile);
QString content;
QString value;
int pos;

    if (m_file.exists() == true) {
        m_file.open(QIODevice::ReadOnly);
        QTextStream m_stream(&m_file);
        content = m_stream.readAll();
        m_file.close();
    }

    pos = content.lastIndexOf("ClamAV update process started at");
    if (pos != -1){
        value = content.mid(pos + 33,content.indexOf("\n",pos + 33) - (pos + 33));
        setupFile->setSectionValue("Updater","LastUpdate",value);
    }

    pos = content.lastIndexOf("main.cvd updated (");
    if (pos != -1){
        value = content.mid(pos + 17,content.indexOf("\n",pos + 17) - (pos + 17));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","MainVersion",value);
    }

    pos = content.lastIndexOf("main.cvd database is up-to-date (");
    if (pos != -1){
        value = content.mid(pos + 32,content.indexOf("\n",pos + 32) - (pos + 32));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","MainVersion",value);
    }

    pos = content.lastIndexOf("daily.cvd updated (");
    if (pos != -1){
        value = content.mid(pos + 18,content.indexOf("\n",pos + 18) - (pos + 18));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","DailyVersion",value);
    }

    pos = content.lastIndexOf("daily.cld updated (");
    if (pos != -1){
        value = content.mid(pos + 18,content.indexOf("\n",pos + 18) - (pos + 18));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","DailyVersion",value);
    }

    pos = content.lastIndexOf("daily.cld database is up-to-date (");
    if (pos != -1){
        value = content.mid(pos + 33,content.indexOf("\n",pos + 33) - (pos + 33));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","DailyVersion",value);
    }

    pos = content.lastIndexOf("bytecode.cvd database is up-to-date (");
    if (pos != -1){
        value = content.mid(pos + 36,content.indexOf("\n",pos + 36) - (pos + 36));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","BytecodeVersion",value);
    }

    pos = content.lastIndexOf("bytecode.cvd updated (");
    if (pos != -1){
        value = content.mid(pos + 21,content.indexOf("\n",pos + 21) - (pos + 21));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","BytecodeVersion",value);
    }

    pos = content.lastIndexOf("Database updated");
    if (pos != -1){
        value = content.mid(pos,content.indexOf("\n",pos) - (pos));
        setupFile->setSectionValue("Updater","DatabaseFrom",value);
    }

    setUpdaterInfo();
    emit systemStatusChanged();

    ui->deamonLogText->clear();
    QStringList m_lines = content.split("\n");
    foreach(QString line,m_lines){
        ui->deamonLogText->insertPlainText(line + "\n");
        ui->deamonLogText->ensureCursorVisible();
    }
}

void freshclamsetter::slot_updateFileWatcherTriggered(){
QFile m_file(QDir::homePath() + "/.clamav-gui/update.log");
QString m_content;
QString m_value;
int pos;

    if (m_file.exists() == true) {
      m_file.open(QIODevice::ReadOnly);
      QTextStream m_stream(&m_file);
      m_content = m_stream.readAll();
      m_file.close();
    }

    pos = m_content.lastIndexOf("ClamAV update process started at");
    if (pos != -1){
        m_value = m_content.mid(pos + 33,m_content.indexOf("\n",pos + 33) - (pos + 33));
        setupFile->setSectionValue("Updater","LastUpdate",m_value);
    }

    pos = m_content.lastIndexOf("main.cvd updated");
    if (pos != -1){
        m_value = m_content.mid(pos + 17,m_content.indexOf("\n",pos + 17) - (pos + 17));
        m_value.replace("(","");
        m_value.replace(")","");
        setupFile->setSectionValue("Updater","MainVersion",m_value);
    }

    pos = m_content.lastIndexOf("daily.cvd updated");
    if (pos != -1){
        m_value = m_content.mid(pos + 18,m_content.indexOf("\n",pos + 18) - (pos + 18));
        m_value.replace("(","");
        m_value.replace(")","");
        setupFile->setSectionValue("Updater","DailyVersion",m_value);
    }

    pos = m_content.lastIndexOf("bytecode.cvd updated");
    if (pos != -1){
        m_value = m_content.mid(pos + 21,m_content.indexOf("\n",pos + 21) - (pos + 21));
        m_value.replace("(","");
        m_value.replace(")","");
        setupFile->setSectionValue("Updater","BytecodeVersion",m_value);
    }

    pos = m_content.lastIndexOf("Database updated");
    if (pos != -1){
        m_value = m_content.mid(pos,m_content.indexOf("\n",pos) - (pos));
        setupFile->setSectionValue("Updater","DatabaseFrom",m_value);
    }

    emit systemStatusChanged();

    ui->logPlainText->setPlainText("");
    QStringList m_lines = m_content.split("\n");
    foreach(QString line,m_lines){
        ui->logPlainText->insertPlainText(line + "\n");
        ui->logPlainText->ensureCursorVisible();
    }
}

void freshclamsetter::setForm(bool mode){
    ui->clearLogButton->setEnabled(mode);
    ui->deamonClearLogButton->setEnabled(mode);
    ui->deamonClearLogButton->setEnabled(mode);
    ui->startStopDeamonButton->setEnabled(mode);
    ui->updateNowButton->setEnabled(mode);
    ui->updateSettingsGroupBox->setEnabled(mode);
}

void freshclamsetter::slot_clearDeamonLogButtonClicked(){
QFile m_file(QDir::homePath() + "/.clamav-gui/freshclam.log");

    ui->deamonLogText->setPlainText("");
    updateLogFileWatcher->removePath(QDir::homePath() + "/.clamav-gui/freshclam.log");
    m_file.remove();
    m_file.open(QIODevice::ReadWrite);
    QTextStream m_stream(&m_file);
    m_stream << "";
    m_file.close();
    m_file.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ReadOther|QFileDevice::WriteOther);
    updateLogFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/freshclam.log");

}

void freshclamsetter::setUpdaterInfo(){
    QString m_htmlCode = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>";
    m_htmlCode = m_htmlCode + "<table><tr><td width='200'>";
    if (setupFile->getSectionValue("Updater","DatabaseFrom") != "") {
        m_htmlCode = m_htmlCode + tr("Database origin : </td><td>") + setupFile->getSectionValue("Updater","DatabaseFrom") + "</td></tr><tr><td>";
    }
    if (setupFile->getSectionValue("Updater","LastUpdate") != "") {
        m_htmlCode = m_htmlCode + tr("Last Update : </td><td>") + setupFile->getSectionValue("Updater","LastUpdate") + "</td></tr><tr><td>";
    }
    if (setupFile->getSectionValue("Updater","MainVersion") != "") {
        m_htmlCode = m_htmlCode + tr("Main File : </td><td>") + setupFile->getSectionValue("Updater","MainVersion") + "</td></tr><tr><td>";
    }
    if (setupFile->getSectionValue("Updater","DailyVersion") != "") {
        m_htmlCode = m_htmlCode + tr("Daily File : </td><td>") + setupFile->getSectionValue("Updater","DailyVersion") + "</td></tr><tr><td>";
    }
    if (setupFile->getSectionValue("Updater","BytecodeVersion") != "") {
        m_htmlCode = m_htmlCode + tr("ByteCode File : </td><td>") + setupFile->getSectionValue("Updater","BytecodeVersion") + "</td></tr></table>";
    }
    m_htmlCode = m_htmlCode + "</body></html>";
    ui->updateInfoText->setHtml(m_htmlCode);
}

QString freshclamsetter::extractPureNumber(QString value){
    QString m_rc = "";
    QString m_validator = "0.123456789";
    bool m_isValid = true;
    int m_index = 0;
    value = value.trimmed();
    QString m_character;

    while ((m_index < value.length()) && (m_isValid == true)) {
        m_character = value.mid(m_index,1);
        if (m_validator.indexOf(m_character) != -1) m_rc += m_character; else m_isValid = false;
        m_index++;
    }

    return m_rc;
}

QString freshclamsetter::selectColor(QString color)
{
    QString m_rc = "";

    if (monochrome == true) {
        m_rc = css_mono;
    } else {
        if (color == "mono") m_rc = css_mono;
        if (color == "red") m_rc = css_red;
        if (color == "green") m_rc = css_green;
    }

    return m_rc;
}

void freshclamsetter::slot_updaterHasOutput(){
    static QString m_oldLine;
    QString m_output = updater->readAll();
    int m_start = m_output.lastIndexOf("]") + 1;
    int m_end = m_output.lastIndexOf("[");
    QString m_line = m_output.mid(m_start,m_end-m_start-1);
    if (m_line != "") {
        QStringList m_values = m_line.split("/");
        if (m_values.size() == 2) {
            QString m_maxValueString = m_values.at(1);
            QString m_valueString = m_values.at(0);
            m_maxValueString = extractPureNumber(m_maxValueString);
            m_valueString = extractPureNumber(m_valueString);
            double m_valueMax = m_maxValueString.toDouble();
            double m_value = m_valueString.toDouble();
            busyLabel->setProgressBarMaxValue(m_valueMax);
            busyLabel->setProgressBarValue(m_value);
        }
    }
    m_line="";
    if ((m_output.indexOf("Testing database:") > -1) && (m_oldLine != "Testing Database")) m_line = "Testing Database";
    if ((m_output.indexOf("bytecode database available for download") != -1) && (m_oldLine != "Downloading bytecode.cvd")) m_line = "Downloading bytecode.cvd";
    if ((m_output.indexOf("main database available for download") != -1) && (m_oldLine != "Downloading main.cvd")) m_line = "Downloading main.cvd";
    if ((m_output.indexOf("daily database available for download") != -1) && (m_oldLine != "Downloading daily.cvd")) m_line = "Downloading daily.cvd";
    if (m_line != "") {
        busyLabel->setText(m_line);
        m_oldLine = m_line;
    }
}

void freshclamsetter::slot_startDeamonProcessFinished(int exitCode,QProcess::ExitStatus exitStatus){
    if ((exitCode != 0) || (exitStatus == QProcess::CrashExit)) freshclamStartupCounter = 0;
    if (exitCode == 0){
        ui->startStopDeamonButton->setText(tr("Deamon running - stop deamon"));
        ui->startStopDeamonButton->setStyleSheet(selectColor("green"));
        ui->updateNowButton->setStyleSheet(selectColor("green"));
//        if (monochrome == false) ui->frame->setStyleSheet("background-color:#c0c0c0;"); else ui->frame->setStyleSheet("");

        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/Clam.png"));
        pidFileWatcher->addPath(pidFile);
        logFileWatcher->addPath(logFile);
        slot_logFileWatcherTriggered();
        pidFileWatcher->addPath(pidFile);
        checkDaemonRunning();
    } else {
        pidFile = "";
        ui->startStopDeamonButton->setText(tr("Deamon not running - start deamon"));
        ui->startStopDeamonButton->setStyleSheet(selectColor("red"));
        ui->updateNowButton->setStyleSheet(selectColor("green"));
//        if (monochrome == false) ui->frame->setStyleSheet("background-color:#c0c0c0;"); else ui->frame->setStyleSheet("");

        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/Clam.png"));
        setupFile->setSectionValue("Freshclam","Pid","n/a");
        emit systemStatusChanged();
    }
}

void freshclamsetter::initFreshclamSettings(){
    QStringList m_parameters;
    QFile m_tempFile;

    lockFreshclamConf = true;
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    if (setupFile->keywordExists("FreshClam","runasroot") == true) ui->runasrootCheckBox->setChecked(setupFile->getSectionBoolValue("FreshClam","runasroot")); else {
        setupFile->setSectionValue("FreshClam","runasroot","true");
        ui->runasrootCheckBox->setChecked(true);
    }

    if (setupFile->keywordExists("Freshclam","StartDaemon") == true) ui->autoStartDaemonCheckBox->setChecked(setupFile->getSectionBoolValue("Freshclam","StartDaemon")); else {
        setupFile->setSectionValue("Freshclam","StartDaemon",false);
        ui->autoStartDaemonCheckBox->setChecked(false);
    }

    getDBUserProcess = new QProcess(this);
    connect(getDBUserProcess,SIGNAL(finished(int)),this,SLOT(slot_getDBUserProcessFinished()));

    freshclamLocationProcess = new QProcess(this);
    connect(freshclamLocationProcess,SIGNAL(finished(int)),this,SLOT(slot_freshclamLocationProcessFinished()));
    connect(freshclamLocationProcess,SIGNAL(readyRead()),this,SLOT(slot_freshclamLocationProcessHasOutput()));

    clamscanLocationProcess = new QProcess(this);
    connect(clamscanLocationProcess,SIGNAL(finished(int)),this,SLOT(slot_clamscanLocationProcessFinished()));
    connect(clamscanLocationProcess,SIGNAL(readyRead()),this,SLOT(slot_clamscanLocationProcessHasOutput()));

    freshclamConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/freshclam.conf",this);
    if (freshclamConf->singleLineExists("DatabaseDirectory") == true) {
        ui->databaseDirectoryPathLabel->setText(freshclamConf->getSingleLineValue("DatabaseDirectory"));
    } else {
        QDir m_tempdir;
        if ((m_tempdir.exists("/var/lib/clamav") == true) && ((m_tempFile.exists("/var/lib/clamav/freshclam.dat") == true))){
            freshclamConf->setSingleLineValue("DatabaseDirectory","/var/lib/clamav");
            ui->runasrootCheckBox->setChecked(true);
            setupFile->setSectionValue("FreshClam","runasroot",true);
        } else {
            freshclamConf->setSingleLineValue("DatabaseDirectory",QDir::homePath() + "/.clamav-gui/signatures");
            ui->runasrootCheckBox->setChecked(false);
            setupFile->setSectionValue("FreshClam","runasroot",false);
        }
        ui->databaseDirectoryPathLabel->setText(freshclamConf->getSingleLineValue("DatabaseDirectory"));
    }

    if (freshclamConf->singleLineExists("LogSyslog") == true) ui->logSysLogComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogSyslog")); else {
        freshclamConf->setSingleLineValue("LogSyslog","no");
        ui->logSysLogComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogSyslog"));
    }

    if (freshclamConf->singleLineExists("LogFacility") == true) ui->logFacilityComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogFacility")); else {
        freshclamConf->setSingleLineValue("LogFacility","LOG_LOCAL6");
        ui->logFacilityComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogFacility"));
    }

    if (freshclamConf->singleLineExists("LogTime") == true) ui->logTimeComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogTime")); else {
        freshclamConf->setSingleLineValue("LogTime","no");
        ui->logTimeComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogTime"));
    }

    if (freshclamConf->singleLineExists("LogRotate") == true) ui->logRotateComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogRotate")); else {
        freshclamConf->setSingleLineValue("LogRotate","no");
        ui->logRotateComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogRotate"));
    }

    if (freshclamConf->singleLineExists("PidFile") == true) ui->pidFilePathLabel->setText(freshclamConf->getSingleLineValue("PidFile")); else {
        freshclamConf->setSingleLineValue("PidFile","/tmp/freshclam.pid");
        ui->pidFilePathLabel->setText(freshclamConf->getSingleLineValue("PidFile"));
    }

    if (freshclamConf->singleLineExists("DatabaseOwner") == true) ui->databaseOwnerLineEdit->setText(freshclamConf->getSingleLineValue("DatabaseOwner")); else {
        freshclamConf->setSingleLineValue("DatabaseOwner","clamav");
        ui->databaseOwnerLineEdit->setText(freshclamConf->getSingleLineValue("DatabaseOwner"));
    }

    if (freshclamConf->singleLineExists("DatabaseMirror") == true) ui->databaseMirrorLineEdit->setText(freshclamConf->getSingleLineValue("DatabaseMirror")); else {
        freshclamConf->setSingleLineValue("DatabaseMirror","database.clamav.net");
        ui->databaseMirrorLineEdit->setText(freshclamConf->getSingleLineValue("DatabaseMirror"));
    }

    if (freshclamConf->singleLineExists("LogVerbose") == true) ui->logVerboseComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogVerbose")); else {
        freshclamConf->setSingleLineValue("LogVerbose","no");
        ui->logVerboseComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogVerbose"));
    }

    if (freshclamConf->singleLineExists("Checks") == true) ui->checkPerDaySpinBox->setValue(freshclamConf->getSingleLineValue("Checks").toInt()); else {
        freshclamConf->setSingleLineValue("Checks","12");
        ui->checkPerDaySpinBox->setValue(freshclamConf->getSingleLineValue("Checks").toInt());
    }

    ui->freshclamLocationLineEdit->setText(setupFile->getSectionValue("FreshclamSettings","FreshclamLocation"));
    ui->databaseTypeComboBox->setCurrentIndex(setupFile->getSectionIntValue("FreshClam","DataBaseToUpdate"));

    if (freshclamConf->singleLineExists("HTTPProxyServer") == true) ui->httpProxyServerLineEdit->setText(freshclamConf->getSingleLineValue("HTTPProxyServer"));
    if (freshclamConf->singleLineExists("HTTPProxyPort") == true) ui->httpProxyPortLineEdit->setText(freshclamConf->getSingleLineValue("HTTPProxyPort"));
    if (freshclamConf->singleLineExists("HTTPProxyUsername") == true) ui->httpProxyUsernameLineEdit->setText(freshclamConf->getSingleLineValue("HTTPProxyUsername"));
    if (freshclamConf->singleLineExists("HTTPProxyPassword") == true) ui->httpProxyPasswordLineEdit->setText(freshclamConf->getSingleLineValue("HTTPProxyPassword"));
    if (freshclamConf->singleLineExists("OnUpdateExecute") == true) ui->onUpdateExecuteLineEdit->setText(freshclamConf->getSingleLineValue("OnUpdateExecute"));
    if (freshclamConf->singleLineExists("OnErrorExecute") == true) ui->onErrorExecuteLineEdit->setText(freshclamConf->getSingleLineValue("OnErrorExecute"));
    if (freshclamConf->singleLineExists("OnOutdatedExecute") == true) ui->onOutdatedExecuteLineEdit->setText(freshclamConf->getSingleLineValue("OnOutdatedExecute"));

    m_parameters << "-ld" << ui->databaseDirectoryPathLabel->text();
    QDir m_dbDir;

    if (m_dbDir.exists(ui->databaseDirectoryPathLabel->text()) == true) {
        getDBUserProcess->start("ls",m_parameters);
    }

    lockFreshclamConf = false;
    clamscanlocationProcessOutput = "";
    freshclamlocationProcessOutput = "";

    m_parameters.clear();
    m_parameters << "clamscan";
    clamscanLocationProcess->start("whereis",m_parameters);

    m_parameters.clear();
    m_parameters << "freshclam";
    freshclamLocationProcess->start("whereis",m_parameters);
}

void freshclamsetter::slot_runasrootCheckBoxChanged(){
    setupFile->setSectionValue("FreshClam","runasroot",ui->runasrootCheckBox->isChecked());
}

void freshclamsetter::slot_writeFreshclamSettings(){
    if (lockFreshclamConf == false) {
        freshclamConf->setSingleLineValue("DatabaseDirectory",ui->databaseDirectoryPathLabel->text());
        freshclamConf->setSingleLineValue("LogSyslog",ui->logSysLogComboBox->currentText());
        freshclamConf->setSingleLineValue("LogFacility",ui->logFacilityComboBox->currentText());
        freshclamConf->setSingleLineValue("LogRotate",ui->logRotateComboBox->currentText());
        freshclamConf->setSingleLineValue("PidFile",ui->pidFilePathLabel->text());
        freshclamConf->setSingleLineValue("DatabaseOwner",ui->databaseOwnerLineEdit->text());
        freshclamConf->setSingleLineValue("DatabaseMirror",ui->databaseMirrorLineEdit->text());
        freshclamConf->setSingleLineValue("LogTime",ui->logTimeComboBox->currentText());
        freshclamConf->setSingleLineValue("LogVerbose",ui->logVerboseComboBox->currentText());

        if (ui->onErrorExecuteLineEdit->text() != "") freshclamConf->setSingleLineValue("OnErrorExecute",ui->onErrorExecuteLineEdit->text()); else {
            freshclamConf->setSingleLineValue("OnErrorExecute","obsolete");
            freshclamConf->removeSingleLine("OnErrorExecute","obsolete");
        }

        if (ui->onUpdateExecuteLineEdit->text() != "") freshclamConf->setSingleLineValue("OnUpdateExecute",ui->onUpdateExecuteLineEdit->text()); else {
            freshclamConf->setSingleLineValue("OnUpdateExecute","obsolete");
            freshclamConf->removeSingleLine("OnUpdateExecute","obsolete");
        }

        if (ui->onOutdatedExecuteLineEdit->text() != "") freshclamConf->setSingleLineValue("OnOutdatedExecute",ui->onOutdatedExecuteLineEdit->text()); else {
            freshclamConf->setSingleLineValue("OnOutdatedExecute","obsolete");
            freshclamConf->removeSingleLine("OnOutdatedExecute","obsolete");
        }

        if (ui->httpProxyServerLineEdit->text() != "") freshclamConf->setSingleLineValue("HTTPProxyServer",ui->httpProxyServerLineEdit->text()); else {
            freshclamConf->setSingleLineValue("HTTPProxyServer","obsolete");
            freshclamConf->removeSingleLine("HTTPProxyServer","obsolete");
        }

        if (ui->httpProxyPortLineEdit->text() != "") freshclamConf->setSingleLineValue("HTTPProxyPort",ui->httpProxyPortLineEdit->text()); else {
            freshclamConf->setSingleLineValue("HTTPProxyPort","obsolete");
            freshclamConf->removeSingleLine("HTTPProxyPort","obsolete");
        }

        if (ui->httpProxyUsernameLineEdit->text() != "") freshclamConf->setSingleLineValue("HTTPProxyUsername",ui->httpProxyUsernameLineEdit->text()); else {
            freshclamConf->setSingleLineValue("HTTPProxyUsername","obsolete");
            freshclamConf->removeSingleLine("HTTPProxyUsername","obsolete");
        }

        if (ui->httpProxyPasswordLineEdit->text() != "") freshclamConf->setSingleLineValue("HTTPProxyPassword",ui->httpProxyPasswordLineEdit->text()); else {
            freshclamConf->setSingleLineValue("HTTPProxyPassword","obsolete");
            freshclamConf->removeSingleLine("HTTPProxyPassword","obsolete");
        }

        setupFile->setSectionValue("FreshClam","UpdatesPerDay",ui->checkPerDaySpinBox->value());
        setupFile->setSectionValue("FreshClam","DataBaseToUpdate",ui->databaseTypeComboBox->currentIndex());
    }
}

void freshclamsetter::slot_dbPathChanged(QString dbPath){
    if (dbPath != "") {
        freshclamConf->setSingleLineValue("DatabaseDirectory",dbPath);
        ui->databaseDirectoryPathLabel->setText(dbPath);

        QStringList m_parameters;
        m_parameters << "-ld" << ui->databaseDirectoryPathLabel->text();
        QDir m_dbDir;
        if (m_dbDir.exists(ui->databaseDirectoryPathLabel->text()) == true) {
            getDBUserProcess->start("ls",m_parameters);
        } else {
            freshclamConf->setSingleLineValue("DatabaseOwner","clamav");
        }
    }
}

void freshclamsetter::slot_getDBUserProcessFinished(){
    QString m_output = getDBUserProcess->readAll();
    QStringList m_values = m_output.split(" ");
    if (m_values.size() > 1) {
        freshclamConf->setSingleLineValue("DatabaseOwner",m_values[2]);
        ui->databaseOwnerLineEdit->setText(m_values[2]);
    }
}

void freshclamsetter::slot_pidFileSelectButtonClicked()
{
    QString m_rc = QFileDialog::getExistingDirectory(this,tr("Select Folder for the PID-File"),"/tmp");
    if (m_rc != "") {
        freshclamConf->setSingleLineValue("PidFile",m_rc + "/freshclam.pid");
        setupFile->setSectionValue("Freshclam","PidFile",m_rc + "/freshclam.pid");
        ui->pidFilePathLabel->setText(m_rc + "/freshclam.pid");
    }
}

void freshclamsetter::slot_freshclamLocationProcessFinished(){
    freshclamlocationProcessOutput = freshclamlocationProcessOutput + freshclamLocationProcess->readAll();
    if (freshclamlocationProcessOutput.length() > 13) {
        QStringList m_values = freshclamlocationProcessOutput.split(" ");
        if (m_values.size() > 1) {
            if (m_values.length() > 0) setupFile->setSectionValue("FreshclamSettings","FreshclamLocation",m_values[1].replace("\n","")); else setupFile->setSectionValue("FreshclamSettings","FreshclamLocation","not found");
            ui->freshclamLocationLineEdit->setText(m_values[1]);
            emit systemStatusChanged();
            QFile m_file(ui->databaseDirectoryPathLabel->text() + "/freshclam.dat");
            if (m_file.exists() == false) {
                if (QMessageBox::warning(this,tr("Virus definitions missing!"),ui->databaseDirectoryPathLabel->text() + "\n" + tr("No virus definitions found in the database folder. Should the virus definitions be downloaded?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes) {
                    emit updateDatabase();
                }
            }
        }
    } else {
        setupFile->setSectionValue("FreshclamSettings","FreshclamLocation","not found");
        ui->freshclamLocationLineEdit->setText("not found");
        QMessageBox::warning(this,"WARNING","Freshclam is missing. Please install!",QMessageBox::Ok);
        setForm(false);        //emit disableUpdateButtons();
        emit quitApplication();
    }
}

void freshclamsetter::slot_clamscanLocationProcessFinished(){
    clamscanlocationProcessOutput = clamscanlocationProcessOutput + clamscanLocationProcess->readAll();
    if (clamscanlocationProcessOutput.length() < 13) {
        QMessageBox::warning(this,"ERROR","Clamav is missing. Please install!",QMessageBox::Ok);
        emit quitApplication();
    }
}

void freshclamsetter::slot_freshclamLocationProcessHasOutput(){
    freshclamlocationProcessOutput = freshclamlocationProcessOutput + freshclamLocationProcess->readAll();
}

void freshclamsetter::slot_clamscanLocationProcessHasOutput(){
    clamscanlocationProcessOutput = clamscanlocationProcessOutput + clamscanLocationProcess->readAll();
}

void freshclamsetter::slot_setFreshclamsettingsFrameState(bool state){
    ui->freshclamLocationLineEdit->setEnabled(state);
    ui->pidFileSelectPushButton->setEnabled(state);
    ui->pidFilePathLabel->setEnabled(state);
    ui->databaseOwnerLineEdit->setEnabled(state);
    ui->databaseMirrorLineEdit->setEnabled(state);
    ui->logSysLogComboBox->setEnabled(state);
    ui->logFacilityComboBox->setEnabled(state);
    ui->logRotateComboBox->setEnabled(state);
    ui->logTimeComboBox->setEnabled(state);
    ui->logVerboseComboBox->setEnabled(state);
    ui->databaseDirectoryPathLabel->setEnabled(state);
    ui->databaseTypeComboBox->setEnabled(state);
    ui->checkPerDaySpinBox->setEnabled(state);
    ui->runasrootCheckBox->setEnabled(state);
    ui->autoStartDaemonCheckBox->setEnabled(state);
    ui->httpProxyServerLineEdit->setEnabled(state);
    ui->httpProxyPortLineEdit->setEnabled(state);
    ui->httpProxyUsernameLineEdit->setEnabled(state);
    ui->httpProxyPasswordLineEdit->setEnabled(state);
    ui->onUpdateExecuteLineEdit->setEnabled(state);
    ui->onUpdateExecutePushButton->setEnabled(state);
    ui->onErrorExecuteLineEdit->setEnabled(state);
    ui->onErrorExecutePushButton->setEnabled(state);
    ui->onOutdatedExecuteLineEdit->setEnabled(state);
    ui->onOutdatedExecutePushButton->setEnabled(state);
}

void freshclamsetter::slot_autoStartDaemon(){
    setupFile->setSectionValue("Freshclam","StartDaemon",ui->autoStartDaemonCheckBox->isChecked());
}

void freshclamsetter::slot_onUpdateExecuteButtonClicked(){
    QString m_rc = QFileDialog::getOpenFileName(this,tr("On Update Execute"),tr("Select a programm that will be executed when the database is updated."));
    if (m_rc != "") ui->onUpdateExecuteLineEdit->setText(m_rc);
    slot_writeFreshclamSettings();
}

void freshclamsetter::slot_onErrorExecuteButtonClicked(){
    QString m_rc = QFileDialog::getOpenFileName(this,tr("On Error Execute"),tr("Select a programm that will be executed when an error occured."));
    if (m_rc != "") ui->onErrorExecuteLineEdit->setText(m_rc);
    slot_writeFreshclamSettings();
}

void freshclamsetter::slot_onOutdatedExecuteButtonClicked(){
    QString m_rc = QFileDialog::getOpenFileName(this,tr("On Outdated Execute"),tr("Select a programm that will be executed when the database is outdated."));
    if (m_rc != "") ui->onOutdatedExecuteLineEdit->setText(m_rc);
    slot_writeFreshclamSettings();
}

void freshclamsetter::slot_processWatcherExpired(){
    QString m_freshclamPid = setupFile->getSectionValue("Freshclam","Pid");

    QDir m_checkDir;
    if ( m_freshclamPid != "n/a") {
        if (m_checkDir.exists("/proc/" + m_freshclamPid) == false) {
            setupFile->setSectionValue("Freshclam","Pid","n/a");
            emit systemStatusChanged();
            checkDaemonRunning();
        }
    }
}

void freshclamsetter::slot_add_remove_highlighter(bool state)
{
    if (state == true) {
      if (updateLogHighLighter != NULL) {
        delete updateLogHighLighter;
        delete freshclamLogHighLighter;
        updateLogHighLighter = NULL;
        freshclamLogHighLighter = NULL;
        ui->groupBox->setStyleSheet("");
        ui->updateInfoText->setStyleSheet("");
        ui->updateNowButton->setStyleSheet(css_mono);
      }
      monochrome = true;
    } else {
         if (updateLogHighLighter == NULL) {
             updateLogHighLighter = new highlighter(ui->logPlainText->document());
             freshclamLogHighLighter = new highlighter(ui->deamonLogText->document());
             ui->groupBox->setStyleSheet(css_mono);
             ui->updateInfoText->setStyleSheet(css_mono);
         } else {
             delete updateLogHighLighter;
             delete freshclamLogHighLighter;
             updateLogHighLighter = new highlighter(ui->logPlainText->document());
             freshclamLogHighLighter = new highlighter(ui->deamonLogText->document());
             ui->groupBox->setStyleSheet(css_mono);
             ui->updateInfoText->setStyleSheet(css_mono);
         }
         ui->startStopDeamonButton->setStyleSheet(css_green);
         ui->updateNowButton->setStyleSheet(css_green);
         monochrome = false;
    }
    checkDaemonRunning();
}
