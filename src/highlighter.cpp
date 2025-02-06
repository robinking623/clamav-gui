/***************************************************************************
 *   Copyright (C) 2012 by Joerg Zopes                                     *
 *   joerg.zopes@linux-specialist.com                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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

#include "highlighter.h"
//
highlighter::highlighter(QTextDocument * parent) : QSyntaxHighlighter(parent)
{
     HighlightingRule rule;

     keywordFormat.setForeground(Qt::darkYellow);
     keywordFormat.setFontWeight(QFont::Bold);

     singleLineCommentFormat.setForeground(Qt::blue);
     singleLineCommentFormat.setBackground(Qt::white);
     rule.pattern = QRegExp("^Downloading.*|^ClamInotif:|.-> .*|^/.*|^Database updated.*|^ClamAV update process started.*|^---.*|.Testing database:*|.Database Test passed.");
     singleLineCommentFormat.setFontWeight(QFont::Normal);
     rule.format = singleLineCommentFormat;
     highlightingRules.append(rule);

     singleLineCommentFormat.setForeground(QColor(0x4f,0x7e,0x8a,0xff));
     singleLineCommentFormat.setBackground(Qt::white);
     singleLineCommentFormat.setFontWeight(QFont::Bold);
     rule.pattern = QRegExp("^Known viruses:.|^Engine version:.|^Data scanned:.|^Data read:.|^Time:.|^Start Date:.|^End Date:.");
     rule.format = singleLineCommentFormat;
     highlightingRules.append(rule);

     singleLineCommentFormat.setForeground(Qt::darkGreen);
     singleLineCommentFormat.setBackground(Qt::white);
     singleLineCommentFormat.setFontWeight(QFont::Normal);
     rule.pattern = QRegExp("LOCAL:|OLE2:|Phishing:|Heuristic:|Structured:|Local:|Limits:|.enabled|.disabled|.watching .*|^daily.cvd.*|^daily.cld.*|^bytecode.cvd.*|^main.cvd.*|^freshclam deamon.*|. OK|^Scanned directories:.|^Scanned files:.|^Infected files: 0");
     rule.format = singleLineCommentFormat;
     highlightingRules.append(rule);

     singleLineCommentFormat.setForeground(Qt::darkYellow);
     singleLineCommentFormat.setBackground(Qt::white);
     singleLineCommentFormat.setFontWeight(QFont::Normal);
     rule.pattern = QRegExp(".Pid file removed.|. Started at.*|. Stopped at.*|.Socket file removed.");
     rule.format = singleLineCommentFormat;
     highlightingRules.append(rule);

     singleLineCommentFormat.setForeground(Qt::red);
     singleLineCommentFormat.setBackground(Qt::white);
     singleLineCommentFormat.setFontWeight(QFont::Normal);
     rule.pattern = QRegExp("^ERROR: ClamCom:|.Empty file|^WARN.*|.FOUND *|.ERROR:.*|.WARNING:.*|^Can't connect to port.*|.Access denied|^Infected files:.|^Total errors:.*");
     rule.format = singleLineCommentFormat;
     highlightingRules.append(rule);

     multiLineCommentFormat.setForeground(Qt::red);
     singleLineCommentFormat.setBackground(Qt::white);
     singleLineCommentFormat.setFontWeight(QFont::Normal);
     commentStartExpression = QRegExp("START");
     commentEndExpression = QRegExp("ENDE");

}

void highlighter::highlightBlock(const QString &text)
 {
     foreach (const HighlightingRule &rule, highlightingRules) {
         QRegExp expression(rule.pattern);
         int index = expression.indexIn(text);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = expression.indexIn(text, index + length);
         }
     }
     
     setCurrentBlockState(0);

     int startIndex = 0;
     if (previousBlockState() != 1) startIndex = commentStartExpression.indexIn(text);
     
     while (startIndex >= 0) {
         int endIndex = commentEndExpression.indexIn(text, startIndex);
         int commentLength;
         if (endIndex == -1) {
             setCurrentBlockState(1);
             commentLength = text.length() - startIndex;
         } else {
             commentLength = endIndex - startIndex
                             + commentEndExpression.matchedLength();
         }
         startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
//
