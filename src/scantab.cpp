#include "scantab.h"
#include "ui_scantab.h"

scanTab::scanTab(QWidget *parent) : QWidget(parent),ui(new Ui::scanTab){
    ui->setupUi(this);
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    logHighLighter = NULL;
    monochrome = setupFile->getSectionBoolValue("Setup","DisableLogHighlighter");
    if (monochrome == false) logHighLighter = new highlighter(ui->logPlainTextEdit->document());
    devicelabel = ui->deviceLabel;
    checkMonochromeSettings();

    model = new CFileSystemModel;
    model->setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);

    ui->treeView->setModel(model);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);

    QStringList m_directories = setupFile->getSectionValue("Settings","Directories").split("\n");
    ui->treeView->collapseAll();
    model->unCheckAll();
    model->setRootPath("/");
    foreach(QString dir,m_directories){
        if (dir != ""){
            model->setChecked(dir,true);
            QModelIndex index = model->index(dir);
            ui->treeView->scrollTo(index);
            ui->treeView->setCurrentIndex(index);
        }
    }

    ui->recursivCheckBox->setChecked(setupFile->getSectionBoolValue("Settings","RecursivScan"));
    ui->showHiddenDirsCheckBox->setChecked(setupFile->getSectionBoolValue("Settings","ShowHiddenDirs"));
    slot_hiddenFoldersCheckBoxClicked();
    ui->virusFoundComboBox->setCurrentIndex(setupFile->getSectionIntValue("Settings","VirusFoundComboBox"));

    whoamiProcess = new QProcess(this);
    connect(whoamiProcess,SIGNAL(finished(int)),this,SLOT(slot_whoamiProcessFinished()));
    whoamiProcess->start("whoami",QStringList());
}

scanTab::~scanTab()
{
    delete ui;
}

void scanTab::checkMonochromeSettings()
{
    if (monochrome == false) {
        devicelabel->setStyleSheet("background-color:#c0c0c0;color:black;padding:4px;border-radius:5px;");
        ui->pathLabel->setStyleSheet("background-color:#c0c0c0;color:black;padding:4px;border-radius:5px;");
    } else {
        devicelabel->setStyleSheet("background-color:#404040;color:white;padding:4px;border-radius:5px;");
        ui->pathLabel->setStyleSheet("background-color:#404040;color:white;padding:4px;border-radius:5px;");
    }
}

void scanTab::slot_scanButtonClicked(){
QList <QPersistentModelIndex> m_list = model->checkedIndexes.values();
QStringList m_scanObjects;

    for (int i = 0; i < m_list.count(); i++){
        if (m_list[i].data(QFileSystemModel::FilePathRole).toString() != "") m_scanObjects << m_list[i].data(QFileSystemModel::FilePathRole).toString();
    }

    if (m_scanObjects.count() > 0) emit triggerScanRequest(m_scanObjects);
}

void scanTab::slot_basePathButtonClicked(){
QModelIndex m_index = model->index("/");

    ui->treeView->scrollTo(m_index);
    ui->treeView->setCurrentIndex(m_index);
}

void scanTab::slot_homePathButtonClicked(){
QModelIndex m_index = model->index(QDir::homePath());

    ui->treeView->scrollTo(m_index);
    ui->treeView->setCurrentIndex(m_index);
}

