/***************************************************************************
 *   Copyright (C) 2012 by Joerg Zopes                                     *
 *   joerg.zopes@gmx.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "setupfilehandler.h"

/********************************************************************
 * setupFileHandler                                                 *
 * Parameter    : None                                              *
 * Return Value : None                                              *
 * Description  : Standard Costructor                               *
 ********************************************************************/
setupFileHandler::setupFileHandler(QObject *parent) : QObject(parent) {
}

/********************************************************************
 * setupFileHandler                                                 *
 * Parameter    : QString (filename of the setup file               *
 * Return Value : None                                              *
 * Description  : overloaded standard costructor                    *
 ********************************************************************/
setupFileHandler::setupFileHandler(QString filename, QObject *parent) : QObject(parent) {
    setSetupFileName(filename);
}

/********************************************************************
 * setSetupFileName                                                 *
 * Parameter    : QString                                           *
 * Return Value : None                                              *
 * Description  : Function to set the filename for the setup file   *
 ********************************************************************/
void setupFileHandler::setSetupFileName(QString filename){
QFile m_file(filename);

    setupFileName = filename;

    if (!m_file.exists()){
        QString m_path = filename.left(filename.lastIndexOf("/"));
        QDir * m_tempDir = new QDir(m_path);
        m_tempDir->mkpath(m_path);
        if (m_file.open(QIODevice::WriteOnly|QIODevice::Text)){
            QTextStream m_stream(&m_file);
            Qt::endl(m_stream);
            m_file.close();
            m_file.flush();
        }
    }
}

/********************************************************************
 * getSetupFileName                                                 *
 * Parameter    : None                                              *
 * Return Value : QString (Filename of the setup file)              *
 * Description  : Returns the name of the setup file                *
 ********************************************************************/
QString setupFileHandler::getSetupFileName(){
    return setupFileName;
}

/********************************************************************
 * setSectionValue                                                  *
 * Parameter    : QString, QString, const char*, bool               *
 * Return Value : None                                              *
 * Description  : Set the value in the given Section and for the    *
 *                given keyword. If the section does not exist it   *
 *                is created. If the keyword does not exist it is   *
 *                created. If the keyword exists the overwrite bool *
 *                controls wether the value is changed or not.      *
 ********************************************************************/
void setupFileHandler::setSectionValue(QString section, QString keyword, const char *value, bool overwrite){
    if (keywordExists(section,keyword)){
        if (overwrite) setSectionValue(section,keyword,(QString)value);
    } else {
        setSectionValue(section,keyword,(QString)value);
    }
}

/********************************************************************
 * setSectionValue                                                  *
 * Parameter    : QString, QString, QString                         *
 * Return Value : None                                              *
 * Description  : Set the value in the given Section and for the    *
 *                given keyword. If the section does not exist it   *
 *                created. If the keyword does not exist it is      *
 *                created.                                          *
 ********************************************************************/
void setupFileHandler::setSectionValue(QString section, QString keyword, QString value, bool overwrite){
bool m_doit = false;

    if (keywordExists(section,keyword)){
        m_doit = overwrite;
    } else {
        m_doit = true;
    }

    if (m_doit) {
        readSetupFile();
        QString m_tempSection = getSection(section).trimmed();

            // Check for multiline content and replace \n by <!nl>
            value.replace("\n","<!nl>");

            if (m_tempSection == "") {
                setupFileContent = setupFileContent + "\n[" + section + "]\n" + keyword + "=" + value + "\n";
            } else {
                QString m_sectionReminder = m_tempSection;
                QStringList m_list = m_tempSection.split("\n");
                QString m_rc;
                QString m_line;
                bool m_found = false;

                for (int i = 0; i < m_list.count(); i++){
                    m_line = m_list.at(i);
                    if (m_line.indexOf(keyword+"=") == 0){
                       m_line = m_line.left(m_line.indexOf("=")) + "=" + value;
                       m_found = true;
                    }
                    if (m_line != "") m_rc = m_rc + m_line + "\n";
                }
                if (!m_found){
                    m_rc = m_rc + keyword + "=" + value + "\n";
                }

                setupFileContent.replace(m_sectionReminder,m_rc.trimmed());
            }
            writeSetupFile();
    }
}

