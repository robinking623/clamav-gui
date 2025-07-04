/***************************************************************************
 *   Copyright (C) 2015 by Joerg Zopes                                     *
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

#ifndef LOGVIEWOBJECT_H
#define LOGVIEWOBJECT_H

#include "ui_logviewobject.h"
#include "partiallogobject.h"
#include "setupfilehandler.h"

#include <QPlainTextEdit>
#include <QMessageBox>
#include <QTextStream>
#include <QDialog>
#include <QFile>

namespace Ui {
class logViewObject;
}

class logViewObject : public QDialog
{
    Q_OBJECT

public:
    explicit logViewObject(QWidget *parent = 0, QString filename = "");
    ~logViewObject() = default;
    void keyPressEvent(QKeyEvent*);
    void closeEvent(QCloseEvent*);

private:
    Ui::logViewObject m_ui;
    QString m_logFileName;
    bool m_contentChanged {false};
    int m_searchPosition {0};
    
    void loadLogFile(QString);

private slots:
    void slot_closeButtonClicked();
    void slot_clearLogButtonClicked();
    void slot_add_remove_highlighter(bool state);

signals:
    void logChanged();
    void logHighlighterChanged(bool);
};

#endif // LOGVIEWOBJECT_H
