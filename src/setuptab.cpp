#include "setuptab.h"
#include "ui_setuptab.h"
#define css_red "background-color:red;color:white"
#define css_yellow "background-color:yellow;color:black"
#define css_green "background-color:green;color:yellow"
#define css_mono "background-color:#404040;color:white"

setupTab::setupTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setupTab)
{
    ui->setupUi(this);

    int index = -1;

    QString langhelper;

    supressMessage = true; // verhindert, dass bei der Initialisierung der Sprachauswahl die Warnmeldung kommt.

    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    monochrome = false;
    if (setupFile->keywordExists("Setup","DisableLogHighlighter") == true) monochrome = setupFile->getSectionBoolValue("Setup","DisableLogHighlighter");

    if (setupFile->keywordExists("Setup","language") == true) {
        langhelper = setupFile->getSectionValue("Setup","language");
        index = ui->languageSelectComboBox->findText(langhelper,Qt::MatchStartsWith);
        if (index == -1) index = ui->languageSelectComboBox->findText("[en_GB]",Qt::MatchStartsWith);
        ui->languageSelectComboBox->setCurrentIndex(index);
    } else {
        QString lang = QLocale::system().name();
        index = ui->languageSelectComboBox->findText("["+lang+"]",Qt::MatchStartsWith);
        if (index == -1) index = ui->languageSelectComboBox->findText("[en_GB]",Qt::MatchStartsWith);
        ui->languageSelectComboBox->setCurrentIndex(index);
    }

    if (setupFile->keywordExists("Setup","WindowState") == true) {
        if (setupFile->getSectionValue("Setup","WindowState") == "minimized") ui->windowStateComboBox->setCurrentIndex(1); else ui->windowStateComboBox->setCurrentIndex(0);
    }

    if (setupFile->keywordExists("Clamd","ClamdScanMultithreading") == true) {
        ui->clamdscanComboBox->setCurrentIndex(setupFile->getSectionIntValue("Clamd","ClamdScanMultithreading"));
    } else {
        setupFile->setSectionValue("Clamd","ClamdScanMultithreading",0);
    }

    if (setupFile->keywordExists("Setup","DisableLogHighlighter") == true) {
        ui->logHighlighterCheckBox->setChecked(setupFile->getSectionBoolValue("Setup","DisableLogHighlighter"));
    } else {
        setupFile->setSectionValue("Setup","DisableLogHighlighter",false);
    }

    slot_updateSystemInfo();

    supressMessage = false;
}

setupTab::~setupTab(){
    delete ui;
}

QString setupTab::checkmonochrome(QString color)
{
    QString rc = "";
    if (monochrome == true) {
        rc = css_mono;
    } else {
        if (color == "red") rc = css_red;
        if (color == "yellow") rc = css_yellow;
        if (color == "green") rc = css_green;
    }

    return rc;
}