/********************************************************************
 * setSectionValue                                                  *
 * Parameter    : QString, QString, bool                            *
 * Return Value : None                                              *
 * Description  : overloaded function.                              *
 ********************************************************************/
void setupFileHandler::setSectionValue(QString section, QString keyword, bool tempValue, bool overwrite){
QString m_value;
    tempValue?m_value="true":m_value="false";

    setSectionValue(section,keyword,m_value, overwrite);
}

/********************************************************************
 * setSectionValue                                                  *
 * Parameter    : QString, QString, qint64                          *
 * Return Value : None                                              *
 * Description  : overloaded function.                              *
 ********************************************************************/
void setupFileHandler::setSectionValue(QString section, QString keyword, qint64 tempValue, bool overwrite){
QString m_value;
    m_value = QString::number(tempValue);

    setSectionValue(section,keyword,m_value,overwrite);
}

/********************************************************************
 * setSectionValue                                                  *
 * Parameter    : QString, QString, int                             *
 * Return Value : None                                              *
 * Description  : overloaded function.                              *
 ********************************************************************/
void setupFileHandler::setSectionValue(QString section, QString keyword, int tempValue, bool overwrite){
QString m_value;
    m_value = QString::number(tempValue);

    setSectionValue(section,keyword,m_value, overwrite);
}

/********************************************************************
 * setSectionValue                                                  *
 * Parameter    : QString, QString, double                          *
 * Return Value : None                                              *
 * Description  : overloaded function.                              *
 ********************************************************************/
void setupFileHandler::setSectionValue(QString section, QString keyword, double tempValue, bool overwrite){
QString m_value;
    m_value = QString::number(tempValue);

    setSectionValue(section,keyword,m_value, overwrite);
}

/********************************************************************
 * getSectionValue                                                  *
 * Parameter    : QString, QString                                  *
 * Return Value : QString                                           *
 * Description  : Returns the value for the given section and the   *
 *                the given keyword                                 *
 ********************************************************************/
QString setupFileHandler::getSectionValue(QString section, QString keyword){
readSetupFile();
QString m_tempSection = getSection(section).trimmed();
QString m_rc = "";

    if (m_tempSection != "") {
        QStringList m_list = m_tempSection.split("\n");
        QString m_line;

        for (int i = 0; i < m_list.count(); i++){
            m_line = m_list.at(i);
            if (m_line.indexOf(keyword+"=") == 0){
               m_rc = m_line.mid(m_line.indexOf("=") + 1);
            }
        }
    }
    // replace encoded \n (<!nl>) by newline (\n)
    m_rc.replace("<!nl>","\n");
    return m_rc;
}

/********************************************************************
 * getSectionBoolValue                                              *
 * Parameter    : QString, QString                                  *
 * Return Value : bool                                              *
 * Description  : Returns the value for the given section and the   *
 *                the given keyword                                 *
 ********************************************************************/
bool setupFileHandler::getSectionBoolValue(QString section, QString keyword){
readSetupFile();
QString m_tempSection = getSection(section).trimmed();
QString m_rc = "";
bool m_boolrc;

    if (m_tempSection != "") {
        QStringList m_list = m_tempSection.split("\n");
        QString m_line;

        for (int i = 0; i < m_list.count(); i++){
            m_line = m_list.at(i);
            if (m_line.indexOf(keyword+"=") == 0){
               m_rc = m_line.mid(m_line.indexOf("=") + 1);
            }
        }
    }

    m_rc == "true"?m_boolrc=true:m_boolrc=false;
    return m_boolrc;
}

/********************************************************************
 * getSectionIntValue                                               *
 * Parameter    : QString, QString                                  *
 * Return Value : qint64                                            *
 * Description  : Returns the value for the given section and the   *
 *                the given keyword                                 *
 ********************************************************************/
