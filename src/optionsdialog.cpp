#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#define css "background-color:#404040;color:white"

optionsDialog::optionsDialog(QWidget *parent) : QWidget(parent), ui(new Ui::optionsDialog){
    ui->setupUi(this);
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    updateDirectories();
    ui->tabWidget->setTabText(0,tr("Options"));
    ui->tabWidget->setTabText(1,tr("Directories"));
    scanLimits = new scanLimitsTab(this);
    ui->tabWidget->addTab(scanLimits,QIcon(":/icons/icons/scanlimits.png"),tr("Scan Limitations"));
    incExcOptTab = new includeExcludeOptions(this);

    connect(incExcOptTab,SIGNAL(updateClamdConf()),this,SLOT(slot_updateClamdConf()));
    connect(scanLimits,SIGNAL(updateClamdConf()),this,SLOT(slot_updateClamdConf()));

    ui->tabWidget->addTab(incExcOptTab,QIcon(":/icons/icons/includeexclude.png"),tr("Include/Exclude"));
    ui->tabWidget->setIconSize(QSize(24,24));

    getClamscanParametersProcess = new QProcess(this);
    connect(getClamscanParametersProcess,SIGNAL(readyReadStandardError()),this,SLOT(slot_getClamscanProcessHasOutput()));
    connect(getClamscanParametersProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(slot_getClamscanProcessHasOutput()));
    connect(getClamscanParametersProcess,SIGNAL(finished(int)),this,SLOT(slot_getClamscanProcessFinished()));

    QStringList m_parameters;
    m_parameters << "--help";

    getClamscanParametersProcess->start("clamscan",m_parameters);
}

optionsDialog::~optionsDialog(){
    delete ui;
}

void optionsDialog::slot_updateClamdConf(){
    emit updateClamdConf();
}

void optionsDialog::slot_getClamscanProcessHasOutput(){
    getClamscanProcessOutput = getClamscanProcessOutput + getClamscanParametersProcess->readAll();
}

