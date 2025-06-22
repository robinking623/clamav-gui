#include "scanlimitstab.h"
#include "ui_scanlimitstab.h"
#define css "background-color:#404040;color:white"

scanLimitsTab::scanLimitsTab(QWidget *parent) : QWidget(parent), ui(new Ui::scanLimitsTab)
{
    ui->setupUi(this);
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    if (setupFile->sectionExists("ScanLimitations") == true) updateLimits(); else writeLimits();
}

scanLimitsTab::~scanLimitsTab()
{
    delete ui;
}

void scanLimitsTab::writeLimits(){
QString m_keyword;
QString m_value;
QString m_checked;

    if (update == false) {
        QStringList m_keywords;
/*00*/        m_keywords << "Files larger than this will be skipped and assumed clean";
/*01*/        m_keywords << "The maximum amount of data to scan for each container file";
/*02*/        m_keywords << "The maximum number of files to scan for each container file";
/*03*/        m_keywords << "Maximum archive recursion level for container file";
/*04*/        m_keywords << "Maximum directory recursion level";
/*05*/        m_keywords << "Maximum size file to check for embedded PE";
/*06*/        m_keywords << "Maximum size of HTML file to normalize";
/*07*/        m_keywords << "Maximum size of normalized HTML file to scan";
/*08*/        m_keywords << "Maximum size of script file to normalize";
/*09*/        m_keywords << "Maximum size zip to type reanalyze";
/*10*/        m_keywords << "Maximum number of partitions in disk image to be scanned";
/*11*/        m_keywords << "Maximum number of icons in PE file to be scanned";
/*12*/        m_keywords << "Number of seconds to wait for waiting a response back from the stats server";
/*13*/        m_keywords << "Bytecode timeout in milliseconds";
/*14*/        m_keywords << "Collect and print execution statistics";
/*15*/        m_keywords << "Structured SSN Format";
/*16*/        m_keywords << "Structured SSN Count";
/*17*/        m_keywords << "Structured CC Count";
/*18*/        m_keywords << "Structured CC Mode";
/*19*/        m_keywords << "Max Scan-Time";
/*20*/        m_keywords << "Max recursion to HWP3 parsing function";
/*21*/        m_keywords << "Max calls to PCRE match function";
/*22*/        m_keywords << "Max recursion calls to the PCRE match function";
/*23*/        m_keywords << "Max PCRE file size";
/*24*/        m_keywords << "Database outdated if older than x days";

        for (int i = 0; i < m_keywords.length(); i++) {
            m_keyword = m_keywords.at(i);

            switch (i) {
            case 0 : m_value = QString::number(ui->filesLargerThanThisSpinBox->value()) + ui->filesLargerThanThisComboBox->currentText();
                     ui->filesLargerThanThisCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 1 : m_value = QString::number(ui->maxAmountForContainerSpinBox->value()) + ui->maxAmountForContainerComboBox->currentText();
                     ui->maxAmountForContainerCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 2 : m_value = QString::number(ui->maxNumberForContainerSpinBox->value());
                     ui->maxNumberForContainerCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 3 : m_value = QString::number(ui->maxArchiveRecursionForContainerSpinBox->value());
                     ui->maxArchiveRecursionForContainerCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 4 : m_value = QString::number(ui->maxDirRecursionLevelSpinBox->value());
                     ui->maxDirRecursionLevelCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 5 : m_value = QString::number(ui->maxSizeFileForPESpinBox->value()) + ui->maxSizeFileForPEComboBox->currentText();
                     ui->maxSizeFileForPECheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 6 : m_value = QString::number(ui->maxSizeHTMLFileToNormalizeSpinBox->value()) + ui->maxSizeHTMLFileToNormalizeComboBox->currentText();
                     ui->maxSizeHTMLFileToNormalizeCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 7 : m_value = QString::number(ui->maxSizeOfNormalizedHTMLFileSpinBox->value()) + ui->maxSizeOfNormalizedHTMLFileComboBox->currentText();
                     ui->maxSizeOfNormalizedHTMLFileCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 8 : m_value = QString::number(ui->maxSizeOfScriptFileToNormalizeSpinBox->value()) + ui->maxSizeOfScriptFileToNormalizeComboBox->currentText();
                     ui->maxSizeOfScriptFileToNormalizeCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 9 : m_value = QString::number(ui->maxSizeZipToTypeReanalzeSpinBox->value()) + ui->maxSizeZipToTypeReanalzeComboBox->currentText();
                     ui->maxSizeZipToTypeReanalzeCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 10: m_value = QString::number(ui->maxNumberOfPartitionsInDiskImageSpinBox->value());
                     ui->maxNumberOfPartitionsInDiskImageCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 11: m_value = QString::number(ui->maxNumberOfIconsInPEFileSpinBox->value());
                     ui->maxNumberOfIconsInPEFileCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 12: m_value = QString::number(ui->numberOfSecondsToWaitForResponseSpinBox->value());
                     ui->numberOfSecondsToWaitForResponseCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 13: m_value = QString::number(ui->numberOfSecondsToWaitForByteCodeSpinBox->value());
                     ui->numberOfSecondsToWaitForByteCodeCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 14: m_value = QString::number(ui->collectAndPrintExecutionStatisticsComboBox->currentIndex());
                     ui->collectAndPrintExecutionStatisticsCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 15: m_value = QString::number(ui->structuredSSNFormatComboBox->currentIndex());
                     ui->structuredSSNFormatCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 16: m_value = QString::number(ui->structuredSSNCountSpinBox->value());
                     ui->structuredSSNCountCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 17: m_value = QString::number(ui->structuredCCCountSpinBox->value());
                     ui->structuredCCCountCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 18: m_value = QString::number(ui->structuredCCModeComboBox->currentIndex());
                     ui->structuredCCModeCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 19: m_value = QString::number(ui->maxScanTimeSpinBox->value());
                     ui->maxScanTimeCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 20: m_value = QString::number(ui->maxRecursionHWP3SpinBox->value());
                     ui->maxRecursionHWP3CheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 21: m_value = QString::number(ui->maxPCREMatchSpinBox->value());
                     ui->maxPCREMatchCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 22: m_value = QString::number(ui->maxRecursionPCREMatchSpinBox->value());
                     ui->maxRecursionPCREMatchCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 23: m_value = QString::number(ui->maxPCREFileSizeSpinBox->value()) + ui->maxPCREFileSizeComboBox->currentText();
                     ui->maxPCREFileSizeCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            case 24: m_value = QString::number(ui->databaseOutdatedSpinBox->value());
                     ui->databaseOutdatedCheckBox->isChecked() == true?m_checked="m_checked":m_checked="not m_checked";
                        break;
            }
            setupFile->setSectionValue("ScanLimitations",m_keyword,m_checked + "|" + m_value);
        }

        emit updateClamdConf();

        updateLimits();
    }
}