qint64 setupFileHandler::getSectionIntValue(QString section, QString keyword){
readSetupFile();
QString m_tempSection = getSection(section).trimmed();
QString m_rc = "";
qint64 m_intrc = 0;

    if (m_tempSection != "") {
        QStringList m_list = m_tempSection.split("\n");
        QString m_line;

        for (int i = 0; i < m_list.count(); i++){
            m_line = m_list.at(i);
            if (m_line.indexOf(keyword+"=") == 0){
               m_rc = m_line.mid(m_line.indexOf("=") + 1);
            }
        }
    }

    m_intrc = m_rc.toULongLong();
    return m_intrc;
}

/********************************************************************
 * getSectionIntValue                                               *
 * Parameter    : QString, QString                                  *
 * Return Value : double                                            *
 * Description  : Returns the value for the given section and the   *
 *                the given keyword                                 *
 ********************************************************************/
double setupFileHandler::getSectionDoubleValue(QString section, QString keyword){
readSetupFile();
QString m_tempSection = getSection(section).trimmed();
QString m_rc = "";
double m_doublerc = 0;

    if (m_tempSection != "") {
        QStringList m_list = m_tempSection.split("\n");
        QString m_line;

        for (int i = 0; i < m_list.count(); i++){
            m_line = m_list.at(i);
            if (m_line.indexOf(keyword+"=") == 0){
               m_rc = m_line.mid(m_line.indexOf("=") + 1);
            }
        }
    }

    m_doublerc = m_rc.toDouble();
    return m_doublerc;
}

/********************************************************************
 * getSection                                                       *
 * Parameter    : QString                                           *
 * Return Value : QString                                           *
 * Description  : Returns the complete section for the given ID     *
 ********************************************************************/
QString setupFileHandler::getSection(QString sectionID){
QStringList m_list = setupFileContent.split("\n");
QString m_rc;
QString m_line;
bool m_sectionFlag = false;

    for (int i = 0; i < m_list.count(); i++){
        m_line = m_list.at(i);
        if ((m_line.left(1) == "[") && (m_line.indexOf("["+sectionID+"]") == -1)) m_sectionFlag = false;
        if (m_line.indexOf("["+sectionID+"]") == 0){
            m_sectionFlag = true;
        }
        if (m_sectionFlag == true){
            m_rc = m_rc + m_line + "\n";
        }
    }
    m_rc = m_rc;
    return m_rc;
}

/********************************************************************
 * readSetupFile                                                    *
 * Parameter    : None                                              *
 * Return Value : None                                              *
 * Description  : Read the complete setup file into a global var    *
 ********************************************************************/
void setupFileHandler::readSetupFile(){
QFile m_file(setupFileName);
    if (m_file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QTextStream m_stream(&m_file);
        setupFileContent = m_stream.readAll().toLocal8Bit().constData();
        if (setupFileContent.right(1) != "\n") setupFileContent = setupFileContent + "\n";
        m_file.close();
    }
}

/********************************************************************
 * writeSetupFile                                                   *
 * Parameter    : None                                              *
 * Return Value : None                                              *
 * Description  : Write the setup data to file                      *
 ********************************************************************/
void setupFileHandler::writeSetupFile(){
QFile m_file(setupFileName);

    QFileDevice::Permissions m_p = m_file.permissions();

    m_file.remove();
    if (m_file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QTextStream m_stream(&m_file);
        if (setupFileContent.indexOf("\n\n") != -1) setupFileContent.replace("\n\n\n","\n\n");
        m_stream << setupFileContent.trimmed();
        Qt::endl(m_stream);
        do {
        } while (!m_file.flush());
        m_file.close();
        m_file.setPermissions(m_p);
    }
}

/********************************************************************
 * removeSection                                                    *
 * Parameter    : QString                                           *
 * Return Value : None                                              *
 * Description  : Removes the complete section with all keywords for*
 *                the given section                                 *
 ********************************************************************/
void setupFileHandler::removeSection(QString section){
readSetupFile();
QString m_tempSection = getSection(section).trimmed();
    setupFileContent.replace(m_tempSection,"");

    writeSetupFile();
}