void setupTab::slot_updateSystemInfo(){
    QString systemInfo;
    if (setupFile->keywordExists("Clamd","ClamdLocation") == true) ui->clamdPath->setText(setupFile->getSectionValue("Clamd","ClamdLocation").replace("\n",""));
    if (setupFile->keywordExists("Clamd","ClamonaccLocation") == true) ui->clamonaccPath->setText(setupFile->getSectionValue("Clamd","ClamonaccLocation").replace("\n",""));
    if (setupFile->keywordExists("FreshclamSettings","FreshclamLocation") == true) ui->freshclamPath->setText(setupFile->getSectionValue("FreshclamSettings","FreshclamLocation").replace("\n",""));

    if (setupFile->sectionExists("Updater") == true) {
        ui->databasePath->setText(setupFile->getSectionValue("Directories","LoadSupportedDBFiles").mid(setupFile->getSectionValue("Directories","LoadSupportedDBFiles").indexOf("|")+1));
        ui->databaseLastUpdate->setText(setupFile->getSectionValue("Updater","LastUpdate"));
        ui->databaseMainFile->setText(setupFile->getSectionValue("Updater","MainVersion"));
        ui->databaseDailyFile->setText(setupFile->getSectionValue("Updater","DailyVersion"));
        ui->databaseBytecodeFile->setText(setupFile->getSectionValue("Updater","BytecodeVersion"));

        QString value = setupFile->getSectionValue("Updater","DailyVersion");
        QString scannerVersion = setupFile->getSectionValue("Updater","Version");
        scannerVersion = scannerVersion.replace("Scanner ","");
        value = value.mid(value.indexOf(" "), value.indexOf(",") - value.indexOf(" "));
        systemInfo = "<div style='font-size:12px;line-height:20px;'><b>Scanner: <font color='navy'>" + scannerVersion + "</font><br>Database: <font color='navy'>" + value + "</font><br>";
        systemInfo += "Date: <font color='navy'>" + setupFile->getSectionValue("Updater","LastUpdate") + "</font></b></div>";
        emit sendSystemInfo(systemInfo);
    }

    if (setupFile->keywordExists("Clamd","ClamonaccPid") == true) {
        ui->clamonaccPID->setText(setupFile->getSectionValue("Clamd","ClamonaccPid"));
        if (setupFile->getSectionValue("Clamd","ClamonaccPid") == "n/a") {
            ui->clamonaccActivityLabel->setPixmap(QPixmap(":/icons/icons/gifs/activity.gif"));
            ui->clamonaccStatus->setText(setupFile->getSectionValue("Clamd","Status2"));
            ui->clamonaccStatus->setStyleSheet(checkmonochrome("red"));
        } else {
            ui->clamonaccActivityLabel->setMovie(new QMovie(":/icons/icons/gifs/activity.gif"));
            ui->clamonaccActivityLabel->movie()->start();
            ui->clamonaccStatus->setText("is running");
            ui->clamonaccStatus->setStyleSheet(checkmonochrome("green"));
        }
    }

    if (setupFile->keywordExists("Clamd","ClamdPid") == true) {
        ui->clamdPID->setText(setupFile->getSectionValue("Clamd","ClamdPid"));
        if (setupFile->getSectionValue("Clamd","ClamdPid") == "n/a") {
            ui->clamdActivityLabel->setPixmap(QPixmap(":/icons/icons/gifs/activity.gif"));
            QString message = setupFile->getSectionValue("Clamd","Status");
            if ((message == "starting up ...") || (message == "shutting down ...")){
                ui->clamdStatus->setStyleSheet(checkmonochrome("yellow"));
                ui->clamdStatus->setText(message);
                if (setupFile->getSectionValue("Clamd","Status2") != "n/a") {
                    ui->clamonaccStatus->setStyleSheet(checkmonochrome("yellow"));
                    ui->clamonaccStatus->setText(message);
                }
            }
            if  (message == "is running") {
                ui->clamdStatus->setStyleSheet(checkmonochrome("green"));
                ui->clamdStatus->setText(message);
                if (setupFile->getSectionValue("Clamd","Status2") != "is running") {
                    ui->clamonaccStatus->setStyleSheet(checkmonochrome("green"));
                    ui->clamonaccStatus->setText(message);
                }
            }
            if  ((message == "shut down") || (message == "not running")) {
                ui->clamdStatus->setStyleSheet(checkmonochrome("red"));
                ui->clamdStatus->setText("is down");
                ui->clamonaccStatus->setStyleSheet(checkmonochrome("red"));
                ui->clamonaccStatus->setText("is down");
            }
        } else {
            ui->clamdActivityLabel->setMovie(new QMovie(":/icons/icons/gifs/activity.gif"));
            ui->clamdActivityLabel->movie()->start();
            ui->clamdStatus->setText("is running");
            ui->clamdStatus->setStyleSheet(checkmonochrome("green"));
        }
    }

    if (setupFile->keywordExists("Freshclam","Pid") == true) {
        ui->freshclamPID->setText(setupFile->getSectionValue("Freshclam","Pid"));
        if (setupFile->getSectionValue("Freshclam","Pid") == "n/a") {
            ui->freshclamActivityLabel->setPixmap(QPixmap(":/icons/icons/gifs/activity.gif"));
            ui->freshclamStatus->setText("is down");
            ui->freshclamStatus->setStyleSheet(checkmonochrome("red"));
        } else {
            ui->freshclamActivityLabel->setMovie(new QMovie(":/icons/icons/gifs/activity.gif"));
            ui->freshclamActivityLabel->movie()->start();
            ui->freshclamStatus->setText("is running");
            ui->freshclamStatus->setStyleSheet(checkmonochrome("green"));
        }
    }
}

void setupTab::slot_clamdButtonClicked(){
    emit switchActiveTab(6);
}

void setupTab::slot_freshclamButtonClicked() {
    emit switchActiveTab(5);
}

void setupTab::slot_clamdscanComboBoxClicked()
{
    setupFile->setSectionValue("Clamd","ClamdScanMultithreading",ui->clamdscanComboBox->currentIndex());
}

void setupTab::slot_logHightlighterCheckBoxClicked()
{
    setupFile->setSectionValue("Setup","DisableLogHighlighter",ui->logHighlighterCheckBox->isChecked());
    logHighlightingChanged(ui->logHighlighterCheckBox->isChecked());
    monochrome = ui->logHighlighterCheckBox->isChecked();
    slot_updateSystemInfo();
}

void setupTab::slot_clamonaccButtonClicked() {
    emit switchActiveTab(6);
}

void setupTab::slot_selectedLanguageChanged(){
    setupFile->setSectionValue("Setup","language",ui->languageSelectComboBox->currentText().mid(0,7));
    if (supressMessage == false) QMessageBox::information(this,tr("Warning"),tr("You have to restart the application for changes to take effect!"));
}

void setupTab::slot_basicSettingsChanged(){
    if (ui->windowStateComboBox->currentIndex() == 0) setupFile->setSectionValue("Setup","WindowState","maximized");
    if (ui->windowStateComboBox->currentIndex() == 1) setupFile->setSectionValue("Setup","WindowState","minimized");
}

