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
setupFileHandler::setupFileHandler(QObject* parent) : QObject(parent) {}

/********************************************************************
 * setupFileHandler                                                 *
 * Parameter    : QString (filename of the setup file               *
 * Return Value : None                                              *
 * Description  : overloaded standard costructor                    *
 ********************************************************************/
setupFileHandler::setupFileHandler(QString filename, QObject* parent) : QObject(parent)
{
    setSetupFileName(filename);
}

/********************************************************************
 * setSetupFileName                                                 *
 * Parameter    : QString                                           *
 * Return Value : None                                              *
 * Description  : Function to set the filename for the setup file   *
 ********************************************************************/
void setupFileHandler::setSetupFileName(QString filename)
{
    QFile file(filename);

    m_setupFileName = filename;

    if (!file.exists()) {
        QString path = filename.left(filename.lastIndexOf("/"));
        QDir* tempDir = new QDir(path);
        tempDir->mkpath(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            Qt::endl(stream);
            file.close();
            file.flush();
        }
    }
}

/********************************************************************
 * getSetupFileName                                                 *
 * Parameter    : None                                              *
 * Return Value : QString (Filename of the setup file)              *
 * Description  : Returns the name of the setup file                *
 ********************************************************************/
QString setupFileHandler::getSetupFileName()
{
    return m_setupFileName;
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
void setupFileHandler::setSectionValue(QString section, QString keyword, const char* value, bool overwrite)
{
    if (keywordExists(section, keyword)) {
        if (overwrite)
            setSectionValue(section, keyword, (QString)value);
    }
    else {
        setSectionValue(section, keyword, (QString)value);
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
void setupFileHandler::setSectionValue(QString section, QString keyword, QString value, bool overwrite)
{
    bool doit = false;

    if (keywordExists(section, keyword)) {
        doit = overwrite;
    }
    else {
        doit = true;
    }

    if (doit) {
        readSetupFile();
        QString tempSection = getSection(section).trimmed();

        // Check for multiline content and replace \n by <!nl>
        value.replace("\n", "<!nl>");

        if (tempSection == "") {
            m_setupFileContent = m_setupFileContent + "\n[" + section + "]\n" + keyword + "=" + value + "\n";
        }
        else {
            QString sectionReminder = tempSection;
            QStringList list = tempSection.split("\n");
            QString rc;
            QString line;
            bool found = false;

            for (int i = 0; i < list.count(); i++) {
                line = list.at(i);
                if (line.indexOf(keyword + "=") == 0) {
                    line = line.left(line.indexOf("=")) + "=" + value;
                    found = true;
                }
                if (line != "")
                    rc = rc + line + "\n";
            }
            if (!found) {
                rc = rc + keyword + "=" + value + "\n";
            }

            m_setupFileContent.replace(sectionReminder, rc.trimmed());
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
void setupFileHandler::setSectionValue(QString section, QString keyword, bool tempValue, bool overwrite)
{
    QString value;
    tempValue ? value = "true" : value = "false";

    setSectionValue(section, keyword, value, overwrite);
}

/********************************************************************
 * setSectionValue                                                  *
 * Parameter    : QString, QString, qint64                          *
 * Return Value : None                                              *
 * Description  : overloaded function.                              *
 ********************************************************************/
void setupFileHandler::setSectionValue(QString section, QString keyword, qint64 tempValue, bool overwrite)
{
    QString value;
    value = QString::number(tempValue);

    setSectionValue(section, keyword, value, overwrite);
}

/********************************************************************
 * setSectionValue                                                  *
 * Parameter    : QString, QString, int                             *
 * Return Value : None                                              *
 * Description  : overloaded function.                              *
 ********************************************************************/
void setupFileHandler::setSectionValue(QString section, QString keyword, int tempValue, bool overwrite)
{
    QString value;
    value = QString::number(tempValue);

    setSectionValue(section, keyword, value, overwrite);
}

/********************************************************************
 * setSectionValue                                                  *
 * Parameter    : QString, QString, double                          *
 * Return Value : None                                              *
 * Description  : overloaded function.                              *
 ********************************************************************/
void setupFileHandler::setSectionValue(QString section, QString keyword, double tempValue, bool overwrite)
{
    QString value;
    value = QString::number(tempValue);

    setSectionValue(section, keyword, value, overwrite);
}

/********************************************************************
 * getSectionValue                                                  *
 * Parameter    : QString, QString                                  *
 * Return Value : QString                                           *
 * Description  : Returns the value for the given section and the   *
 *                the given keyword                                 *
 ********************************************************************/
QString setupFileHandler::getSectionValue(QString section, QString keyword)
{
    readSetupFile();
    QString tempSection = getSection(section).trimmed();
    QString rc = "";

    if (tempSection != "") {
        QStringList list = tempSection.split("\n");
        QString line;

        for (int i = 0; i < list.count(); i++) {
            line = list.at(i);
            if (line.indexOf(keyword + "=") == 0) {
                rc = line.mid(line.indexOf("=") + 1);
            }
        }
    }
    // replace encoded \n (<!nl>) by newline (\n)
    rc.replace("<!nl>", "\n");
    return rc;
}

/********************************************************************
 * getSectionBoolValue                                              *
 * Parameter    : QString, QString                                  *
 * Return Value : bool                                              *
 * Description  : Returns the value for the given section and the   *
 *                the given keyword                                 *
 ********************************************************************/
bool setupFileHandler::getSectionBoolValue(QString section, QString keyword)
{
    readSetupFile();
    QString tempSection = getSection(section).trimmed();
    QString rc = "";
    bool boolrc;

    if (tempSection != "") {
        QStringList list = tempSection.split("\n");
        QString line;

        for (int i = 0; i < list.count(); i++) {
            line = list.at(i);
            if (line.indexOf(keyword + "=") == 0) {
                rc = line.mid(line.indexOf("=") + 1);
            }
        }
    }

    rc == "true" ? boolrc = true : boolrc = false;
    return boolrc;
}

/********************************************************************
 * getSectionIntValue                                               *
 * Parameter    : QString, QString                                  *
 * Return Value : qint64                                            *
 * Description  : Returns the value for the given section and the   *
 *                the given keyword                                 *
 ********************************************************************/
qint64 setupFileHandler::getSectionIntValue(QString section, QString keyword)
{
    readSetupFile();
    QString tempSection = getSection(section).trimmed();
    QString rc = "";
    qint64 intrc = 0;

    if (tempSection != "") {
        QStringList list = tempSection.split("\n");
        QString line;

        for (int i = 0; i < list.count(); i++) {
            line = list.at(i);
            if (line.indexOf(keyword + "=") == 0) {
                rc = line.mid(line.indexOf("=") + 1);
            }
        }
    }

    intrc = rc.toULongLong();
    return intrc;
}

/********************************************************************
 * getSectionIntValue                                               *
 * Parameter    : QString, QString                                  *
 * Return Value : double                                            *
 * Description  : Returns the value for the given section and the   *
 *                the given keyword                                 *
 ********************************************************************/
double setupFileHandler::getSectionDoubleValue(QString section, QString keyword)
{
    readSetupFile();
    QString tempSection = getSection(section).trimmed();
    QString rc = "";
    double doublerc = 0;

    if (tempSection != "") {
        QStringList list = tempSection.split("\n");
        QString line;

        for (int i = 0; i < list.count(); i++) {
            line = list.at(i);
            if (line.indexOf(keyword + "=") == 0) {
                rc = line.mid(line.indexOf("=") + 1);
            }
        }
    }

    doublerc = rc.toDouble();
    return doublerc;
}

/********************************************************************
 * getSection                                                       *
 * Parameter    : QString                                           *
 * Return Value : QString                                           *
 * Description  : Returns the complete section for the given ID     *
 ********************************************************************/
QString setupFileHandler::getSection(QString sectionID)
{
    QStringList list = m_setupFileContent.split("\n");
    QString rc;
    QString line;
    bool sectionFlag = false;

    for (int i = 0; i < list.count(); i++) {
        line = list.at(i);
        if ((line.left(1) == "[") && (line.indexOf("[" + sectionID + "]") == -1))
            sectionFlag = false;
        if (line.indexOf("[" + sectionID + "]") == 0) {
            sectionFlag = true;
        }
        if (sectionFlag == true) {
            rc = rc + line + "\n";
        }
    }
    rc = rc;
    return rc;
}

/********************************************************************
 * readSetupFile                                                    *
 * Parameter    : None                                              *
 * Return Value : None                                              *
 * Description  : Read the complete setup file into a global var    *
 ********************************************************************/
void setupFileHandler::readSetupFile()
{
    QFile file(m_setupFileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        m_setupFileContent = stream.readAll().toLocal8Bit().constData();
        if (m_setupFileContent.right(1) != "\n")
            m_setupFileContent = m_setupFileContent + "\n";
        file.close();
    }
}

/********************************************************************
 * writeSetupFile                                                   *
 * Parameter    : None                                              *
 * Return Value : None                                              *
 * Description  : Write the setup data to file                      *
 ********************************************************************/
void setupFileHandler::writeSetupFile()
{
    QFile file(m_setupFileName);

    QFileDevice::Permissions p = file.permissions();

    file.remove();
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        if (m_setupFileContent.indexOf("\n\n") != -1)
            m_setupFileContent.replace("\n\n\n", "\n\n");
        stream << m_setupFileContent.trimmed();
        Qt::endl(stream);
        do {
        } while (!file.flush());
        file.close();
        file.setPermissions(p);
    }
}

/********************************************************************
 * removeSection                                                    *
 * Parameter    : QString                                           *
 * Return Value : None                                              *
 * Description  : Removes the complete section with all keywords for*
 *                the given section                                 *
 ********************************************************************/
void setupFileHandler::removeSection(QString section)
{
    readSetupFile();
    QString tempSection = getSection(section).trimmed();
    m_setupFileContent.replace(tempSection, "");

    writeSetupFile();
}

/********************************************************************
 * removeKeyword                                                    *
 * Parameter    : QString, QString                                  *
 * Return Value : None                                              *
 * Description  : Removes the given keyword from the given section  *
 ********************************************************************/
void setupFileHandler::removeKeyword(QString section, QString keyword)
{
    readSetupFile();
    QString tempSection = getSection(section).trimmed();

    if (tempSection != "") {
        QString sectionReminder = tempSection;
        QStringList list = tempSection.split("\n");
        QString rc;
        QString line;

        for (int i = 0; i < list.count(); i++) {
            line = list.at(i);
            if (line.indexOf(keyword + "=") != 0) {
                rc = rc + line + "\n";
            }
        }
        m_setupFileContent.replace(sectionReminder, rc.trimmed());
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
    m_setupFileContent = "";
    writeSetupFile();
}

/********************************************************************
 * getSectionNames                                                  *
 * Parameter    : None                                              *
 * Return Value : QStringList                                       *
 * Description  : Returns the names of the sections                 *
 ********************************************************************/
QStringList setupFileHandler::getSectionNames()
{
    QStringList sections;
    QString line;
    int index;

    readSetupFile();
    do {
        index = m_setupFileContent.indexOf("\n");
        line = m_setupFileContent.mid(0, index);
        if ((line.at(0) == '[') && (line.right(1) == "]")) {
            line = line.mid(1, line.length() - 2);
            sections << line;
        }
        m_setupFileContent = m_setupFileContent.mid(index + 1);
    } while (m_setupFileContent != "");
    return sections;
}

/********************************************************************
 * getSectionNames                                                  *
 * Parameter    : None                                              *
 * Return Value : QStringList                                       *
 * Description  : Returns the filtered names of the sections        *
 ********************************************************************/
QStringList setupFileHandler::getSectionNames(QStringList excludeList)
{
    QStringList sections;
    QString line;
    int index;

    readSetupFile();
    do {
        index = m_setupFileContent.indexOf("\n");
        line = m_setupFileContent.mid(0, index);
        if ((line.at(0) == '[') && (line.right(1) == "]")) {
            line = line.mid(1, line.length() - 2);
            if (excludeList.indexOf(line) == -1)
                sections << line;
        }
        m_setupFileContent = m_setupFileContent.mid(index + 1);
    } while (m_setupFileContent != "");
    return sections;
}

/********************************************************************
 * getSectionNames                                                  *
 * Parameter    : None                                              *
 * Return Value : QStringList                                       *
 * Description  : Returns the filtered names of the sections        *
 ********************************************************************/
QStringList setupFileHandler::getSectionNames(QString excludeString)
{
    QStringList sections;
    QStringList excludeList = excludeString.split(",");
    QString line;
    int index;

    readSetupFile();
    do {
        index = m_setupFileContent.indexOf("\n");
        line = m_setupFileContent.mid(0, index);
        if ((line.at(0) == '[') && (line.right(1) == "]")) {
            line = line.mid(1, line.length() - 2);
            if (excludeList.indexOf(line) == -1)
                sections << line;
        }
        m_setupFileContent = m_setupFileContent.mid(index + 1);
    } while (m_setupFileContent != "");
    return sections;
}

/********************************************************************
 * getKeywords                                                      *
 * Parameter    : QString,                                          *
 * Return Value : QStringList                                       *
 * Description  : Removes the given keyword from the given section  *
 ********************************************************************/
QStringList setupFileHandler::getKeywords(QString section)
{
    readSetupFile();
    QString tempSection = getSection(section).trimmed();
    QStringList rc;

    if (tempSection != "") {
        QStringList list = tempSection.split("\n");
        QString line;

        for (int i = 1; i < list.count(); i++) {
            line = list.at(i);
            if (line.indexOf("=") != -1) {
                rc << line.left(line.indexOf("="));
            }
        }
    }
    return rc;
}

/********************************************************************
 * getfreeFloaterValue                                              *
 * Parameter    : QString,                                          *
 * Return Value : QString                                           *
 * Description  : Returns the value for the given keyword           *
 ********************************************************************/
QString setupFileHandler::getFreeFloaterValue(QString keyword)
{
    readSetupFile();
    QString keywordValue;
    int from, to;

    m_setupFileContent = m_setupFileContent.left(m_setupFileContent.indexOf("[") - 2);
    from = m_setupFileContent.indexOf(keyword);
    from = m_setupFileContent.indexOf("=", from) + 1;
    to = m_setupFileContent.indexOf("\n", from);
    to = to - from;
    keywordValue = m_setupFileContent.mid(from, to);

    return keywordValue;
}

/********************************************************************
 * getSingleLineValue                                               *
 * Parameter    : QString,                                          *
 * Return Value : QString                                           *
 * Description  : Returns the value for the given sing line keyword *
 ********************************************************************/
QString setupFileHandler::getSingleLineValue(QString keyword)
{
    readSetupFile();
    QString KeywordValue = "";
    QStringList lines = m_setupFileContent.split("\n");
    QString line = "";

    for (int x = 0; x < lines.length(); x++) {
        line = lines[x];
        if (line.indexOf(keyword + " ") == 0) {
            KeywordValue = line.mid(line.indexOf(" ") + 1);
        }
    }

    return KeywordValue;
}

/********************************************************************
 * singleLineExists                                                 *
 * Parameter    : QString,                                          *
 * Return Value : bool                                              *
 * Description  : Checks if the given single line value exists      *
 ********************************************************************/
bool setupFileHandler::singleLineExists(QString keyword)
{
    readSetupFile();
    bool rc = false;
    QStringList lines = m_setupFileContent.split("\n");
    QString line = "";

    for (int x = 0; x < lines.length(); x++) {
        line = lines[x];
        if (line.indexOf(keyword + " ") == 0)
            rc = true;
    }

    return rc;
}

/********************************************************************
 * removeSingleLine                                                 *
 * Parameter    : QString,                                          *
 * Return Value :                                                   *
 * Description  : Removes the single Line Value from the file       *
 ********************************************************************/
void setupFileHandler::removeSingleLine(QString keyword, QString value)
{
    readSetupFile();
    QStringList lines = m_setupFileContent.split("\n");
    QString line = "";
    QString newContent = "";

    for (int x = 0; x < lines.length(); x++) {
        line = lines[x];
        if (line.indexOf(keyword + " " + value) != 0) {
            if (newContent == "")
                newContent = line;
            else
                newContent = newContent + "\n" + line;
        }
    }

    m_setupFileContent = newContent;
    writeSetupFile();
}

/********************************************************************
 * setSingleLineValue                                               *
 * Parameter    : QString,                                          *
 * Return Value :                                                   *
 * Description  : set a new value for the single line value         *
 ********************************************************************/
void setupFileHandler::setSingleLineValue(QString keyword, QString value)
{
    readSetupFile();
    QString prev;
    QString rest;
    int start, stop;

    if (value.right(0) != "\n")
        value = value + "\n";

    start = m_setupFileContent.indexOf(keyword + " ");

    if (start > -1) {
        stop = m_setupFileContent.indexOf("\n", start);
        prev = m_setupFileContent.mid(0, start);
        rest = m_setupFileContent.mid(stop + 1);
        m_setupFileContent = prev + keyword + " " + value + rest;
    }
    else {
        m_setupFileContent = m_setupFileContent + keyword + " " + value;
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
    if (m_setupFileContent.indexOf(keyword + " " + value) == -1)
        m_setupFileContent = m_setupFileContent + keyword + " " + value;
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
void setupFileHandler::setFreeFloaterValue(QString keyword, QString value, bool overwrite)
{
    bool doit = false;

    if (freeFloaterExists(keyword)) {
        doit = overwrite;
    }
    else {
        doit = true;
    }

    if (doit) {
        readSetupFile();
        QString tempSection = m_setupFileContent.left(m_setupFileContent.indexOf("[")).trimmed();
        QString newFreeFloaterSection;

        if (tempSection == "") {
            m_setupFileContent = newFreeFloaterSection + "\n" + keyword + "=" + value + "\n";
        }
        else {
            QString sectionReminder = tempSection;
            QStringList list = tempSection.split("\n");
            QString rc;
            QString line;
            bool found = false;

            for (int i = 0; i < list.count(); i++) {
                line = list.at(i);
                if (line.indexOf(keyword + "=") == 0) {
                    line = line.left(line.indexOf("=")) + "=" + value;
                    found = true;
                }
                if (line != "")
                    rc = rc + line + "\n";
            }
            if (!found) {
                rc = rc + keyword + "=" + value + "\n";
            }

            m_setupFileContent.replace(sectionReminder, rc.trimmed());
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
bool setupFileHandler::sectionExists(QString section)
{
    readSetupFile();
    QString sectionContent = getSection(section).trimmed();
    bool rc;

    if (sectionContent != "") {
        rc = true;
    }
    else {
        rc = false;
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
bool setupFileHandler::keywordExists(QString section, QString keyword)
{
    readSetupFile();
    QString sectionContent = getSection(section);
    bool rc = true;

    if (sectionContent == "")
        rc = false;

    if (rc) {
        rc = false;
        QStringList lines = sectionContent.split("\n");
        foreach (QString line, lines) {
            if (line.indexOf(keyword + "=") == 0)
                rc = true;
        }
    }

    return rc;
}

/********************************************************************
 * freeFloaterExists                                                *
 * Parameter    : QString                                           *
 * Return Value : bool                                              *
 * Description  : Returns true if the given keyword exists otherwise*
 *                returns false                                     *
 ********************************************************************/
bool setupFileHandler::freeFloaterExists(QString keyword)
{
    readSetupFile();
    bool rc = false;

    m_setupFileContent = m_setupFileContent.left(m_setupFileContent.indexOf("[") - 2);
    QStringList lines = m_setupFileContent.split("\n");
    foreach (QString line, lines) {
        if (line.indexOf(keyword) == 0)
            rc = true;
    }

    return rc;
}
