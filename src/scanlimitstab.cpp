#include "scanlimitstab.h"
#include "ui_scanlimitstab.h"

scanLimitsTab::scanLimitsTab(QWidget *parent) : QWidget(parent), ui(new Ui::scanLimitsTab)
{
    ui->setupUi(this);
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini");
    if (setupFile->sectionExists("ScanLimitations") == true) updateLimits(); else writeLimits();
}

scanLimitsTab::~scanLimitsTab()
{
    delete ui;
}

void scanLimitsTab::writeLimits(){
QString keyword;
QString value;
QString checked;

    if (update == false) {
        QStringList keywords;
/*00*/        keywords << "Files larger than this will be skipped and assumed clean";
/*01*/        keywords << "The maximum amount of data to scan for each container file";
/*02*/        keywords << "The maximum number of files to scan for each container file";
/*03*/        keywords << "Maximum archive recursion level for container file";
/*04*/        keywords << "Maximum directory recursion level";
/*05*/        keywords << "Maximum size file to check for embedded PE";
/*06*/        keywords << "Maximum size of HTML file to normalize";
/*07*/        keywords << "Maximum size of normalized HTML file to scan";
/*08*/        keywords << "Maximum size of script file to normalize";
/*09*/        keywords << "Maximum size zip to type reanalyze";
/*10*/        keywords << "Maximum number of partitions in disk image to be scanned";
/*11*/        keywords << "Maximum number of icons in PE file to be scanned";
/*12*/        keywords << "Number of seconds to wait for waiting a response back from the stats server";
/*13*/        keywords << "Bytecode timeout in milliseconds";
/*14*/        keywords << "Collect and print execution statistics";
/*15*/        keywords << "Structured SSN Format";
/*16*/        keywords << "Structured SSN Count";
/*17*/        keywords << "Structured CC Count";
/*18*/        keywords << "Structured CC Mode";
/*19*/        keywords << "Max Scan-Time";
/*20*/        keywords << "Max recursion to HWP3 parsing function";
/*21*/        keywords << "Max calls to PCRE match function";
/*22*/        keywords << "Max recursion calls to the PCRE match function";
/*23*/        keywords << "Max PCRE file size";
/*24*/        keywords << "Database outdated if older than x days";

        for (int i = 0; i < keywords.length(); i++) {
            keyword = keywords.at(i);

            switch (i) {
            case 0 : value = QString::number(ui->filesLargerThanThisSpinBox->value()) + ui->filesLargerThanThisComboBox->currentText();
                     ui->filesLargerThanThisCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 1 : value = QString::number(ui->maxAmountForContainerSpinBox->value()) + ui->maxAmountForContainerComboBox->currentText();
                     ui->maxAmountForContainerCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 2 : value = QString::number(ui->maxNumberForContainerSpinBox->value());
                     ui->maxNumberForContainerCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 3 : value = QString::number(ui->maxArchiveRecursionForContainerSpinBox->value());
                     ui->maxArchiveRecursionForContainerCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 4 : value = QString::number(ui->maxDirRecursionLevelSpinBox->value());
                     ui->maxDirRecursionLevelCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 5 : value = QString::number(ui->maxSizeFileForPESpinBox->value()) + ui->maxSizeFileForPEComboBox->currentText();
                     ui->maxSizeFileForPECheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 6 : value = QString::number(ui->maxSizeHTMLFileToNormalizeSpinBox->value()) + ui->maxSizeHTMLFileToNormalizeComboBox->currentText();
                     ui->maxSizeHTMLFileToNormalizeCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 7 : value = QString::number(ui->maxSizeOfNormalizedHTMLFileSpinBox->value()) + ui->maxSizeOfNormalizedHTMLFileComboBox->currentText();
                     ui->maxSizeOfNormalizedHTMLFileCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 8 : value = QString::number(ui->maxSizeOfScriptFileToNormalizeSpinBox->value()) + ui->maxSizeOfScriptFileToNormalizeComboBox->currentText();
                     ui->maxSizeOfScriptFileToNormalizeCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 9 : value = QString::number(ui->maxSizeZipToTypeReanalzeSpinBox->value()) + ui->maxSizeZipToTypeReanalzeComboBox->currentText();
                     ui->maxSizeZipToTypeReanalzeCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 10: value = QString::number(ui->maxNumberOfPartitionsInDiskImageSpinBox->value());
                     ui->maxNumberOfPartitionsInDiskImageCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 11: value = QString::number(ui->maxNumberOfIconsInPEFileSpinBox->value());
                     ui->maxNumberOfIconsInPEFileCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 12: value = QString::number(ui->numberOfSecondsToWaitForResponseSpinBox->value());
                     ui->numberOfSecondsToWaitForResponseCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 13: value = QString::number(ui->numberOfSecondsToWaitForByteCodeSpinBox->value());
                     ui->numberOfSecondsToWaitForByteCodeCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 14: value = QString::number(ui->collectAndPrintExecutionStatisticsComboBox->currentIndex());
                     ui->collectAndPrintExecutionStatisticsCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 15: value = QString::number(ui->structuredSSNFormatComboBox->currentIndex());
                     ui->structuredSSNFormatCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 16: value = QString::number(ui->structuredSSNCountSpinBox->value());
                     ui->structuredSSNCountCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 17: value = QString::number(ui->structuredCCCountSpinBox->value());
                     ui->structuredCCCountCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 18: value = QString::number(ui->structuredCCModeComboBox->currentIndex());
                     ui->structuredCCModeCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 19: value = QString::number(ui->maxScanTimeSpinBox->value());
                     ui->maxScanTimeCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 20: value = QString::number(ui->maxRecursionHWP3SpinBox->value());
                     ui->maxRecursionHWP3CheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 21: value = QString::number(ui->maxPCREMatchSpinBox->value());
                     ui->maxPCREMatchCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 22: value = QString::number(ui->maxRecursionPCREMatchSpinBox->value());
                     ui->maxRecursionPCREMatchCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 23: value = QString::number(ui->maxPCREFileSizeSpinBox->value()) + ui->maxPCREFileSizeComboBox->currentText();
                     ui->maxPCREFileSizeCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            case 24: value = QString::number(ui->databaseOutdatedSpinBox->value());
                     ui->databaseOutdatedCheckBox->isChecked() == true?checked="checked":checked="not checked";
                        break;
            }
            setupFile->setSectionValue("ScanLimitations",keyword,checked + "|" + value);
        }

        emit updateClamdConf();

        updateLimits();
    }
}