/********************************************************************
 * removeKeyword                                                    *
 * Parameter    : QString, QString                                  *
 * Return Value : None                                              *
 * Description  : Removes the given keyword from the given section  *
 ********************************************************************/
void setupFileHandler::removeKeyword(QString section, QString keyword){
readSetupFile();
QString m_tempSection = getSection(section).trimmed();

    if (m_tempSection != "") {
        QString m_sectionReminder = m_tempSection;
        QStringList m_list = m_tempSection.split("\n");
        QString m_rc;
        QString m_line;

        for (int i = 0; i < m_list.count(); i++){
            m_line = m_list.at(i);
            if (m_line.indexOf(keyword+"=") != 0){
                m_rc = m_rc + m_line + "\n";
            }
        }
        setupFileContent.replace(m_sectionReminder,m_rc.trimmed());
    }
    writeSetupFile();
}

/********************************************************************
 * clearSetupFile                                                   *
 * Parameter    : None                                              *
 * Return Value : None                                              *
 * Description  : Clears, removes file content                      *
 ********************************************************************/
void setupFileHandler::clearSetupFile()
{
    setupFileContent = "";
    writeSetupFile();
}

/********************************************************************
 * getSectionNames                                                  *
 * Parameter    : None                                              *
 * Return Value : QStringList                                       *
 * Description  : Returns the names of the sections                 *
 ********************************************************************/
QStringList setupFileHandler::getSectionNames(){
QStringList m_sections;
QString m_line;
int m_index;

    readSetupFile();
    do {
        m_index = setupFileContent.indexOf("\n");
        m_line = setupFileContent.mid(0,m_index);
        if ((m_line.at(0) == '[') && (m_line.right(1) == "]")){
            m_line = m_line.mid(1,m_line.length()-2);
            m_sections << m_line;
        }
        setupFileContent = setupFileContent.mid(m_index+1);
    } while (setupFileContent != "");
    return m_sections;
}

/********************************************************************
 * getSectionNames                                                  *
 * Parameter    : None                                              *
 * Return Value : QStringList                                       *
 * Description  : Returns the filtered names of the sections        *
 ********************************************************************/
QStringList setupFileHandler::getSectionNames(QStringList excludeList){
QStringList m_sections;
QString m_line;
int m_index;

    readSetupFile();
    do {
        m_index = setupFileContent.indexOf("\n");
        m_line = setupFileContent.mid(0,m_index);
        if ((m_line.at(0) == '[') && (m_line.right(1) == "]")){
            m_line = m_line.mid(1,m_line.length()-2);
            if (excludeList.indexOf(m_line) == -1) m_sections << m_line;
        }
        setupFileContent = setupFileContent.mid(m_index+1);
    } while (setupFileContent != "");
    return m_sections;
}

/********************************************************************
 * getSectionNames                                                  *
 * Parameter    : None                                              *
 * Return Value : QStringList                                       *
 * Description  : Returns the filtered names of the sections        *
 ********************************************************************/
QStringList setupFileHandler::getSectionNames(QString excludeString){
QStringList m_sections;
QStringList m_excludeList = excludeString.split(",");
QString m_line;
int m_index;

    readSetupFile();
    do {
        m_index = setupFileContent.indexOf("\n");
        m_line = setupFileContent.mid(0,m_index);
        if ((m_line.at(0) == '[') && (m_line.right(1) == "]")){
            m_line = m_line.mid(1,m_line.length()-2);
            if (m_excludeList.indexOf(m_line) == -1) m_sections << m_line;
        }
        setupFileContent = setupFileContent.mid(m_index+1);
    } while (setupFileContent != "");
    return m_sections;
}

/********************************************************************
 * getKeywords                                                      *
 * Parameter    : QString,                                          *
 * Return Value : QStringList                                       *
 * Description  : Removes the given keyword from the given section  *
 ********************************************************************/
