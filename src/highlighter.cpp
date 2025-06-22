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
     HighlightingRule m_rule;

     keywordFormat.setForeground(Qt::darkYellow);
     keywordFormat.setFontWeight(QFont::Bold);

     singleLineCommentFormat.setForeground(Qt::blue);
     singleLineCommentFormat.setBackground(Qt::white);
     m_rule.pattern = QRegExp("^Downloading.*|^ClamInotif:|.-> .*|^/.*|^Database updated.*|^ClamAV update process started.*|^---.*|.Testing database:*|.Database Test passed.");
     singleLineCommentFormat.setFontWeight(QFont::Normal);
     m_rule.format = singleLineCommentFormat;
     highlightingRules.append(m_rule);

     singleLineCommentFormat.setForeground(QColor(0x4f,0x7e,0x8a,0xff));
     singleLineCommentFormat.setBackground(Qt::white);
     singleLineCommentFormat.setFontWeight(QFont::Bold);
     m_rule.pattern = QRegExp("^Known viruses:.|^Engine version:.|^Data scanned:.|^Data read:.|^Time:.|^Start Date:.|^End Date:.");
     m_rule.format = singleLineCommentFormat;
     highlightingRules.append(m_rule);

     singleLineCommentFormat.setForeground(Qt::darkGreen);
     singleLineCommentFormat.setBackground(Qt::white);
     singleLineCommentFormat.setFontWeight(QFont::Normal);
     m_rule.pattern = QRegExp("LOCAL:|OLE2:|Phishing:|Heuristic:|Structured:|Local:|Limits:|.enabled|.disabled|.watching .*|^daily.cvd.*|^daily.cld.*|^bytecode.cvd.*|^main.cvd.*|^freshclam deamon.*|. OK|^Scanned directories:.|^Scanned files:.|^Infected files: 0");
     m_rule.format = singleLineCommentFormat;
     highlightingRules.append(m_rule);

     singleLineCommentFormat.setForeground(Qt::darkYellow);
     singleLineCommentFormat.setBackground(Qt::white);
     singleLineCommentFormat.setFontWeight(QFont::Normal);
     m_rule.pattern = QRegExp(".Pid file removed.|. Started at.*|. Stopped at.*|.Socket file removed.");
     m_rule.format = singleLineCommentFormat;
     highlightingRules.append(m_rule);

     singleLineCommentFormat.setForeground(Qt::red);
     singleLineCommentFormat.setBackground(Qt::white);
     singleLineCommentFormat.setFontWeight(QFont::Normal);
     m_rule.pattern = QRegExp("^ERROR: ClamCom:|.Empty file|^WARN.*|.FOUND *|.ERROR:.*|.WARNING:.*|^Can't connect to port.*|.Access denied|^Infected files:.|^Total errors:.*");
     m_rule.format = singleLineCommentFormat;
     highlightingRules.append(m_rule);

     multiLineCommentFormat.setForeground(Qt::red);
     singleLineCommentFormat.setBackground(Qt::white);
     singleLineCommentFormat.setFontWeight(QFont::Normal);
     commentStartExpression = QRegExp("START");
     commentEndExpression = QRegExp("ENDE");

}

void highlighter::highlightBlock(const QString &text)
 {
     foreach (const HighlightingRule &rule, highlightingRules) {
         QRegExp m_expression(rule.pattern);
         int index = m_expression.indexIn(text);
         while (index >= 0) {
             int length = m_expression.matchedLength();
             setFormat(index, length, rule.format);
             index = m_expression.indexIn(text, index + length);
         }
     }

     setCurrentBlockState(0);

     int m_startIndex = 0;
     if (previousBlockState() != 1) m_startIndex = commentStartExpression.indexIn(text);

     while (m_startIndex >= 0) {
         int m_endIndex = commentEndExpression.indexIn(text, m_startIndex);
         int m_commentLength;
         if (m_endIndex == -1) {
             setCurrentBlockState(1);
             m_commentLength = text.length() - m_startIndex;
         } else {
             m_commentLength = m_endIndex - m_startIndex
                             + commentEndExpression.matchedLength();
         }
         m_startIndex = commentStartExpression.indexIn(text, m_startIndex + m_commentLength);
    }
}
//
