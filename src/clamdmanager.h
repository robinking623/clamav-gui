#ifndef CLAMDMANAGER_H
#define CLAMDMANAGER_H

#include <QFileSystemWatcher>
#include <QFileDialog>
#include <QProcess>
#include <QWidget>
#include <QTimer>
#include <QMovie>
#include "clamdconfstringoption.h"
#include "clamdconfspinboxoption.h"
#include "clamdconfcomboboxoption.h"
#include "setupfilehandler.h"
#include "highlighter.h"
#include "translator.h"

namespace Ui {
class clamdManager;
}

class clamdManager : public QWidget
{
    Q_OBJECT

public:
    explicit clamdManager(QWidget *parent = 0);
    ~clamdManager();
    setupFileHandler    * setupFile; // clamd && freshclam
    setupFileHandler    * clamdConf; // clamd
    setupFileHandler    * freshclamConf;
    QProcess            * clamdLocationProcess; // clamd
    QProcess            * clamonaccLocationProcess; // clamd
    QProcess            * startClamdProcess; // clamd
    QProcess            * findclamonaccProcess; // clamd
    QProcess            * restartClamonaccProcess; // clamd
    QProcess            * killProcess;
    QString               clamdLocation; // clamd
    QString               clamonaccLocation; // clamd
    QString               clamonaccPid; // clamd
    QString               lastFound; // clamd
    QString               sudoGUI;
    QTimer              * startDelayTimer;
    QTimer              * processWatcher;
    highlighter         * logHighlighter; // clamd
    QFileSystemWatcher  * clamdLogWatcher; // clamd
    QFileSystemWatcher  * clamdPidWatcher; // clamd
    int                   dirsUnderMonitoring;
    bool                  clamdRestartInProgress; // clamd
    bool                  monochrome;
    QString               trimLocationOutput(QString);
    bool                  startup;
    bool                  waitForFreshclam;
    bool                  initprocessrunning;
    int                   clamdStartupCounter;
    void initClamdSettings(); // clamd
    void restartClamonacc(); // clamd
    void initClamdConfElements();
    bool checkClamdRunning(); // clamd
    QString selectColor(QString color);

private slots:
    void slot_updateClamdConf(); // ab hier --> clamd
    void slot_logFileContentChanged();
    void slot_clamdStartStopButtonClicked();
    void slot_pidWatcherTriggered();
    void slot_startClamdProcessFinished(int exitCode,QProcess::ExitStatus exitStatus);
    void slot_killClamdProcessFinished();
    void slot_findclamonaccProcessFinished(int rc);
    void slot_monitoringAddButtonClicked();
    void slot_monitoringDelButtonClicked();
    void slot_restartClamonaccProcessFinished();
    void slot_restartClamdButtonClicked();
    void slot_clamdSettingsChanged();
    void slot_clamdLocationProcessFinished();
    void slot_clamonaccLocationProcessFinished();
    void slot_startClamdOnStartupCheckBoxClicked();
    void slot_startDelayTimerExpired();
    void slot_waitForFreshclamStarted();
    void slot_processWatcherExpired();
    void slot_add_remove_highlighter(bool);
signals:
    void setBallonMessage(int, QString,QString); // clamd
    void setActiveTab();
    void systemStatusChanged();

private:
    Ui::clamdManager *ui;
};

#endif // CLAMDMANAGER_H
