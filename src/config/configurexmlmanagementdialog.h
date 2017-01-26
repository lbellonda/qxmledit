/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2016 by Luca Bellonda and individual contributors  *
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


#ifndef CONFIGUREXMLMANAGEMENTDIALOG_H
#define CONFIGUREXMLMANAGEMENTDIALOG_H

#include "xmlEdit.h"
#include "modules/xml/xmlattributesindentationhelper.h"
#include "applicationdata.h"

namespace Ui
{
class ConfigureXMLManagementDialog;
}

class ConfigureXMLManagementDialog : public QDialog
{
    Q_OBJECT
    ApplicationData* _data;
    bool _started ;
    XMLAttributesIndentationHelper _attributeHelper;

public:
    explicit ConfigureXMLManagementDialog(QWidget *parent = 0);
    ~ConfigureXMLManagementDialog();

    void init(ApplicationData* data);
    void saveIfChanged();
    void enableIndent();

private:
    Ui::ConfigureXMLManagementDialog *ui;

    void calcEnableProlog();


private slots:
    void on_chkMetadata_clicked(bool /*state*/);
    void on_comboEncoding_activated(int /*index*/);
    void on_chkInsProlog_clicked(bool /*state*/);
    void on_xmlIndent_valueChanged(int newValue);
    void on_cmdIndentReset_clicked();
    void on_chkNoIndent_stateChanged(int /*state*/);
    void on_chkSortAttributes_stateChanged(int /*state*/);
    void on_attrCharacters_valueChanged(int /*i*/);
    void on_attrNoIndendation_clicked(bool /*checked*/);
    void on_attrNewLineAt_clicked(bool /*checked*/);
    void on_cmdPredefinedAttributes_clicked();
};

#endif // CONFIGUREXMLMANAGEMENTDIALOG_H
