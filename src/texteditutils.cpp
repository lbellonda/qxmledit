/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
 *    as indicated in the AUTHORS file                                    *
 *  lbellonda _at_ gmail.com                                              *
 *                                                                        *
 * This library is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU Library General Public            *
 * License as published by the Free Software Foundation; either           *
 * version 2 of the License, or (at your option) any later version.       *
 *                                                                        *
 * This library is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 * Library General Public License for more details.                       *
 *                                                                        *
 * You should have received a copy of the GNU Library General Public      *
 * License along with this library; if not, write to the                  *
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,       *
 * Boston, MA  02110-1301  USA                                            *
 **************************************************************************/
#include "texteditutils.h"
#include <QTextCursor>

void TextEditUtils::gotoPos(QTextEdit *textEdit, const int column, const int row)
{
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, row) ;
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column) ;
    textEdit->setTextCursor(cursor);
}

void TextEditUtils::hiliteCurrentPos(QTextEdit *textEdit)
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    QColor lineColor = QColor(Qt::yellow).lighter(160);

    selection.format.setBackground(lineColor);
    selection.format.setFontWeight(QFont::Black);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textEdit->textCursor();
    if(selection.cursor.hasSelection()) {
        selection.cursor.clearSelection();
    }
    extraSelections.append(selection);
    textEdit->setExtraSelections(extraSelections);
}