void optionsDialog::slot_getClamscanProcessFinished(){
    QString m_excludeList =  "--help|--version|--database|--log|--file-list|--move|--copy|--exclude|--exclude-dir|--include|--include-dir|";
    m_excludeList += "--bytecode-timeout|--statistics|--exclude-pua|--include-pua|--structured-ssn-format|--structured-ssn-count|--structured-cc-count|--tempdir|--follow-dir-symlinks|";
    m_excludeList += "--structured-cc-mode|--max-scantime|--max-filesize|--max-scansize|--max-files|--max-recursion|--max-dir-recursion|--max-embeddedpe|--max-htmlnormalize|--follow-file-symlinks|";
    m_excludeList += "--max-htmlnotags|--max-scriptnormalize|--max-ziptypercg|--max-partitions|--max-iconspe|--max-rechwp3|--pcre-match-limit|--pcre-recmatch-limit|--pcre-max-filesize|";
    m_excludeList += "--fail-if-cvd-older-than=days";
    getClamscanProcessOutput = getClamscanProcessOutput + getClamscanParametersProcess->readAll();

    QString m_value;
    QString m_line;
    QString m_keyword;
    QString m_parameter;
    int m_commentStart = 0;
    QStringList m_lines = getClamscanProcessOutput.split("\n");
    QStringList m_comments;
    QString m_comment;

    setupFile->removeSection("AvailableOptions");
    setupFile->removeSection("OtherKeywords");

    QString m_commentSum = "";
    for (int x = 0; x < m_lines.size(); x++) {
        m_line = m_lines[x];
        m_line = m_line.trimmed();
        if (m_line.indexOf("Clam AntiVirus:") != -1) {
            QString m_version = m_line.mid(m_line.indexOf("Clam AntiVirus:") + 15);
            if ((m_version.indexOf("Scanner 1.4.1") != -1) || (m_version.indexOf("Scanner 1.0.7") != -1)) {
              QFile m_ca_certFile("/etc/pki/tls/certs/ca-bundle.crt");
              QString m_message = "WARNING\nThe file \"/etc/pki/tls/certs/ca-bundle.crt\" is missing!\nVersion 1.0.7 and 1.4.1 are known to require this file in this specific locaction and without it \"freshclam\" will not work.\nInstall the appropriate package for your distribution.";
              QFile m_ca_alternative_location ("/etc/ssl/ca-bundle.pem");
              if (m_ca_alternative_location.exists() == true) m_message = "WARNING\nThe file \"/etc/pki/tls/certs/ca-bundle.crt\" is missing!\nVersion 1.0.7 and 1.4.1 are known to require this file in this specific locaction and without it \"freshclam\" will not work.\n\nFound the file in the location \"/etc/ssl/ca-bundle.pem\".\n\nDo the following command as root (sudo) to fix this issue:\n\nln -s /etc/ssl/ca-bundle.pem /etc/pki/tls/certs/ca-bundle.crt";
              m_ca_alternative_location.setFileName("/et/ssl/certs/ca-certificates.crt");
              if (m_ca_alternative_location.exists() == true) m_message = "WARNING\nThe file \"/etc/pki/tls/certs/ca-bundle.crt\" is missing!\nVersion 1.0.7 and 1.4.1 are known to require this file in this specific locaction and without it \"freshclam\" will not work.\n\nFound the file in the location \"/etc/ssl/ca-bundle.pem\".\n\nDo the following command as root (sudo) to fix this issue:\n\nln -s /etc/ssl/certs/ca-certificates.crt /etc/pki/tls/certs/ca-bundle.crt";
              if (m_ca_certFile.exists() == false) QMessageBox::warning(this,"WARNING",m_message);
            }
            ui->versionLabel->setText("ClamAV Version:" + m_version);
            setupFile->setSectionValue("Updater","Version",m_version);
        }

        if (m_line.indexOf("--") == 0) {
            m_commentStart = m_line.indexOf(" ");
            m_keyword = m_line.left(m_commentStart);
            m_parameter = "";
            if (m_keyword.indexOf("[=") != -1) {
                m_keyword = m_keyword.left(m_keyword.indexOf("["));
                int start = m_line.indexOf("[") + 2;
                int m_length = m_line.indexOf("]") - start;
                m_parameter = m_line.mid(start,m_length);
            } else {
                m_keyword = m_keyword.left(m_keyword.indexOf("="));
            }
            if (m_excludeList.indexOf(m_keyword) == -1) {
                if (m_value == "") m_value = m_keyword; else m_value = m_value + "\n" + m_keyword;
                if (m_parameter != "") {
                    m_value = m_value + "=" + m_parameter;
                }
                m_comment = m_line.mid(m_commentStart);
                m_comment = m_comment.trimmed();
                while (m_comment.indexOf("  ") != -1) m_comment = m_comment.replace("  "," ");
                if (m_comment.left(1) == "-") {
                    m_comment = m_comment.mid(m_comment.indexOf(" ") + 1);
                }
                m_comment = tr(m_comment.toLocal8Bit());
                m_comments.append(m_comment);
                m_commentSum == ""? m_commentSum = m_comment:m_commentSum = m_commentSum + "|" + m_comment;
                if ((m_parameter == "yes/no(*)") ||(m_parameter == "yes(*)/no")) {
                    if (m_parameter == "yes/no(*)") {
                        setupFile->setSectionValue("AvailableOptions",m_keyword + "<equal>no",m_comment);
                        setupFile->setSectionValue("AvailableOptions",m_keyword + "<equal>yes",m_comment);
                    } else {
                        setupFile->setSectionValue("AvailableOptions",m_keyword + "<equal>yes",m_comment);
                        setupFile->setSectionValue("AvailableOptions",m_keyword + "<equal>no",m_comment);
                    }
                } else {
                    setupFile->setSectionValue("AvailableOptions",m_keyword,m_comment);
                }
            } else {
                setupFile->setSectionValue("OtherKeywords",m_keyword,"exists");
            }
        }
    }

    /*For Debuggin-Reasons only*/
    /*QFile m_tempfile("/home/wusel/m_parameters.txt");
    QStringList m_commentList = m_commentSum.split("|");
    if (m_tempfile.open(QIODevice::WriteOnly|QIODevice::Text)){
        QTextStream m_stream(&m_tempfile);
        for (int i = 0; i < m_commentList.m_length(); i++) {
            m_stream << "base = base + \"" << m_commentList.at(i) << "|\";" << "\n";
        }
    }
    m_tempfile.close();*/

    QStringList m_parameters = m_value.split("\n");
    scanoption * m_option;
    scanoptionyn * m_optionyn;
    QString m_label;
    QString m_yes_no;
    bool m_flipflop = false;

    for (int x = 0; x < m_parameters.length(); x++) {
        m_label = m_parameters[x];
        if ((m_label.indexOf("yes(*)/no") != -1) || (m_label.indexOf("yes/no(*)") != -1)){
            m_label.indexOf("yes/no(*)") == -1?m_yes_no="yes":m_yes_no="no";
            m_label = m_label.left(m_label.indexOf("="));
            if (setupFile->keywordExists("SelectedOptions",m_label+"<equal>no") == true) {
                m_label = m_label + "<equal>no";
            } else {
                if (setupFile->keywordExists("SelectedOptions",m_label+"<equal>yes") == true) {
                    m_label = m_label + "<equal>yes";
                } else {
                    if (m_yes_no == "yes") {
                        m_label = m_label + "<equal>yes";
                    } else {
                        m_label = m_label + "<equal>no";
                    }
                }
            }
        }

        if (setupFile->keywordExists("SelectedOptions",m_label.replace("=","<equal>")) == true) {
            if (m_label.indexOf("<equal>") == -1) {
                m_option = new scanoption(nullptr, QDir::homePath() + "/.clamav-gui/settings.ini","SelectedOptions",true,m_label,m_comments[x]);
                connect(m_option,SIGNAL(valuechanged()),this,SLOT(slot_updateClamdConf()));
                if (m_flipflop == false) {
                    ui->optionLayout->addWidget(m_option);
                    m_flipflop = true;
                } else {
                    ui->optionLayout_2->addWidget(m_option);
                    m_flipflop = false;
                }
            } else {
                m_optionyn = new scanoptionyn(nullptr, QDir::homePath() + "/.clamav-gui/settings.ini","SelectedOptions",true,m_label,m_comments[x]);
                connect(m_optionyn,SIGNAL(valuechanged()),this,SLOT(slot_updateClamdConf()));
                if (m_flipflop == false) {
                    ui->optionLayout->addWidget(m_optionyn);
                    m_flipflop = true;
                } else {
                    ui->optionLayout_2->addWidget(m_optionyn);
                    m_flipflop = false;
                }
            }
        } else {
            if (m_label.indexOf("<equal>") == -1) {
                m_option = new scanoption(nullptr, QDir::homePath() + "/.clamav-gui/settings.ini","SelectedOptions",false,m_label,m_comments[x]);
                connect(m_option,SIGNAL(valuechanged()),this,SLOT(slot_updateClamdConf()));
                if (m_flipflop == false) {
                    ui->optionLayout->addWidget(m_option);
                    m_flipflop = true;
                } else {
                    ui->optionLayout_2->addWidget(m_option);
                    m_flipflop = false;
                }
            } else {
                m_optionyn = new scanoptionyn(nullptr, QDir::homePath() + "/.clamav-gui/settings.ini","SelectedOptions",false,m_label,m_comments[x]);
                connect(m_optionyn,SIGNAL(valuechanged()),this,SLOT(slot_updateClamdConf()));
                if (m_flipflop == false) {
                    ui->optionLayout->addWidget(m_optionyn);
                    m_flipflop = true;
                } else {
                    ui->optionLayout_2->addWidget(m_optionyn);
                    m_flipflop = false;
                }
            }
        }
    }
}