void scanTab::slot_updateDeviceList(){
QDir m_dir("/run/media/" + username.trimmed());
QDir m_dir2("/run/media");
QDir m_dirUbuntu("/media/" + username.trimmed());
QDir m_dirUbuntu2("/media");
fileSystemWatcher = new QFileSystemWatcher(this);
fileSystemWatcherUbuntu = new QFileSystemWatcher(this);
QStringList m_filters;
    m_filters << "*";
QStringList m_dirs = m_dir.entryList(m_filters,QDir::AllDirs|QDir::NoDotAndDotDot);
QStringList m_dirsUbuntu = m_dirUbuntu.entryList(m_filters,QDir::AllDirs|QDir::NoDotAndDotDot);
devicelabel = new QLabel(tr("Devices"));
QLayoutItem *m_item = NULL;

    while ((m_item = ui->devicesFrame->layout()->takeAt(0)) != 0) {
        delete m_item->widget();
    }

    if (m_dir.exists() == true){
        fileSystemWatcher->addPath("/run/media/" + username.trimmed());
        connect(fileSystemWatcher,SIGNAL(directoryChanged(QString)),this,SLOT(slot_updateDeviceList()));
    } else {
        if (m_dir2.exists() == true) {
            fileSystemWatcher->addPath("/run/media");
            connect(fileSystemWatcher,SIGNAL(directoryChanged(QString)),this,SLOT(slot_updateDeviceList()));
        }
    }

    if (m_dirUbuntu.exists() == true) {
        fileSystemWatcherUbuntu->addPath("/media/" + username.trimmed());
        connect(fileSystemWatcherUbuntu,SIGNAL(directoryChanged(QString)),this,SLOT(slot_updateDeviceList()));
    } else {
        if (m_dirUbuntu2.exists() == true) {
            fileSystemWatcherUbuntu->addPath("/media");
            connect(fileSystemWatcherUbuntu,SIGNAL(directoryChanged(QString)),this,SLOT(slot_updateDeviceList()));
        }
    }

    if (setupFile->getSectionBoolValue("Setup","DisableLogHighlighter") == true) {
        devicelabel->setStyleSheet("background-color:#404040;color:white;padding:3px;");
    } else {
        devicelabel->setStyleSheet("background-color:#c0c0c0;color:black;padding:3px;");
    }
    ui->devicesFrame->layout()->addWidget(devicelabel);

    deviceGroup = new QButtonGroup(this);
    connect(deviceGroup,SIGNAL(idClicked(int)),this,SLOT(slot_deviceButtonClicked(int)));

    if (m_dirs.count() > 0){
        devices.clear();
        buttonID = 0;
        foreach (QString entry,m_dirs){
            devices << (QString)("/run/media/" + username.trimmed() + "/" + entry);
            dragablePushButton * m_button = new dragablePushButton(QIcon(":/icons/icons/media.png"),entry.mid(entry.lastIndexOf("/") + 1),this,(QString)("/var/run/media/" + username.trimmed() + "/" + entry));
            connect(m_button,SIGNAL(dragStarted()),this,SLOT(slot_requestDropZoneVisible()));
            m_button->setIconSize(QSize(28,28));
            m_button->setStyleSheet("text-align:left");
            m_button->setFlat(true);
            deviceGroup->addButton(m_button,buttonID);
            ui->devicesFrame->layout()->addWidget(m_button);
            buttonID++;
        }
    }

    if (m_dirsUbuntu.count() > 0){
        devices.clear();
        buttonID = 0;
        foreach (QString entry,m_dirsUbuntu){
            devices << (QString)("/media/" + username.trimmed() + "/" + entry);
            dragablePushButton * m_button = new dragablePushButton(QIcon(":/icons/icons/media.png"),entry.mid(entry.lastIndexOf("/") + 1),this,(QString)("/media/" + username.trimmed() + "/" + entry));
            connect(m_button,SIGNAL(dragStarted()),this,SLOT(slot_requestDropZoneVisible()));
            m_button->setIconSize(QSize(28,28));
            m_button->setStyleSheet("text-align:left");
            m_button->setFlat(true);
            deviceGroup->addButton(m_button,buttonID);
            ui->devicesFrame->layout()->addWidget(m_button);
            buttonID++;
        }
    }
}

void scanTab::slot_deviceButtonClicked(int buttonIndex){
QModelIndex m_index = model->index(devices.at(buttonIndex));

    ui->treeView->scrollTo(m_index);
    ui->treeView->setCurrentIndex(m_index);
}

void scanTab::slot_whoamiProcessFinished(){

    username = whoamiProcess->readAllStandardOutput();

    slot_updateDeviceList();
}

void scanTab::slot_recursivScanCheckBoxClicked(){
    setupFile->setSectionValue("Settings","RecursivScan",ui->recursivCheckBox->isChecked());
}

void scanTab::slot_virusFoundComboBoxChanged(){
    setupFile->setSectionValue("Settings","VirusFoundComboBox",ui->virusFoundComboBox->currentIndex());
}

bool scanTab::recursivChecked(){
    return ui->recursivCheckBox->isChecked();
}

