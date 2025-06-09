#include "scanoption.h"
#include "ui_scanoption.h"
#define css "background-color:#404040;color:white"

scanoption::scanoption(QWidget *parent, QString setupFileName, QString section, bool checked, QString label, QString comment) :
    QWidget(parent),
    ui(new Ui::scanoption)
{
    ui->setupUi(this);
    setupFile = new setupFileHandler(setupFileName,this);
    setupFileHandler * baseSetup = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
    languageset = baseSetup->getSectionValue("Setup","language");
    trans = new translator(languageset);

    setupFileSection = section;
    option = label;
    com = trans->translateit(comment);
    ui->checkBox->setChecked(checked);

    if (checked == true) {
        this->setStyleSheet(css);
        setupFile->setSectionValue(setupFileSection,option,com);
    }

    ui->checkBox->setText(trans->beautifyString(com));
    ui->checkBox->setToolTip(option);
}

scanoption::~scanoption(){
    delete ui;
}


void scanoption::slot_checkboxClicked(){
    if (ui->checkBox->isChecked() == false) {
        setupFile->removeKeyword(setupFileSection,option);
        this->setStyleSheet("");
    } else {
        setupFile->setSectionValue(setupFileSection,option,com);
        this->setStyleSheet(css);
    }

    emit valuechanged();
}
