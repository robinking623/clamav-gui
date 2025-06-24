#include "scanoption.h"
#define css "background-color:#404040;color:white"

scanoption::scanoption(QWidget *parent, QString setupFileName, QString section, bool checked, QString label, QString comment) 
: QWidget(parent)
{
    m_ui.setupUi(this);
    m_setupFile = new setupFileHandler(setupFileName,this);
    setupFileHandler * baseSetup = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    QString languageset = baseSetup->getSectionValue("Setup","language");
    translator trans(languageset);

    m_setupFileSection = section;
    m_option = label;
    m_com = trans.translateit(comment);
    m_ui.checkBox->setChecked(checked);

    if (checked == true) {
        this->setStyleSheet(css);
        m_setupFile->setSectionValue(m_setupFileSection,m_option,m_com);
    }

    m_ui.checkBox->setText(trans.beautifyString(m_com));
    m_ui.checkBox->setToolTip(m_option);
}


void scanoption::slot_checkboxClicked(){
    if (m_ui.checkBox->isChecked() == false) {
        m_setupFile->removeKeyword(m_setupFileSection,m_option);
        this->setStyleSheet("");
    } else {
        m_setupFile->setSectionValue(m_setupFileSection,m_option,m_com);
        this->setStyleSheet(css);
    }

    emit valuechanged();
}