QString optionsDialog::getCopyDirectory(){
    return ui->copyDirectoryLineEdit->text();
}

QString optionsDialog::getMoveDirectory(){
    return ui->moveDirectoryLineEdit->text();
}

void optionsDialog::slot_selectLVDButtonClicked(){
QString m_rc;
    m_rc = QFileDialog::getExistingDirectory(this,tr("Select Directory"),QDir::homePath() + "/.clamav-gui/signatures",QFileDialog::ShowDirsOnly);
    if (m_rc != "") {
        ui->loadVirusDatabaseLineEdit->setText(m_rc);
        writeDirectories();
        emit systemStatusChanged();
        if (ui->loadVirusDatabaseCheckBox->isChecked() == true) {
            emit databasePathChanged(m_rc);
            QFile file(m_rc + "/freshclam.dat");
            if (file.exists() == false) {
                if (QMessageBox::warning(this,tr("Virus definitions missing!"),tr("No virus definitions found in the database folder. Should the virus definitions be downloaded?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes) {
                    emit updateDatabase();
                }
            }
        }
    }
}

void optionsDialog::slot_selectCDButtonClicked(){
QString m_rc;
    m_rc = QFileDialog::getExistingDirectory(this,tr("Select Directory"),QDir::homePath() + "/.clamav-gui/quarantine");
    if (m_rc != "") ui->copyDirectoryLineEdit->setText(m_rc);
    writeDirectories();
}

void optionsDialog::slot_selectMDButtonClicked(){
QString m_rc;
    m_rc = QFileDialog::getExistingDirectory(this,tr("Select Directory"),QDir::homePath() + "/.clamav-gui/quarantine");
    if (m_rc != "") ui->moveDirectoryLineEdit->setText(m_rc);

    writeDirectories();
}

void optionsDialog::slot_selectSRTFButtonClicked(){
QString m_rc;
    m_rc = QFileDialog::getSaveFileName(this,tr("Select File"),QDir::homePath() + "/.clamav-gui/logs");
    if (m_rc != "") ui->scanReportToFileLineEdit->setText(m_rc);
    writeDirectories();
}

void optionsDialog::slot_selectSFFButtonClicked(){
QString m_rc;
    m_rc = QFileDialog::getOpenFileName(this,tr("Select File"),QDir::homePath());
    if (m_rc != "") ui->scanFilesFromFileLineEdit->setText(m_rc);
    writeDirectories();
}

void optionsDialog::slot_selectTDButtonClicked(){
QString m_rc;
    m_rc = QFileDialog::getExistingDirectory(this,tr("Select Directory"),QDir::homePath());
    if (m_rc != "") ui->tempFilesLineEdit->setText(m_rc);
    writeDirectories();
}

void optionsDialog::updateDirectories(){
QString m_value;
QString m_checked;
QStringList m_keywords;

    m_keywords << "LoadSupportedDBFiles" << "ScanReportToFile" << "ScanFilesFromFile" << "TmpFile" << "MoveInfectedFiles";
    m_keywords << "CopyInfectedFiles" << "FollowDirectorySymLinks" << "FollowFileSymLinks";

    for (int i = 0; i < m_keywords.length(); i++){
        m_value=setupFile->getSectionValue("Directories",m_keywords.at(i));
        m_checked = m_value.left(m_value.indexOf("|"));
        m_value = m_value.mid(m_value.indexOf("|") + 1);

        if (m_checked == "m_checked"){
            switch (i) {
                case 0 : ui->loadVirusDatabaseCheckBox->setChecked(true);
                         ui->databaseFrame->setStyleSheet(css);
                         ui->loadVirusDatabaseLineEdit->setEnabled(true);
                         ui->selectLVDButton->setEnabled(true);
                         ui->loadVirusDatabaseLineEdit->setText(m_value);
                            break;
                case 1 : ui->scanReportToFileCheckBox->setChecked(true);
                         ui->scanReportFrame->setStyleSheet(css);
                         ui->scanReportToFileLineEdit->setEnabled(true);
                         ui->selectSCRFButton->setEnabled(true);
                         ui->scanReportToFileLineEdit->setText(m_value);
                            break;
                case 2 : ui->scanFilesFromFileCheckBox->setChecked(true);
                         ui->scanFileFrame->setStyleSheet(css);
                         ui->scanFilesFromFileLineEdit->setEnabled(true);
                         ui->selectSFFButton->setEnabled(true);
                         ui->scanFilesFromFileLineEdit->setText(m_value);
                            break;
                case 3 : ui->tempFileCheckBox->setChecked(true);
                         ui->tempFileFrame->setStyleSheet(css);
                         ui->tempFilesLineEdit->setEnabled(true);
                         ui->selectTFButton->setEnabled(true);
                         ui->tempFilesLineEdit->setText(m_value);
                            break;
                case 4 : ui->moveDirectoryCheckBox->setChecked(true);
                         ui->moveFrame->setStyleSheet(css);
                         ui->moveDirectoryLineEdit->setEnabled(true);
                         ui->selectMDButton->setEnabled(true);
                         ui->moveDirectoryLineEdit->setText(m_value);
                            break;
                case 5 : ui->copyDirectoryCheckBox->setChecked(true);
                         ui->copyFrame->setStyleSheet(css);
                         ui->copyDirectoryLineEdit->setEnabled(true);
                         ui->selectCFButton->setEnabled(true);
                         ui->copyDirectoryLineEdit->setText(m_value);
                            break;
                case 6 : ui->followDirectorySymlinksCheckBox->setChecked(true);
                         ui->followDirLinksFrame->setStyleSheet(css);
                         ui->followDirectorySymlinksComboBox->setEnabled(true);
                         ui->followDirectorySymlinksComboBox->setCurrentIndex(m_value.toInt());
                            break;
                case 7 : ui->followFileSymlinksCheckBox->setChecked(true);
                         ui->followFileLinksFrame->setStyleSheet(css);
                         ui->followFileSymlinksComboBox->setEnabled(true);
                         ui->followFileSymlinksComboBox->setCurrentIndex(m_value.toInt());
                            break;
            }
        } else {
            switch (i) {
                case 0 : ui->loadVirusDatabaseCheckBox->setChecked(false);
                         ui->databaseFrame->setStyleSheet("");
                         ui->loadVirusDatabaseLineEdit->setEnabled(false);
                         ui->selectLVDButton->setEnabled(false);
                         ui->loadVirusDatabaseLineEdit->setText(m_value);
                            break;
                case 1 : ui->scanReportToFileCheckBox->setChecked(false);
                         ui->scanReportFrame->setStyleSheet("");
                         ui->scanReportToFileLineEdit->setEnabled(false);
                         ui->selectSCRFButton->setEnabled(false);
                         ui->scanReportToFileLineEdit->setText(m_value);
                            break;
                case 2 : ui->scanFilesFromFileCheckBox->setChecked(false);
                         ui->scanFileFrame->setStyleSheet("");
                         ui->scanFilesFromFileLineEdit->setEnabled(false);
                         ui->selectSFFButton->setEnabled(false);
                         ui->scanFilesFromFileLineEdit->setText(m_value);
                            break;
                case 3 : ui->tempFileCheckBox->setChecked(false);
                         ui->tempFileFrame->setStyleSheet("");
                         ui->tempFilesLineEdit->setEnabled(false);
                         ui->selectTFButton->setEnabled(false);
                         ui->tempFilesLineEdit->setText(m_value);
                            break;
                case 4 : ui->moveDirectoryCheckBox->setChecked(false);
                         ui->moveFrame->setStyleSheet("");
                         ui->moveDirectoryLineEdit->setEnabled(false);
                         ui->selectMDButton->setEnabled(false);
                         ui->moveDirectoryLineEdit->setText(m_value);
                            break;
                case 5 : ui->copyDirectoryCheckBox->setChecked(false);
                         ui->copyFrame->setStyleSheet("");
                         ui->copyDirectoryLineEdit->setEnabled(false);
                         ui->selectCFButton->setEnabled(false);
                         ui->copyDirectoryLineEdit->setText(m_value);
                            break;
                case 6 : ui->followDirectorySymlinksCheckBox->setChecked(false);
                         ui->followDirLinksFrame->setStyleSheet("");
                         ui->followDirectorySymlinksComboBox->setEnabled(false);
                         ui->followDirectorySymlinksComboBox->setCurrentIndex(m_value.toInt());
                            break;
                case 7 : ui->followFileSymlinksCheckBox->setChecked(false);
                         ui->followFileLinksFrame->setStyleSheet("");
                         ui->followFileSymlinksComboBox->setEnabled(false);
                         ui->followFileSymlinksComboBox->setCurrentIndex(m_value.toInt());
                            break;
            }
        }
    }
}

void optionsDialog::writeDirectories(){
QString m_keyword;
QString m_value;
QString m_checked;

    m_keyword = "LoadSupportedDBFiles";
    m_value = ui->loadVirusDatabaseLineEdit->text();
    ui->loadVirusDatabaseCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
    setupFile->setSectionValue("Directories",m_keyword,m_checked + "|" + m_value);

    m_keyword = "ScanReportToFile";
    m_value = ui->scanReportToFileLineEdit->text();
    ui->scanReportToFileCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
    setupFile->setSectionValue("Directories",m_keyword,m_checked + "|" + m_value);

    m_keyword = "ScanFilesFromFile";
    m_value = ui->scanFilesFromFileLineEdit->text();
    ui->scanFilesFromFileCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
    setupFile->setSectionValue("Directories",m_keyword,m_checked + "|" + m_value);

    m_keyword = "TmpFile";
    m_value = ui->tempFilesLineEdit->text();
    ui->tempFileCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
    setupFile->setSectionValue("Directories",m_keyword,m_checked + "|" + m_value);

    m_keyword = "MoveInfectedFiles";
    m_value = ui->moveDirectoryLineEdit->text();
    ui->moveDirectoryCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
    setupFile->setSectionValue("Directories",m_keyword,m_checked + "|" + m_value);

    m_keyword = "CopyInfectedFiles";
    m_value = ui->copyDirectoryLineEdit->text();
    ui->copyDirectoryCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
    setupFile->setSectionValue("Directories",m_keyword,m_checked + "|" + m_value);

    m_keyword = "FollowDirectorySymLinks";
    m_value = QString::number(ui->followDirectorySymlinksComboBox->currentIndex());
    ui->followDirectorySymlinksCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
    setupFile->setSectionValue("Directories",m_keyword,m_checked + "|" + m_value);

    m_keyword = "FollowFileSymLinks";
    m_value = QString::number(ui->followFileSymlinksComboBox->currentIndex());
    ui->followFileSymlinksCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
    setupFile->setSectionValue("Directories",m_keyword,m_checked + "|" + m_value);

    if (ui->loadVirusDatabaseCheckBox->isChecked() == true) {
        if (ui->loadVirusDatabaseLineEdit->text() != "") {
            emit databasePathChanged(ui->loadVirusDatabaseLineEdit->text());
            QFile m_file(ui->loadVirusDatabaseLineEdit->text() + "/main.cvd");
            if (m_file.exists() == false) {
                if (QMessageBox::warning(this,tr("Database files missing!"),tr("The virus definition files are missing in the database directory. Start download of the missing files?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes) {
                    emit updateDatabase();
                }
            }
        }
    } else {
        emit databasePathChanged("/var/lib/clamav");
    }
    updateDirectories();
}

void optionsDialog::slot_updateDirectories(){
    updateDirectories();
}

void optionsDialog::slot_logViewerButtonClicked(){
QFile m_file;

    if ((ui->scanReportToFileCheckBox->isChecked() == true) & (ui->scanReportToFileLineEdit->text() != "")){
        m_file.setFileName(ui->scanReportToFileLineEdit->text());
        if (m_file.exists()){
            logViewObject * m_logView = new logViewObject(this,ui->scanReportToFileLineEdit->text());
            m_logView->setModal(true);
            m_logView->showMaximized();
        }
    }
}
