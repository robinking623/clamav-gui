#include "partiallogobject.h"
#include "ui_partiallogobject.h"

partialLogObject::partialLogObject(QWidget *parent, QString logText, bool highlighterDisabled) : QWidget(parent), ui(new Ui::partialLogObject){
    ui->setupUi(this);
    logHighlighter = NULL;
    if (highlighterDisabled == false) logHighlighter = new highlighter(ui->logPlainText->document());
    setLogText(logText);
    start = 0;
    errorStart = 0;
    infectedStart = 0;
}

partialLogObject::~partialLogObject(){
    delete ui;
}

void partialLogObject::setLogText(QString logText){
QString m_engine;
QString m_scannedDirs;
QString m_scannedFiles;
QString m_errors;
QString m_infectedFiles;
int m_pos;

    ui->logPlainText->setPlainText(logText);

    m_pos = logText.indexOf("Engine version:");
    if (m_pos > -1){
        m_engine = logText.mid(m_pos + 15,logText.indexOf("\n",m_pos + 15) - (m_pos + 15));
    } else {
        m_engine = "n/a";
    }

    m_pos = logText.indexOf("Scanned directories:");
    if (m_pos > -1){
        m_scannedDirs = logText.mid(m_pos + 20,logText.indexOf("\n",m_pos + 20) - (m_pos + 20));
    } else {
        m_scannedDirs = "n/a";
    }

    m_pos = logText.indexOf("Scanned files:");
    if (m_pos > -1){
        m_scannedFiles = logText.mid(m_pos + 14,logText.indexOf("\n",m_pos + 14) - (m_pos + 14));
    } else {
        m_scannedFiles = "n/a";
    }

    m_pos = logText.indexOf("Total m_errors:");
    if (m_pos > -1){
        m_errors = logText.mid(m_pos + 13,logText.indexOf("\n",m_pos + 13) - (m_pos + 13));
    } else {
        m_errors = "n/a";
    }

    m_pos = logText.indexOf("Infected files:");
    if (m_pos > -1){
        m_infectedFiles = logText.mid(m_pos + 15,logText.indexOf("\n",m_pos + 15) - (m_pos + 15));
    } else {
        m_infectedFiles = "n/a";
    }

    ui->engineVersionLabel->setText(tr("Engine Version: ") + m_engine);
    ui->scannedDirectoriesLabel->setText(tr("Scanned Directories: ") + m_scannedDirs);
    ui->scannedFilesLabel->setText(tr("Scanned Files: ") + m_scannedFiles);
    ui->totalErrorsLabel->setText(tr("Total Errors: ") + m_errors);
    ui->infectedFilesLabel->setText(tr("Infected Files: ") + m_infectedFiles);
}

QString partialLogObject::getLogText(){
    return "<Scanning startet> " + ui->logPlainText->toPlainText();
}

void partialLogObject::slot_searchButtonClicked(){
QTextCursor m_cursor = ui->logPlainText->textCursor();
QString m_searchString = ui->searchTextLineEdit->text();
int m_pos = ui->logPlainText->toPlainText().toUpper().indexOf(m_searchString.toUpper(),start);

    if (m_pos > -1){
        ui->searchButton->setText(tr("continue"));
        start = m_pos + m_searchString.length();
        m_cursor.movePosition(QTextCursor::Start);
        m_cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,m_pos);
        m_cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,m_searchString.length());
        ui->logPlainText->setTextCursor(m_cursor);
        ui->logPlainText->ensureCursorVisible();
    } else {
        if (QMessageBox::question(this,"INFO",tr("Searchstring not found!\nContinue from the Start of the Log?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes){
            start = 0;
            m_pos = ui->logPlainText->toPlainText().toUpper().indexOf(m_searchString.toUpper(),start);
            if (m_pos > -1){
                ui->searchButton->setText(tr("continue"));
                start = m_pos + m_searchString.length();
                m_cursor.movePosition(QTextCursor::Start);
                m_cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,m_pos);
                m_cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,m_searchString.length());
                ui->logPlainText->setTextCursor(m_cursor);
                ui->logPlainText->ensureCursorVisible();
            } else {
                QMessageBox::information(this,tr("INFO"),tr("Searchstring not found!"));
            }
        }
    }
}

void partialLogObject::slot_searchLineEditChanged(){
    start = 0;
    ui->searchButton->setText(tr("search"));
}

void partialLogObject::slot_clearLineEditButtonClicked(){
    ui->searchTextLineEdit->setText("");
    start = 0;
}

void partialLogObject::slot_totalErrorButtonClicked(){
QTextCursor m_cursor = ui->logPlainText->textCursor();
QStringList m_searchStrings;
QString m_searchString;
int m_pos = -1;
int m_index = 0;

    m_searchStrings << "Access denied" << "Empty file";

    while ((m_pos == -1) & (m_index < m_searchStrings.count())){
        m_searchString = m_searchStrings.at(m_index);
        m_index ++;
        m_pos = ui->logPlainText->toPlainText().indexOf(m_searchString,errorStart);
    }

    if (m_pos > -1){
        errorStart = m_pos + m_searchString.length();
        m_cursor.movePosition(QTextCursor::Start);
        m_cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,m_pos);
        m_cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,m_searchString.length());
        ui->logPlainText->setTextCursor(m_cursor);
        ui->logPlainText->ensureCursorVisible();
        if (ui->logPlainText->toPlainText().indexOf(m_searchString,errorStart) == -1) errorStart = 0;
    }
}

void partialLogObject::slot_infectedFilesButtonClicked(){
QTextCursor m_cursor = ui->logPlainText->textCursor();
QString m_searchString = "FOUND";
int m_pos = ui->logPlainText->toPlainText().indexOf(m_searchString,infectedStart);

    if (m_pos > -1){
        infectedStart = m_pos + m_searchString.length();
        m_cursor.movePosition(QTextCursor::Start);
        m_cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,m_pos);
        m_cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,m_searchString.length());
        ui->logPlainText->setTextCursor(m_cursor);
        ui->logPlainText->ensureCursorVisible();
        if (ui->logPlainText->toPlainText().indexOf(m_searchString,infectedStart) == -1) infectedStart = 0;
    }
}

void partialLogObject::slot_add_remove_highlighter(bool state)
{
    if (state == true) {
      if (logHighlighter != NULL) {
        delete logHighlighter;
        logHighlighter = NULL;
      }
    } else {
         if (logHighlighter == NULL) {
             logHighlighter = new highlighter(ui->logPlainText->document());
         } else {
             delete logHighlighter;
             logHighlighter = new highlighter(ui->logPlainText->document());
         }
    }
}
