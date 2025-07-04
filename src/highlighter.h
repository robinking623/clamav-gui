/***************************************************************************
 *   Copyright (C) 2015 by Joerg Zopes                                     *
 *   joerg.zopes@gmx.de                                                    *
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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H
//
#include <QSyntaxHighlighter>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegExp>
#endif
//
class highlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    highlighter(QTextDocument* parent = 0);

protected:
    void highlightBlock(const QString& text);

private:
    struct HighlightingRule {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> m_highlightingRules;

    QRegExp m_commentStartExpression;
    QRegExp m_commentEndExpression;

    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_classFormat;
    QTextCharFormat m_quotationFormat;
    QTextCharFormat m_functionFormat;
    QTextCharFormat m_multiLineCommentFormat;
    QTextCharFormat m_singleLineCommentFormat;
    QTextCharFormat m_xmlFormat;
};
#endif
