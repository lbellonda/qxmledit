/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017-2018 by Luca Bellonda and individual contributors  *
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

#include "shortcutsdialog.h"
#include "ui_shortcutsdialog.h"

void ShortcutsDialog::display(QWidget *parent)
{
    ShortcutsDialog dialog(parent);
    dialog.exec();
}

ShortcutsDialog::ShortcutsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShortcutsDialog)
{
    ui->setupUi(this);
    ui->textBrowser->setText(Text);
}

ShortcutsDialog::~ShortcutsDialog()
{
    delete ui;
}

const QString ShortcutsDialog::Text = tr(
        "<html>\
<body>\
<h1>Shortcuts for element creation</h1>\
<hr/>\
<br/>\
<table border='1' cellspacing='0' cellpadding='8' >\
    <tr><th>Operation</td><th>Shortcut</th></tr>\
    <tr><td>Insert</td><td> INS, i</th></tr>\
        <tr><td>Append</td><td> ALT+INS, a</td></tr>\
        <tr><td>Insert Specific</td><td> SHIFT INS, shift i</td></tr>\
        <tr><td>Append Specific</td><td> ALT+SHIFT+INS, shift a</td></tr>\
        <tr><td>Delete</td><td> delete, backspace</td></tr>\
    <tr><td>Properties</td><td> ALT+ENTER, DOUBLE CLICK</td></tr>\
        <tr><td>Spec Properties</td><td> ?, ALT+SHIFT+ENTER, (ALT|CTRL) DOUBLE CLICK</td></tr>\
        <tr><td>Edit text</td><td> t</td></tr>\
        <tr><td>Edit element</td><td> e</td></tr>\
        <tr><td>Edit inner XML</td><td> x</td></tr>\
        <tr><td>Edit inner XML base64</td><td> y</td></tr>\
</table>\
<br/>\
<br/>\
<table border='1' cellspacing='0' cellpadding='8'>\
    <tr>\
        <th>Edit Mode</th>\
        <th>DblClick</th>\
        <th>dc+Shift</th>\
        <th>dc+Ctrl</th>\
        <th>dc+Alt</th>\
        <th>...</th>\
        <th>Enter</th>\
        <th>En+Shift</th>\
        <th>En+Ctrl</th>\
        <th>En+Alt</th>\
    </tr>\
    <tr>\
        <td>Normal</td>\
        <td>Detail</td>\
        <td>Textual(tag/text)</td>\
        <td>Specific</td>\
        <td>Specific</td>\
        <td>...</td>\
        <td>Not used</td>\
        <td>Textual</td>\
        <td>Detail</td>\
        <td>Specific</td>\
    </tr>\
    <tr>\
        <td>Special</td>\
        <td>Special</td>\
        <td>Textual(tag/text)</td>\
        <td>Detail</td>\
        <td>Detail</td>\
        <td>...</td>\
        <td>Not used</td>\
        <td>Textual</td>\
        <td>Specific</td>\
        <td>Detail</td>\
    </tr>\
\
</table>\
</body>\
</html>");