QStringList setupFileHandler::getKeywords(QString section){
readSetupFile();
QString m_tempSection = getSection(section).trimmed();
QStringList m_rc;


    if (m_tempSection != "") {
        QStringList m_list = m_tempSection.split("\n");
        QString m_line;

        for (int i = 1; i < m_list.count(); i++){
            m_line = m_list.at(i);
            if (m_line.indexOf("=") != -1){
                m_rc << m_line.left(m_line.indexOf("="));
            }
        }
    }
    return m_rc;
}

/********************************************************************
 * getfreeFloaterValue                                              *
 * Parameter    : QString,                                          *
 * Return Value : QString                                           *
 * Description  : Returns the value for the given keyword           *
 ********************************************************************/
QString setupFileHandler::getFreeFloaterValue(QString keyword){
readSetupFile();
QString m_keywordValue;
int m_from,m_to;

    setupFileContent = setupFileContent.left(setupFileContent.indexOf("[")-2);
    m_from = setupFileContent.indexOf(keyword);
    m_from = setupFileContent.indexOf("=",m_from) + 1;
    m_to = setupFileContent.indexOf("\n",m_from);
    m_to = m_to - m_from;
    m_keywordValue = setupFileContent.mid(m_from,m_to);

    return m_keywordValue;
}


/********************************************************************
 * getSingleLineValue                                               *
 * Parameter    : QString,                                          *
 * Return Value : QString                                           *
 * Description  : Returns the value for the given sing line keyword *
 ********************************************************************/
QString setupFileHandler::getSingleLineValue(QString keyword){
    readSetupFile();
    QString m_KeywordValue = "";
    QStringList m_lines = setupFileContent.split("\n");
    QString m_line = "";

    for (int x = 0; x < m_lines.length(); x++) {
        m_line = m_lines[x];
        if (m_line.indexOf(keyword + " ") == 0) {
            m_KeywordValue = m_line.mid(m_line.indexOf(" ") + 1);
        }
    }

    return m_KeywordValue;
}


/********************************************************************
 * singleLineExists                                                 *
 * Parameter    : QString,                                          *
 * Return Value : bool                                              *
 * Description  : Checks if the given single line value exists      *
 ********************************************************************/
bool setupFileHandler::singleLineExists(QString keyword){
    readSetupFile();
    bool m_rc = false;
    QStringList m_lines = setupFileContent.split("\n");
    QString m_line = "";

    for (int x = 0; x < m_lines.length(); x++) {
        m_line = m_lines[x];
        if (m_line.indexOf(keyword + " ") == 0) m_rc = true;
    }

    return m_rc;
}


/********************************************************************
 * removeSingleLine                                                 *
 * Parameter    : QString,                                          *
 * Return Value :                                                   *
 * Description  : Removes the single Line Value from the file       *
 ********************************************************************/
void setupFileHandler::removeSingleLine(QString keyword, QString value){
    readSetupFile();
    QStringList m_lines = setupFileContent.split("\n");
    QString m_line = "";
    QString m_newContent = "";

    for (int x = 0; x < m_lines.length(); x++) {
        m_line = m_lines[x];
        if (m_line.indexOf(keyword + " " + value) != 0) {
            if (m_newContent == "") m_newContent = m_line; else m_newContent = m_newContent + "\n" + m_line;
        }
    }

    setupFileContent = m_newContent;
    writeSetupFile();
}


/********************************************************************
 * setSingleLineValue                                               *
 * Parameter    : QString,                                          *
 * Return Value :                                                   *
 * Description  : set a new value for the single line value         *
 ********************************************************************/
void setupFileHandler::setSingleLineValue(QString keyword, QString value){
    readSetupFile();
    QString m_prev;
    QString m_rest;
    int m_start, m_stop;

    if (value.right(0) != "\n") value = value + "\n";

    m_start = setupFileContent.indexOf(keyword + " ");


    if (m_start > -1) {
        m_stop = setupFileContent.indexOf("\n",m_start);
        m_prev = setupFileContent.mid(0,m_start);
        m_rest = setupFileContent.mid(m_stop + 1);
        setupFileContent = m_prev + keyword + " " + value + m_rest;
    } else {
        setupFileContent = setupFileContent + keyword + " " + value;
    }
    writeSetupFile();
}

