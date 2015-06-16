/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>

class Regola;

namespace Ui
{
class InfoDialog;
}

class InfoReferences
{
    QString ns;
    QString url;
    QString role;
};


class InfoDialog : public QDialog
{
    Q_OBJECT

    Regola *_regola;

    void setEncodingInfo();
    void setNamespaceInfo();
    void setSchemaReferenceInfo();
    void setupInfo();
    void insSchemaReference(const QString &ns, const QString &uri, const QString &role);
    void insNamespace(const QString &ns, const QString &uri, const QString &role);
    void setFileInfo();
public:
    explicit InfoDialog(QWidget *parent, Regola *regola);
    ~InfoDialog();

private:
    Ui::InfoDialog *ui;
};

void showInfo(QWidget *parent, Regola *regola);

#endif // INFODIALOG_H
