/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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


#ifndef EDITNAMESPACEDIALOG_H
#define EDITNAMESPACEDIALOG_H

#include <QDialog>

class UserNamespace ;

namespace Ui
{
class EditNamespaceDialog;
}

class EditNamespaceDialog : public QDialog
{
    Q_OBJECT
    UserNamespace *_ns;

public:
    explicit EditNamespaceDialog(QWidget *parent, UserNamespace *ns);
    ~EditNamespaceDialog();

    void accept();

private:
    Ui::EditNamespaceDialog *ui;

    void loadData();
    QSet<QString> tagsFromUI();
    bool getDataFromUI();
    void enableButtons();
    bool canSave();
    void addItem(const QString &text);

private slots:
    void on_cmdAddP_clicked();
    void on_cmdDelP_clicked();
    void on_cmdModP_clicked();
    void on_prefixes_currentRowChanged(int currentRow);
    void on_name_textEdited(const QString & /*text*/);
    void on_description_textEdited(const QString & /*text*/);
    void on_defaultPrefix_textEdited(const QString & /*text*/);
    void on_uri_textEdited(const QString & /*text*/);
    void on_schemaLocation_textEdited(const QString & /*text*/);
};

#endif // EDITNAMESPACEDIALOG_H