/********************************************************************
 * addSingleLineValue                                               *
 * Parameter    : QString,                                          *
 * Return Value :                                                   *
 * Description  : set a new value for the single line value         *
 ********************************************************************/
void setupFileHandler::addSingleLineValue(QString keyword, QString value)
{
    readSetupFile();
    if (setupFileContent.indexOf(keyword + " " + value) == -1) setupFileContent = setupFileContent + keyword + " " + value;
    writeSetupFile();
}


/********************************************************************
 * setFreeFloaterValue                                              *
 * Parameter    : QString, QString, bool                            *
 * Return Value : None                                              *
 * Description  : Set the value in the given FreeFloater. If the    *
 *                keyword does not exist it is created. If the      *
 *                keyword exists the overwrite bool controls wether *
 *                the value is changed or not                       *
 ********************************************************************/
void setupFileHandler::setFreeFloaterValue(QString keyword, QString value, bool overwrite){
bool m_doit = false;

    if (freeFloaterExists(keyword)){
        m_doit = overwrite;
    } else {
        m_doit = true;
    }

    if (m_doit) {
        readSetupFile();
        QString m_tempSection = setupFileContent.left(setupFileContent.indexOf("[")).trimmed();
        QString m_newFreeFloaterSection;

            if (m_tempSection == "") {
                setupFileContent = m_newFreeFloaterSection + "\n" + keyword + "=" + value + "\n";
            } else {
                QString m_sectionReminder = m_tempSection;
                QStringList m_list = m_tempSection.split("\n");
                QString m_rc;
                QString m_line;
                bool m_found = false;

                for (int i = 0; i < m_list.count(); i++){
                    m_line = m_list.at(i);
                    if (m_line.indexOf(keyword+"=") == 0){
                       m_line = m_line.left(m_line.indexOf("=")) + "=" + value;
                       m_found = true;
                    }
                    if (m_line != "") m_rc = m_rc + m_line + "\n";
                }
                if (!m_found){
                    m_rc = m_rc + keyword + "=" + value + "\n";
                }

                setupFileContent.replace(m_sectionReminder,m_rc.trimmed());
            }

            writeSetupFile();
    }
}

/********************************************************************
 * sectionExists                                                    *
 * Parameter    : QString                                           *
 * Return Value : bool                                              *
 * Description  : Returns true if the given section exists otherwise*
 *                returns false                                     *
 ********************************************************************/
bool setupFileHandler::sectionExists(QString section){
readSetupFile();
QString m_sectionContent = getSection(section).trimmed();
bool rc;

    if (m_sectionContent != "") {
        rc=true;
    } else {
        rc=false;
    }

    return rc;
}

/********************************************************************
 * keywordExists                                                    *
 * Parameter    : QString                                           *
 * Return Value : bool                                              *
 * Description  : Returns true if the given keyword exists otherwise*
 *                returns false                                     *
 ********************************************************************/
bool setupFileHandler::keywordExists(QString section, QString keyword){
readSetupFile();
QString m_sectionContent = getSection(section);
bool m_rc = true;

    if (m_sectionContent == "") m_rc = false;

    if (m_rc) {
        m_rc = false;
        QStringList m_lines=m_sectionContent.split("\n");
        foreach(QString line, m_lines){
            if (line.indexOf(keyword+"=") == 0) m_rc = true;
        }
    }

    return m_rc;
}

/********************************************************************
 * freeFloaterExists                                                *
 * Parameter    : QString                                           *
 * Return Value : bool                                              *
 * Description  : Returns true if the given keyword exists otherwise*
 *                returns false                                     *
 ********************************************************************/
bool setupFileHandler::freeFloaterExists(QString keyword){
readSetupFile();
bool m_rc = false;

    setupFileContent = setupFileContent.left(setupFileContent.indexOf("[")-2);
    QStringList m_lines=setupFileContent.split("\n");
    foreach(QString line,m_lines){
        if (line.indexOf(keyword) == 0) m_rc = true;
    }

    return m_rc;
}