void scanLimitsTab::updateLimits(){
QString m_value;
QString m_checked;
QStringList m_keywords;
QString m_keyword;
bool m_flag;

    update = true;

/*00*/    m_keywords << "Files larger than this will be skipped and assumed clean";
/*01*/    m_keywords << "The maximum amount of data to scan for each container file";
/*02*/    m_keywords << "The maximum number of files to scan for each container file";
/*03*/    m_keywords << "Maximum archive recursion level for container file";
/*04*/    m_keywords << "Maximum directory recursion level";
/*05*/    m_keywords << "Maximum size file to check for embedded PE";
/*06*/    m_keywords << "Maximum size of HTML file to normalize";
/*07*/    m_keywords << "Maximum size of normalized HTML file to scan";
/*08*/    m_keywords << "Maximum size of script file to normalize";
/*09*/    m_keywords << "Maximum size zip to type reanalyze";
/*10*/    m_keywords << "Maximum number of partitions in disk image to be scanned";
/*11*/    m_keywords << "Maximum number of icons in PE file to be scanned";
/*12*/    m_keywords << "Number of seconds to wait for waiting a response back from the stats server";
/*13*/    m_keywords << "Bytecode timeout in milliseconds";
/*14*/    m_keywords << "Collect and print execution statistics";
/*15*/    m_keywords << "Structured SSN Format";
/*16*/    m_keywords << "Structured SSN Count";
/*17*/    m_keywords << "Structured CC Count";
/*18*/    m_keywords << "Structured CC Mode";
/*19*/    m_keywords << "Max Scan-Time";
/*20*/    m_keywords << "Max recursion to HWP3 parsing function";
/*21*/    m_keywords << "Max calls to PCRE match function";
/*22*/    m_keywords << "Max recursion calls to the PCRE match function";
/*23*/    m_keywords << "Max PCRE file size";
/*24*/    m_keywords << "Database outdated if older than x days";

    QStringList m_switches;
    m_switches << "--max-filesize" << "--max-scansize" << "--max-files" << "--max-recursion" << "--max-dir-recursion" << "--max-embeddedpe";
    m_switches << "--max-htmlnormalize" << "--max-htmlnotags" << "--max-scriptnormalize" << "--max-ziptypercg" << "--max-partitions" << "--max-iconspe";
    m_switches << "--stats-timeout" << "--bytecode-timeout" << "--statistics" << "--structured-ssn-format" << "--structured-ssn-count" << "--structured-cc-count";
    m_switches << "--structured-cc-count" << "--structured-cc-mode" << "--max-rechwp3" << "--pcre-match-limit" << "--pcre-recmatch-limit" << "--pcre-max-filesize";
    m_switches << "--fail-if-cvd-older-than";

    for (int i = 0; i < m_keywords.length(); i++) {
        m_keyword = m_keywords.at(i);

        m_value = setupFile->getSectionValue("ScanLimitations",m_keyword);
        m_checked = m_value.left(m_value.indexOf("|"));
        m_value = m_value.mid(m_value.indexOf("|") + 1);
        m_checked=="m_checked"?m_flag=true:m_flag=false;

        switch (i) {
            case 0 : if (m_value.right(1) != ""){
                         if ((m_value.indexOf("K") != -1) || (m_value.indexOf("M") != -1)) {
                             ui->filesLargerThanThisComboBox->setCurrentIndex(ui->filesLargerThanThisComboBox->findText(m_value.right(1)));
                            m_value = m_value.left(m_value.length() - 1);
                         }
                     }
                     ui->filesLargerThanThisComboBox->setEnabled(m_flag);
                     ui->filesLargerThanThisCheckBox->setChecked(m_flag);
                     if (m_value != "") ui->filesLargerThanThisSpinBox->setValue(m_value.toInt());
                     ui->filesLargerThanThisSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame01->setStyleSheet(css):ui->frame01->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame01->setVisible(true):ui->frame01->setVisible(false);

                        break;
            case 1 : if (m_value.right(1) != ""){
                         if ((m_value.indexOf("K") != -1) || (m_value.indexOf("M") != -1)) {
                             ui->maxAmountForContainerComboBox->setCurrentIndex(ui->maxAmountForContainerComboBox->findText(m_value.right(1)));
                             m_value = m_value.left(m_value.length() - 1);
                         }
                     }
                     ui->maxAmountForContainerComboBox->setEnabled(m_flag);
                     ui->maxAmountForContainerCheckBox->setChecked(m_flag);
                     if (m_value != "") ui->maxAmountForContainerSpinBox->setValue(m_value.toInt());
                     ui->maxAmountForContainerSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame02->setStyleSheet(css):ui->frame02->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame02->setVisible(true):ui->frame02->setVisible(false);
                        break;
            case 2 : ui->maxNumberForContainerCheckBox->setChecked(m_flag);
                     ui->maxNumberForContainerSpinBox->setValue(m_value.toInt());
                     ui->maxNumberForContainerSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame03->setStyleSheet(css):ui->frame03->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame03->setVisible(true):ui->frame03->setVisible(false);
                        break;
            case 3 : ui->maxArchiveRecursionForContainerCheckBox->setChecked(m_flag);
                     ui->maxArchiveRecursionForContainerSpinBox->setValue(m_value.toInt());
                     ui->maxArchiveRecursionForContainerSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame04->setStyleSheet(css):ui->frame04->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame04->setVisible(true):ui->frame04->setVisible(false);
                        break;
            case 4 : ui->maxDirRecursionLevelCheckBox->setChecked(m_flag);
                     ui->maxDirRecursionLevelSpinBox->setValue(m_value.toInt());
                     ui->maxDirRecursionLevelSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame05->setStyleSheet(css):ui->frame05->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame05->setVisible(true):ui->frame05->setVisible(false);
                        break;
            case 5 : if (m_value.right(1) != ""){
                         if ((m_value.indexOf("K") != -1) || (m_value.indexOf("M") != -1)) {
                             ui->maxSizeFileForPEComboBox->setCurrentIndex(ui->maxSizeFileForPEComboBox->findText(m_value.right(1)));
                             m_value = m_value.left(m_value.length() - 1);
                         }
                     }
                     ui->maxSizeFileForPEComboBox->setEnabled(m_flag);
                     ui->maxSizeFileForPECheckBox->setChecked(m_flag);
                     if (m_value != "") ui->maxSizeFileForPESpinBox->setValue(m_value.toInt());
                     ui->maxSizeFileForPESpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame06->setStyleSheet(css):ui->frame06->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame06->setVisible(true):ui->frame06->setVisible(false);
                        break;
            case 6 : if (m_value.right(1) != ""){
                         if ((m_value.indexOf("K") != -1) || (m_value.indexOf("M") != -1)) {
                             ui->maxSizeHTMLFileToNormalizeComboBox->setCurrentIndex(ui->maxSizeHTMLFileToNormalizeComboBox->findText(m_value.right(1)));
                             m_value = m_value.left(m_value.length() - 1);
                         }
                     }
                     ui->maxSizeHTMLFileToNormalizeComboBox->setEnabled(m_flag);
                     ui->maxSizeHTMLFileToNormalizeCheckBox->setChecked(m_flag);
                     if (m_value != "") ui->maxSizeHTMLFileToNormalizeSpinBox->setValue(m_value.toInt());
                     ui->maxSizeHTMLFileToNormalizeSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame07->setStyleSheet(css):ui->frame07->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame07->setVisible(true):ui->frame07->setVisible(false);
                        break;
            case 7 : if (m_value.right(1) != ""){
                         if ((m_value.indexOf("K") != -1) || (m_value.indexOf("M") != -1)) {
                             ui->maxSizeOfNormalizedHTMLFileComboBox->setCurrentIndex(ui->maxSizeOfNormalizedHTMLFileComboBox->findText(m_value.right(1)));
                             m_value = m_value.left(m_value.length() - 1);
                         }
                     }
                     ui->maxSizeOfNormalizedHTMLFileComboBox->setEnabled(m_flag);
                     ui->maxSizeOfNormalizedHTMLFileCheckBox->setChecked(m_flag);
                     if (m_value != "") ui->maxSizeOfNormalizedHTMLFileSpinBox->setValue(m_value.toInt());
                     ui->maxSizeOfNormalizedHTMLFileSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame08->setStyleSheet(css):ui->frame08->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame08->setVisible(true):ui->frame08->setVisible(false);
                        break;
            case 8 : if (m_value.right(1) != ""){
                         if ((m_value.indexOf("K") != -1) || (m_value.indexOf("M") != -1)) {
                             ui->maxSizeOfScriptFileToNormalizeComboBox->setCurrentIndex(ui->maxSizeOfScriptFileToNormalizeComboBox->findText(m_value.right(1)));
                             m_value = m_value.left(m_value.length() - 1);
                         }
                     }
                     ui->maxSizeOfScriptFileToNormalizeComboBox->setEnabled(m_flag);
                     ui->maxSizeOfScriptFileToNormalizeCheckBox->setChecked(m_flag);
                     if (m_value != "") ui->maxSizeOfScriptFileToNormalizeSpinBox->setValue(m_value.toInt());
                     ui->maxSizeOfScriptFileToNormalizeSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame09->setStyleSheet(css):ui->frame09->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame09->setVisible(true):ui->frame09->setVisible(false);
                        break;
            case 9 : if (m_value.right(1) != ""){
                         if ((m_value.indexOf("K") != -1) || (m_value.indexOf("M") != -1)) {
                             ui->maxSizeZipToTypeReanalzeComboBox->setCurrentIndex(ui->maxSizeZipToTypeReanalzeComboBox->findText(m_value.right(1)));
                             m_value = m_value.left(m_value.length() - 1);
                         }
                     }
                     ui->maxSizeZipToTypeReanalzeComboBox->setEnabled(m_flag);
                     ui->maxSizeZipToTypeReanalzeCheckBox->setChecked(m_flag);
                     if (m_value != "") ui->maxSizeZipToTypeReanalzeSpinBox->setValue(m_value.toInt());
                     ui->maxSizeZipToTypeReanalzeSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame10->setStyleSheet(css):ui->frame10->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame10->setVisible(true):ui->frame10->setVisible(false);
                        break;
            case 10: ui->maxNumberOfPartitionsInDiskImageCheckBox->setChecked(m_flag);
                     ui->maxNumberOfPartitionsInDiskImageSpinBox->setValue(m_value.toInt());
                     ui->maxNumberOfPartitionsInDiskImageSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame11->setStyleSheet(css):ui->frame11->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame11->setVisible(true):ui->frame11->setVisible(false);
                        break;
            case 11: ui->maxNumberOfIconsInPEFileCheckBox->setChecked(m_flag);
                     ui->maxNumberOfIconsInPEFileSpinBox->setValue(m_value.toInt());
                     ui->maxNumberOfIconsInPEFileSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame12->setStyleSheet(css):ui->frame12->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame12->setVisible(true):ui->frame12->setVisible(false);
                        break;
            case 12: ui->numberOfSecondsToWaitForResponseCheckBox->setChecked(m_flag);
                     ui->numberOfSecondsToWaitForResponseSpinBox->setValue(m_value.toInt());
                     ui->numberOfSecondsToWaitForResponseSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame13->setStyleSheet(css):ui->frame13->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame13->setVisible(true):ui->frame13->setVisible(false);
                        break;
            case 13: ui->numberOfSecondsToWaitForByteCodeCheckBox->setChecked(m_flag);
                     ui->numberOfSecondsToWaitForByteCodeSpinBox->setValue(m_value.toInt());
                     ui->numberOfSecondsToWaitForByteCodeSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame14->setStyleSheet(css):ui->frame14->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame14->setVisible(true):ui->frame14->setVisible(false);
                        break;
            case 14: ui->collectAndPrintExecutionStatisticsCheckBox->setChecked(m_flag);
                     ui->collectAndPrintExecutionStatisticsComboBox->setCurrentIndex(m_value.toInt());
                     ui->collectAndPrintExecutionStatisticsComboBox->setEnabled(m_flag);
                     m_flag == true?ui->frame15->setStyleSheet(css):ui->frame15->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame15->setVisible(true):ui->frame15->setVisible(false);
                        break;
            case 15: ui->structuredSSNFormatCheckBox->setChecked(m_flag);
                     ui->structuredSSNFormatComboBox->setCurrentIndex(m_value.toInt());
                     ui->structuredSSNFormatComboBox->setEnabled(m_flag);
                     m_flag == true?ui->frame16->setStyleSheet(css):ui->frame16->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame16->setVisible(true):ui->frame16->setVisible(false);
                        break;
            case 16: ui->structuredSSNCountCheckBox->setChecked(m_flag);
                     ui->structuredSSNCountSpinBox->setValue(m_value.toInt());
                     ui->structuredSSNCountSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame17->setStyleSheet(css):ui->frame17->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame17->setVisible(true):ui->frame17->setVisible(false);
                        break;
            case 17: ui->structuredCCCountCheckBox->setChecked(m_flag);
                     ui->structuredCCCountSpinBox->setValue(m_value.toInt());
                     ui->structuredCCCountSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame18->setStyleSheet(css):ui->frame18->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame18->setVisible(true):ui->frame18->setVisible(false);
                        break;
            case 18: ui->structuredCCModeCheckBox->setChecked(m_flag);
                     ui->structuredCCModeComboBox->setCurrentIndex(m_value.toInt());
                     ui->structuredCCModeComboBox->setEnabled(m_flag);
                     m_flag == true?ui->frame19->setStyleSheet(css):ui->frame19->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame19->setVisible(true):ui->frame19->setVisible(false);
                        break;
            case 19: ui->maxScanTimeCheckBox->setChecked(m_flag);
                     ui->maxScanTimeSpinBox->setValue(m_value.toInt());
                     ui->maxScanTimeSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame20->setStyleSheet(css):ui->frame20->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame20->setVisible(true):ui->frame20->setVisible(false);
                        break;
            case 20: ui->maxRecursionHWP3CheckBox->setChecked(m_flag);
                     ui->maxRecursionHWP3SpinBox->setValue(m_value.toInt());
                     ui->maxRecursionHWP3SpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame21->setStyleSheet(css):ui->frame21->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame21->setVisible(true):ui->frame21->setVisible(false);
                        break;
            case 21: ui->maxPCREMatchCheckBox->setChecked(m_flag);
                     ui->maxPCREMatchSpinBox->setValue(m_value.toInt());
                     ui->maxPCREMatchSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame22->setStyleSheet(css):ui->frame22->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame22->setVisible(true):ui->frame22->setVisible(false);
                        break;
            case 22: ui->maxRecursionPCREMatchCheckBox->setChecked(m_flag);
                     ui->maxRecursionPCREMatchSpinBox->setValue(m_value.toInt());
                     ui->maxRecursionPCREMatchSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame23->setStyleSheet(css):ui->frame23->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame23->setVisible(true):ui->frame23->setVisible(false);
                        break;
            case 23: if (m_value.right(1) != ""){
                         if ((m_value.indexOf("K") != -1) || (m_value.indexOf("M") != -1)) {
                             ui->maxPCREFileSizeComboBox->setCurrentIndex(ui->maxPCREFileSizeComboBox->findText(m_value.right(1)));
                             m_value = m_value.left(m_value.length() - 1);
                         }
                     }
                     ui->maxPCREFileSizeComboBox->setEnabled(m_flag);
                     ui->maxPCREFileSizeCheckBox->setChecked(m_flag);
                     if (m_value != "") ui->maxPCREFileSizeSpinBox->setValue(m_value.toInt());
                     ui->maxPCREFileSizeSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame24->setStyleSheet(css):ui->frame24->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame24->setVisible(true):ui->frame24->setVisible(false);
                        break;
            case 24: ui->databaseOutdatedCheckBox->setChecked(m_flag);
                     ui->databaseOutdatedSpinBox->setValue(m_value.toInt());
                     ui->databaseOutdatedSpinBox->setEnabled(m_flag);
                     m_flag == true?ui->frame25->setStyleSheet(css):ui->frame25->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",m_switches.at(i)) == true?ui->frame25->setVisible(true):ui->frame25->setVisible(false);
                        break;
            }
    }
    update = false;
}
