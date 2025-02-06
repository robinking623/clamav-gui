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

    for (int i = 0; i < keywords.length(); i++) {
        keyword = keywords.at(i);

        value = setupFile->getSectionValue("ScanLimitations",keyword);
        checked = value.left(value.indexOf("|"));
        value = value.mid(value.indexOf("|") + 1);
        checked=="checked"?flag=true:flag=false;

        switch (i) {
        case 0 : if (value.right(1) != ""){
                     ui->filesLargerThanThisComboBox->setCurrentIndex(ui->filesLargerThanThisComboBox->findText(value.right(1)));
                     value = value.left(value.length() - 1);
                 }
                 ui->filesLargerThanThisCheckBox->setChecked(flag);
                 ui->filesLargerThanThisSpinBox->setValue(value.toInt());
                    break;
        case 1 : if (value.right(1) != ""){
                     ui->maxAmountForContainerComboBox->setCurrentIndex(ui->maxAmountForContainerComboBox->findText(value.right(1)));
                     value = value.left(value.length() - 1);
                 }
                 ui->maxAmountForContainerCheckBox->setChecked(flag);
                 ui->maxAmountForContainerSpinBox->setValue(value.toInt());
                    break;
        case 2 : ui->maxNumberForContainerCheckBox->setChecked(flag);
                 ui->maxNumberForContainerSpinBox->setValue(value.toInt());
                    break;
        case 3 : ui->maxArchiveRecursionForContainerCheckBox->setChecked(flag);
                 ui->maxArchiveRecursionForContainerSpinBox->setValue(value.toInt());
                    break;
        case 4 : ui->maxDirRecursionLevelCheckBox->setChecked(flag);
                 ui->maxDirRecursionLevelSpinBox->setValue(value.toInt());
                    break;
        case 5 : if (value.right(1) != ""){
                     ui->maxSizeFileForPEComboBox->setCurrentIndex(ui->maxSizeFileForPEComboBox->findText(value.right(1)));
                     value = value.left(value.length() - 1);
                 }
                 ui->maxSizeFileForPECheckBox->setChecked(flag);
                 ui->maxSizeFileForPESpinBox->setValue(value.toInt());
                    break;
        case 6 : if (value.right(1) != ""){
                     ui->maxSizeHTMLFileToNormalizeComboBox->setCurrentIndex(ui->maxSizeHTMLFileToNormalizeComboBox->findText(value.right(1)));
                     value = value.left(value.length() - 1);
                 }
                 ui->maxSizeHTMLFileToNormalizeCheckBox->setChecked(flag);
                 ui->maxSizeHTMLFileToNormalizeSpinBox->setValue(value.toInt());
                    break;
        case 7 : if (value.right(1) != ""){
                     ui->maxSizeOfNormalizedHTMLFileComboBox->setCurrentIndex(ui->maxSizeOfNormalizedHTMLFileComboBox->findText(value.right(1)));
                     value = value.left(value.length() - 1);
                 }
                 ui->maxSizeOfNormalizedHTMLFileCheckBox->setChecked(flag);
                 ui->maxSizeOfNormalizedHTMLFileSpinBox->setValue(value.toInt());
                    break;
        case 8 : if (value.right(1) != ""){
                     ui->maxSizeOfScriptFileToNormalizeComboBox->setCurrentIndex(ui->maxSizeOfScriptFileToNormalizeComboBox->findText(value.right(1)));
                     value = value.left(value.length() - 1);
                 }
                 ui->maxSizeOfScriptFileToNormalizeCheckBox->setChecked(flag);
                 ui->maxSizeOfScriptFileToNormalizeSpinBox->setValue(value.toInt());
                    break;
        case 9 : if (value.right(1) != ""){
                     ui->maxSizeZipToTypeReanalzeComboBox->setCurrentIndex(ui->maxSizeZipToTypeReanalzeComboBox->findText(value.right(1)));
                     value = value.left(value.length() - 1);
                 }
                 ui->maxSizeZipToTypeReanalzeCheckBox->setChecked(flag);
                 ui->maxSizeZipToTypeReanalzeSpinBox->setValue(value.toInt());
                    break;
        case 10: ui->maxNumberOfPartitionsInDiskImageCheckBox->setChecked(flag);
                 ui->maxNumberOfPartitionsInDiskImageSpinBox->setValue(value.toInt());
                    break;
        case 11: ui->maxNumberOfIconsInPEFileCheckBox->setChecked(flag);
                 ui->maxNumberOfIconsInPEFileSpinBox->setValue(value.toInt());
                    break;
        case 12: ui->numberOfSecondsToWaitForResponseCheckBox->setChecked(flag);
                 ui->numberOfSecondsToWaitForResponseSpinBox->setValue(value.toInt());
                    break;
        case 13: ui->numberOfSecondsToWaitForByteCodeCheckBox->setChecked(flag);
                 ui->numberOfSecondsToWaitForByteCodeSpinBox->setValue(value.toInt());
                    break;
        case 14: ui->collectAndPrintExecutionStatisticsCheckBox->setChecked(flag);
                 ui->collectAndPrintExecutionStatisticsComboBox->setCurrentIndex(value.toInt());
                    break;
        case 15: ui->structuredSSNFormatCheckBox->setChecked(flag);
                 ui->structuredSSNFormatComboBox->setCurrentIndex(value.toInt());
                    break;
        case 16: ui->structuredSSNCountCheckBox->setChecked(flag);
                 ui->structuredSSNCountSpinBox->setValue(value.toInt());
                    break;
        case 17: ui->structuredCCCountCheckBox->setChecked(flag);
                 ui->structuredCCCountSpinBox->setValue(value.toInt());
                    break;
        case 18: ui->structuredCCModeCheckBox->setChecked(flag);
                 ui->structuredCCModeComboBox->setCurrentIndex(value.toInt());
                    break;
        case 19: ui->maxScanTimeCheckBox->setChecked(flag);
                 ui->maxScanTimeSpinBox->setValue(value.toInt());
                    break;
        case 20: ui->maxRecursionHWP3CheckBox->setChecked(flag);
                 ui->maxRecursionHWP3SpinBox->setValue(value.toInt());
                    break;
        case 21: ui->maxPCREMatchCheckBox->setChecked(flag);
                 ui->maxPCREMatchSpinBox->setValue(value.toInt());
                    break;
        case 22: ui->maxRecursionPCREMatchCheckBox->setChecked(flag);
                 ui->maxRecursionPCREMatchSpinBox->setValue(value.toInt());
                    break;
        case 23: if (value.right(1) != ""){
                     ui->maxPCREFileSizeComboBox->setCurrentIndex(ui->maxPCREFileSizeComboBox->findText(value.right(1)));
                     value = value.left(value.length() - 1);
                 }
                 ui->maxPCREFileSizeCheckBox->setChecked(flag);
                 ui->maxPCREFileSizeSpinBox->setValue(value.toInt());
                    break;
        case 24: ui->databaseOutdatedCheckBox->setChecked(flag);
                 ui->databaseOutdatedSpinBox->setValue(value.toInt());
                    break;
        }
    }
    update = false;
}