void scanLimitsTab::updateLimits(){
QString value;
QString checked;
QStringList keywords;
QString keyword;
bool flag;

    update = true;

/*00*/    keywords << "Files larger than this will be skipped and assumed clean";
/*01*/    keywords << "The maximum amount of data to scan for each container file";
/*02*/    keywords << "The maximum number of files to scan for each container file";
/*03*/    keywords << "Maximum archive recursion level for container file";
/*04*/    keywords << "Maximum directory recursion level";
/*05*/    keywords << "Maximum size file to check for embedded PE";
/*06*/    keywords << "Maximum size of HTML file to normalize";
/*07*/    keywords << "Maximum size of normalized HTML file to scan";
/*08*/    keywords << "Maximum size of script file to normalize";
/*09*/    keywords << "Maximum size zip to type reanalyze";
/*10*/    keywords << "Maximum number of partitions in disk image to be scanned";
/*11*/    keywords << "Maximum number of icons in PE file to be scanned";
/*12*/    keywords << "Number of seconds to wait for waiting a response back from the stats server";
/*13*/    keywords << "Bytecode timeout in milliseconds";
/*14*/    keywords << "Collect and print execution statistics";
/*15*/    keywords << "Structured SSN Format";
/*16*/    keywords << "Structured SSN Count";
/*17*/    keywords << "Structured CC Count";
/*18*/    keywords << "Structured CC Mode";
/*19*/    keywords << "Max Scan-Time";
/*20*/    keywords << "Max recursion to HWP3 parsing function";
/*21*/    keywords << "Max calls to PCRE match function";
/*22*/    keywords << "Max recursion calls to the PCRE match function";
/*23*/    keywords << "Max PCRE file size";
/*24*/    keywords << "Database outdated if older than x days";

    QStringList switches;
    switches << "--max-filesize" << "--max-scansize" << "--max-files" << "--max-recursion" << "--max-dir-recursion" << "--max-embeddedpe";
    switches << "--max-htmlnormalize" << "--max-htmlnotags" << "--max-scriptnormalize" << "--max-ziptypercg" << "--max-partitions" << "--max-iconspe";
    switches << "--stats-timeout" << "--bytecode-timeout" << "--statistics" << "--structured-ssn-format" << "--structured-ssn-count" << "--structured-cc-count";
    switches << "--structured-cc-count" << "--structured-cc-mode" << "--max-rechwp3" << "--pcre-match-limit" << "--pcre-recmatch-limit" << "--pcre-max-filesize";
    switches << "--fail-if-cvd-older-than";

    for (int i = 0; i < keywords.length(); i++) {
        keyword = keywords.at(i);

        value = setupFile->getSectionValue("ScanLimitations",keyword);
        checked = value.left(value.indexOf("|"));
        value = value.mid(value.indexOf("|") + 1);
        checked=="checked"?flag=true:flag=false;

        switch (i) {
            case 0 : if (value.right(1) != ""){
                         if ((value.indexOf("K") != -1) || (value.indexOf("M") != -1)) {
                             ui->filesLargerThanThisComboBox->setCurrentIndex(ui->filesLargerThanThisComboBox->findText(value.right(1)));
                            value = value.left(value.length() - 1);
                         }
                     }
                     ui->filesLargerThanThisComboBox->setEnabled(flag);
                     ui->filesLargerThanThisCheckBox->setChecked(flag);
                     if (value != "") ui->filesLargerThanThisSpinBox->setValue(value.toInt());
                     ui->filesLargerThanThisSpinBox->setEnabled(flag);
                     flag == true?ui->frame01->setStyleSheet("background-color:lightgreen"):ui->frame01->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame01->setVisible(true):ui->frame01->setVisible(false);

                        break;
            case 1 : if (value.right(1) != ""){
                         if ((value.indexOf("K") != -1) || (value.indexOf("M") != -1)) {
                             ui->maxAmountForContainerComboBox->setCurrentIndex(ui->maxAmountForContainerComboBox->findText(value.right(1)));
                             value = value.left(value.length() - 1);
                         }
                     }
                     ui->maxAmountForContainerComboBox->setEnabled(flag);
                     ui->maxAmountForContainerCheckBox->setChecked(flag);
                     if (value != "") ui->maxAmountForContainerSpinBox->setValue(value.toInt());
                     ui->maxAmountForContainerSpinBox->setEnabled(flag);
                     flag == true?ui->frame02->setStyleSheet("background-color:lightgreen"):ui->frame02->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame02->setVisible(true):ui->frame02->setVisible(false);
                        break;
            case 2 : ui->maxNumberForContainerCheckBox->setChecked(flag);
                     ui->maxNumberForContainerSpinBox->setValue(value.toInt());
                     ui->maxNumberForContainerSpinBox->setEnabled(flag);
                     flag == true?ui->frame03->setStyleSheet("background-color:lightgreen"):ui->frame03->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame03->setVisible(true):ui->frame03->setVisible(false);
                        break;
            case 3 : ui->maxArchiveRecursionForContainerCheckBox->setChecked(flag);
                     ui->maxArchiveRecursionForContainerSpinBox->setValue(value.toInt());
                     ui->maxArchiveRecursionForContainerSpinBox->setEnabled(flag);
                     flag == true?ui->frame04->setStyleSheet("background-color:lightgreen"):ui->frame04->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame04->setVisible(true):ui->frame04->setVisible(false);
                        break;
            case 4 : ui->maxDirRecursionLevelCheckBox->setChecked(flag);
                     ui->maxDirRecursionLevelSpinBox->setValue(value.toInt());
                     ui->maxDirRecursionLevelSpinBox->setEnabled(flag);
                     flag == true?ui->frame05->setStyleSheet("background-color:lightgreen"):ui->frame05->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame05->setVisible(true):ui->frame05->setVisible(false);
                        break;
            case 5 : if (value.right(1) != ""){
                         if ((value.indexOf("K") != -1) || (value.indexOf("M") != -1)) {
                             ui->maxSizeFileForPEComboBox->setCurrentIndex(ui->maxSizeFileForPEComboBox->findText(value.right(1)));
                             value = value.left(value.length() - 1);
                         }
                     }
                     ui->maxSizeFileForPEComboBox->setEnabled(flag);
                     ui->maxSizeFileForPECheckBox->setChecked(flag);
                     if (value != "") ui->maxSizeFileForPESpinBox->setValue(value.toInt());
                     ui->maxSizeFileForPESpinBox->setEnabled(flag);
                     flag == true?ui->frame06->setStyleSheet("background-color:lightgreen"):ui->frame06->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame06->setVisible(true):ui->frame06->setVisible(false);
                        break;
            case 6 : if (value.right(1) != ""){
                         if ((value.indexOf("K") != -1) || (value.indexOf("M") != -1)) {
                             ui->maxSizeHTMLFileToNormalizeComboBox->setCurrentIndex(ui->maxSizeHTMLFileToNormalizeComboBox->findText(value.right(1)));
                             value = value.left(value.length() - 1);
                         }
                     }
                     ui->maxSizeHTMLFileToNormalizeComboBox->setEnabled(flag);
                     ui->maxSizeHTMLFileToNormalizeCheckBox->setChecked(flag);
                     if (value != "") ui->maxSizeHTMLFileToNormalizeSpinBox->setValue(value.toInt());
                     ui->maxSizeHTMLFileToNormalizeSpinBox->setEnabled(flag);
                     flag == true?ui->frame07->setStyleSheet("background-color:lightgreen"):ui->frame07->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame07->setVisible(true):ui->frame07->setVisible(false);
                        break;
            case 7 : if (value.right(1) != ""){
                         if ((value.indexOf("K") != -1) || (value.indexOf("M") != -1)) {
                             ui->maxSizeOfNormalizedHTMLFileComboBox->setCurrentIndex(ui->maxSizeOfNormalizedHTMLFileComboBox->findText(value.right(1)));
                             value = value.left(value.length() - 1);
                         }
                     }
                     ui->maxSizeOfNormalizedHTMLFileComboBox->setEnabled(flag);
                     ui->maxSizeOfNormalizedHTMLFileCheckBox->setChecked(flag);
                     if (value != "") ui->maxSizeOfNormalizedHTMLFileSpinBox->setValue(value.toInt());
                     ui->maxSizeOfNormalizedHTMLFileSpinBox->setEnabled(flag);
                     flag == true?ui->frame08->setStyleSheet("background-color:lightgreen"):ui->frame08->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame08->setVisible(true):ui->frame08->setVisible(false);
                        break;
            case 8 : if (value.right(1) != ""){
                         if ((value.indexOf("K") != -1) || (value.indexOf("M") != -1)) {
                             ui->maxSizeOfScriptFileToNormalizeComboBox->setCurrentIndex(ui->maxSizeOfScriptFileToNormalizeComboBox->findText(value.right(1)));
                             value = value.left(value.length() - 1);
                         }
                     }
                     ui->maxSizeOfScriptFileToNormalizeComboBox->setEnabled(flag);
                     ui->maxSizeOfScriptFileToNormalizeCheckBox->setChecked(flag);
                     if (value != "") ui->maxSizeOfScriptFileToNormalizeSpinBox->setValue(value.toInt());
                     ui->maxSizeOfScriptFileToNormalizeSpinBox->setEnabled(flag);
                     flag == true?ui->frame09->setStyleSheet("background-color:lightgreen"):ui->frame09->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame09->setVisible(true):ui->frame09->setVisible(false);
                        break;
            case 9 : if (value.right(1) != ""){
                         if ((value.indexOf("K") != -1) || (value.indexOf("M") != -1)) {
                             ui->maxSizeZipToTypeReanalzeComboBox->setCurrentIndex(ui->maxSizeZipToTypeReanalzeComboBox->findText(value.right(1)));
                             value = value.left(value.length() - 1);
                         }
                     }
                     ui->maxSizeZipToTypeReanalzeComboBox->setEnabled(flag);
                     ui->maxSizeZipToTypeReanalzeCheckBox->setChecked(flag);
                     if (value != "") ui->maxSizeZipToTypeReanalzeSpinBox->setValue(value.toInt());
                     ui->maxSizeZipToTypeReanalzeSpinBox->setEnabled(flag);
                     flag == true?ui->frame10->setStyleSheet("background-color:lightgreen"):ui->frame10->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame10->setVisible(true):ui->frame10->setVisible(false);
                        break;
            case 10: ui->maxNumberOfPartitionsInDiskImageCheckBox->setChecked(flag);
                     ui->maxNumberOfPartitionsInDiskImageSpinBox->setValue(value.toInt());
                     ui->maxNumberOfPartitionsInDiskImageSpinBox->setEnabled(flag);
                     flag == true?ui->frame11->setStyleSheet("background-color:lightgreen"):ui->frame11->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame11->setVisible(true):ui->frame11->setVisible(false);
                        break;
            case 11: ui->maxNumberOfIconsInPEFileCheckBox->setChecked(flag);
                     ui->maxNumberOfIconsInPEFileSpinBox->setValue(value.toInt());
                     ui->maxNumberOfIconsInPEFileSpinBox->setEnabled(flag);
                     flag == true?ui->frame12->setStyleSheet("background-color:lightgreen"):ui->frame12->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame12->setVisible(true):ui->frame12->setVisible(false);
                        break;
            case 12: ui->numberOfSecondsToWaitForResponseCheckBox->setChecked(flag);
                     ui->numberOfSecondsToWaitForResponseSpinBox->setValue(value.toInt());
                     ui->numberOfSecondsToWaitForResponseSpinBox->setEnabled(flag);
                     flag == true?ui->frame13->setStyleSheet("background-color:lightgreen"):ui->frame13->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame13->setVisible(true):ui->frame13->setVisible(false);
                        break;
            case 13: ui->numberOfSecondsToWaitForByteCodeCheckBox->setChecked(flag);
                     ui->numberOfSecondsToWaitForByteCodeSpinBox->setValue(value.toInt());
                     ui->numberOfSecondsToWaitForByteCodeSpinBox->setEnabled(flag);
                     flag == true?ui->frame14->setStyleSheet("background-color:lightgreen"):ui->frame14->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame14->setVisible(true):ui->frame14->setVisible(false);
                        break;
            case 14: ui->collectAndPrintExecutionStatisticsCheckBox->setChecked(flag);
                     ui->collectAndPrintExecutionStatisticsComboBox->setCurrentIndex(value.toInt());
                     ui->collectAndPrintExecutionStatisticsComboBox->setEnabled(flag);
                     flag == true?ui->frame15->setStyleSheet("background-color:lightgreen"):ui->frame15->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame15->setVisible(true):ui->frame15->setVisible(false);
                        break;
            case 15: ui->structuredSSNFormatCheckBox->setChecked(flag);
                     ui->structuredSSNFormatComboBox->setCurrentIndex(value.toInt());
                     ui->structuredSSNFormatComboBox->setEnabled(flag);
                     flag == true?ui->frame16->setStyleSheet("background-color:lightgreen"):ui->frame16->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame16->setVisible(true):ui->frame16->setVisible(false);
                        break;
            case 16: ui->structuredSSNCountCheckBox->setChecked(flag);
                     ui->structuredSSNCountSpinBox->setValue(value.toInt());
                     ui->structuredSSNCountSpinBox->setEnabled(flag);
                     flag == true?ui->frame17->setStyleSheet("background-color:lightgreen"):ui->frame17->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame17->setVisible(true):ui->frame17->setVisible(false);
                        break;
            case 17: ui->structuredCCCountCheckBox->setChecked(flag);
                     ui->structuredCCCountSpinBox->setValue(value.toInt());
                     ui->structuredCCCountSpinBox->setEnabled(flag);
                     flag == true?ui->frame18->setStyleSheet("background-color:lightgreen"):ui->frame18->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame18->setVisible(true):ui->frame18->setVisible(false);
                        break;
            case 18: ui->structuredCCModeCheckBox->setChecked(flag);
                     ui->structuredCCModeComboBox->setCurrentIndex(value.toInt());
                     ui->structuredCCModeComboBox->setEnabled(flag);
                     flag == true?ui->frame19->setStyleSheet("background-color:lightgreen"):ui->frame19->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame19->setVisible(true):ui->frame19->setVisible(false);
                        break;
            case 19: ui->maxScanTimeCheckBox->setChecked(flag);
                     ui->maxScanTimeSpinBox->setValue(value.toInt());
                     ui->maxScanTimeSpinBox->setEnabled(flag);
                     flag == true?ui->frame20->setStyleSheet("background-color:lightgreen"):ui->frame20->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame20->setVisible(true):ui->frame20->setVisible(false);
                        break;
            case 20: ui->maxRecursionHWP3CheckBox->setChecked(flag);
                     ui->maxRecursionHWP3SpinBox->setValue(value.toInt());
                     ui->maxRecursionHWP3SpinBox->setEnabled(flag);
                     flag == true?ui->frame21->setStyleSheet("background-color:lightgreen"):ui->frame21->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame21->setVisible(true):ui->frame21->setVisible(false);
                        break;
            case 21: ui->maxPCREMatchCheckBox->setChecked(flag);
                     ui->maxPCREMatchSpinBox->setValue(value.toInt());
                     ui->maxPCREMatchSpinBox->setEnabled(flag);
                     flag == true?ui->frame22->setStyleSheet("background-color:lightgreen"):ui->frame22->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame22->setVisible(true):ui->frame22->setVisible(false);
                        break;
            case 22: ui->maxRecursionPCREMatchCheckBox->setChecked(flag);
                     ui->maxRecursionPCREMatchSpinBox->setValue(value.toInt());
                     ui->maxRecursionPCREMatchSpinBox->setEnabled(flag);
                     flag == true?ui->frame23->setStyleSheet("background-color:lightgreen"):ui->frame23->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame23->setVisible(true):ui->frame23->setVisible(false);
                        break;
            case 23: if (value.right(1) != ""){
                         if ((value.indexOf("K") != -1) || (value.indexOf("M") != -1)) {
                             ui->maxPCREFileSizeComboBox->setCurrentIndex(ui->maxPCREFileSizeComboBox->findText(value.right(1)));
                             value = value.left(value.length() - 1);
                         }
                     }
                     ui->maxPCREFileSizeComboBox->setEnabled(flag);
                     ui->maxPCREFileSizeCheckBox->setChecked(flag);
                     if (value != "") ui->maxPCREFileSizeSpinBox->setValue(value.toInt());
                     ui->maxPCREFileSizeSpinBox->setEnabled(flag);
                     flag == true?ui->frame24->setStyleSheet("background-color:lightgreen"):ui->frame24->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame24->setVisible(true):ui->frame24->setVisible(false);
                        break;
            case 24: ui->databaseOutdatedCheckBox->setChecked(flag);
                     ui->databaseOutdatedSpinBox->setValue(value.toInt());
                     ui->databaseOutdatedSpinBox->setEnabled(flag);
                     flag == true?ui->frame25->setStyleSheet("background-color:lightgreen"):ui->frame25->setStyleSheet("");
                     setupFile->keywordExists("OtherKeywords",switches.at(i)) == true?ui->frame25->setVisible(true):ui->frame25->setVisible(false);
                        break;
            }
    }
    update = false;
}
