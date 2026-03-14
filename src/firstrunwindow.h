#ifndef FIRSTRUNWINDOW_H
#define FIRSTRUNWINDOW_H

#include <QProcess>
#include <QDialog>
#include <QDir>
#include "setupfilehandler.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class firstRunWindow;
}
QT_END_NAMESPACE

class firstRunWindow : public QDialog
{
    Q_OBJECT

public:
    firstRunWindow(QWidget *parent = nullptr);
    ~firstRunWindow();

private:
    Ui::firstRunWindow *ui;
    QStringList           initCommands;
    QStringList           initParameters;
    QStringList           processParameters;
    QProcess            * initProcess;
    int                   initIndex;
    bool                  processInit;
    setupFileHandler    * setupFile;
    setupFileHandler    * m_clamdConf;
    setupFileHandler    * m_freshclamConf;
    void createBaseDirStructur();
    void createServiceMenu();
    void createInitialSettings();
    void createClamdConfFile();
    void createFreshclamConfFile();

private slots:
    void slot_initProcessFinished();
    void slot_sddComboBoxChanged();
    void slot_done();

signals:
    void doneit();
    void settingChanged();
    void quitApplication();
};
#endif // FIRSTRUNWINDOW_H