void scanTab::setStatusMessage(QString message){
QString m_currentFile;
int m_start,m_end;

    while(message.indexOf("Scanning") != -1){
        m_start = message.indexOf("Scanning");
        m_end = message.indexOf("\n",m_start);
        m_currentFile = message.mid(m_start,m_end - m_start + 1);
        message.replace(m_currentFile,"");
        m_currentFile = m_currentFile.mid(m_currentFile.lastIndexOf("/") + 1);
        m_currentFile.replace("\n","");
        ui->currentFileLabel->setText("Scanning : " + m_currentFile);
    }
    ui->logPlainTextEdit->insertPlainText(message);
    ui->logPlainTextEdit->ensureCursorVisible();
}

void scanTab::clearLogMessage(){
    ui->logPlainTextEdit->setPlainText("");
}

void scanTab::slot_abortScan(){
    ui->currentFileLabel->setText(tr("Scanning aborted ......"));
    emit abortScan();
}

void scanTab::slot_enableForm(bool mode){

    if (mode == false){
        busyLabel = new QLabel(this);
        busyLabel->setStyleSheet("background:transparent");
        busyLabel->setGeometry((this->width() - 80) / 2,(this->height() - 80) / 2,80,80);
        movie = new QMovie(":/icons/icons/gifs/spinning_segments_large.gif");
        busyLabel->setMovie(movie);
        busyLabel->show();
        movie->start();
    } else {
        if (movie != 0){
            movie->stop();
            delete movie;
            delete busyLabel;
        }
    }

    ui->deviceFrame->setEnabled(mode);
    ui->startScanButton->setEnabled(mode);
    ui->stopScanButton->setEnabled(!mode);
    ui->recursivCheckBox->setEnabled(mode);
    ui->showHiddenDirsCheckBox->setEnabled(mode);
    ui->virusFoundComboBox->setEnabled(mode);
    ui->treeView->setEnabled(mode);
}

void scanTab::setStatusBarMessage(QString message, QString bgColor){
    ui->currentFileLabel->setText(message);
    ui->currentFileLabel->setStyleSheet("background-color:"+bgColor);
}

int scanTab::getVirusFoundComboBoxValue(){
    return ui->virusFoundComboBox->currentIndex();
}

void scanTab::slot_requestDropZoneVisible(){
    emit requestDropZoneVisible();
}

void scanTab::slot_dirtreeSelectionChanged(){
QList <QPersistentModelIndex> m_list = model->checkedIndexes.values();
QString m_directories;

    for (int i = 0; i < m_list.count(); i++){
        if (i < m_list.count() - 1){
            m_directories = m_directories + m_list[i].data(QFileSystemModel::FilePathRole).toString() + "\n";
        } else {
            m_directories = m_directories + m_list[i].data(QFileSystemModel::FilePathRole).toString();
        }
    }

    setupFile->setSectionValue("Settings","Directories",m_directories);
}

void scanTab::slot_updateDBPath(QString path){
    setupFile->setSectionValue("Directories", "LoadSupportedDBFiles", "checked|" + path);
}

void scanTab::slot_disableScanButton()
{
    ui->startScanButton->setEnabled(false);
}

void scanTab::slot_hiddenFoldersCheckBoxClicked()
{
    if (ui->showHiddenDirsCheckBox->isChecked() == true) {
        model->setFilter(QDir::AllDirs|QDir::NoDotAndDotDot|QDir::Hidden);
    } else {
        model->setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);
    }
    setupFile->setSectionValue("Settings","ShowHiddenDirs",ui->showHiddenDirsCheckBox->isChecked());
}

void scanTab::slot_add_remove_highlighter(bool state)
{
    monochrome = state;
    setupFile->setSectionValue("Setup","DisableLogHighlighter",state);

    if (state == true) {
      if (logHighLighter != NULL) {
        delete logHighLighter;
        logHighLighter = NULL;
      }
    } else {
         if (logHighLighter == NULL) {
             logHighLighter = new highlighter(ui->logPlainTextEdit->document());
         } else {
             delete logHighLighter;
             logHighLighter = new highlighter(ui->logPlainTextEdit->document());
         }
    }
    checkMonochromeSettings();
}
