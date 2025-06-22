#include "logviewerobject.h"
#include "ui_logviewerobject.h"

logViewerObject::logViewerObject(QWidget *parent) : QWidget(parent), ui(new Ui::logViewerObject){
    setupfile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    ui->setupUi(this);
    slot_profilesChanged();
}

logViewerObject::~logViewerObject(){
    delete ui;
}

void logViewerObject::slot_profilesChanged(){
QStringList m_profiles = setupfile->getKeywords("Profiles");
QStringList m_profilesWithLog;
QStringList m_values;
QString m_actualProfileName = ui->profileComboBox->currentText();
setupFileHandler sf(this);

    foreach(QString profile,m_profiles){
        sf.setSetupFileName(QDir::homePath() + "/.clamav-gui/profiles/" + profile + ".ini");
        //setupFileHandler * sf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/profiles/" + profile + ".ini");
        m_values = sf.getSectionValue("Directories","ScanReportToFile").split("|");
        if (m_values.count() == 2){
            QFile m_tempFile(m_values[1]);
            if (m_tempFile.exists()) m_profilesWithLog << profile;
        }
    }

    ui->profileComboBox->clear();
    ui->profileComboBox->addItems(m_profilesWithLog);
    if (ui->profileComboBox->findText(m_actualProfileName) != -1){
        ui->profileComboBox->setCurrentIndex(ui->profileComboBox->findText(m_actualProfileName));
        loadLogFile(m_actualProfileName);
    } else {
        loadLogFile(ui->profileComboBox->currentText());
    }
}

void logViewerObject::loadLogFile(QString profile){
setupFileHandler * m_sf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/profiles/" + profile + ".ini",this);
setupFileHandler * m_sf2 = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
bool m_css = m_sf2->getSectionBoolValue("Setup","DisableLogHighlighter");
QString m_buffer;
QStringList m_logs;
QString tabHeader;
QStringList m_values;

    while(ui->logTab->count() > 0){
        ui->logTab->removeTab(0);
    }

    m_values = m_sf->getSectionValue("Directories","ScanReportToFile").split("|");
    if (m_values.count() == 2){
        QFile m_file(m_values[1]);
        logFileName = m_values[1];
        if (m_file.open(QIODevice::ReadOnly)){
            QTextStream m_stream(&m_file);
            m_buffer = m_stream.readAll();
            m_logs = m_buffer.split("<Scanning startet>");
            for (int i = 1; i < m_logs.count(); i++){
                partialLogObject * m_log = new partialLogObject(this,m_logs[i],m_css);
                connect(this,SIGNAL(logHighlightingChanged(bool)),m_log,SLOT(slot_add_remove_highlighter(bool)));
                tabHeader = m_logs[i].mid(1,m_logs[i].indexOf("\n") - 1);
                ui->logTab->insertTab(0,m_log,QIcon(":/icons/icons/information.png"),tabHeader);
                //ui->logTab->addTab(m_log,QIcon(":/icons/icons/information.png"),tabHeader);
            }
            ui->logTab->setCurrentIndex(0);
            m_file.close();
        }
    }
}

void logViewerObject::slot_profileSeclectionChanged(){
    loadLogFile(ui->profileComboBox->currentText());
}

void logViewerObject::saveLog(){
QString m_logText;
partialLogObject * m_log;

    QFile m_logFile(logFileName);
    if (m_logFile.open(QIODevice::ReadWrite|QIODevice::Truncate|QIODevice::Text)){
        QTextStream m_stream(&m_logFile);
        for (int i = 0; i < ui->logTab->count(); i++){
            m_log = (partialLogObject*)ui->logTab->widget(i);
            m_logText = m_logText + m_log->getLogText();
        }
        m_stream << m_logText;
        m_logFile.close();
    }
}

void logViewerObject::slot_clearLogButtonClicked(){
int m_currentTab= ui->logTab->currentIndex();

    if (m_currentTab > -1){
        if (QMessageBox::question(this,tr("Clear Log"),tr("Do you realy want to remove this partial log?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes){
            ui->logTab->removeTab(m_currentTab);
            saveLog();
        }
    }
}

void logViewerObject::slot_clearAllButtonClicked(){
int m_count = ui->logTab->count();

    if ((m_count > 0) && (QMessageBox::question(this,tr("Clear Log"),tr("Do you realy want to remove the complete log?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)){
        for (int i = 0; i < m_count; i++){
            ui->logTab->removeTab(ui->logTab->currentIndex());
        }
    }
    saveLog();
}

void logViewerObject::slot_add_remove_highlighter(bool state)
{
    emit logHighlightingChanged(state);
}
