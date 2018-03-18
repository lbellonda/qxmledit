/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#ifndef CONFIGUREGENERALINDENTATION_H
#define CONFIGUREGENERALINDENTATION_H

#include "xmlEdit.h"
#include "modules/xml/xmlattributesindentationhelper.h"

class QXmlEditData;

namespace Ui
{
class ConfigureGeneralIndentation;
}

class ConfigureGeneralIndentation : public QDialog
{
    Q_OBJECT

    bool _started;
    QXmlEditData *_appData;
    XMLAttributesIndentationHelper _attributeHelper;

public:
    explicit ConfigureGeneralIndentation(QWidget *parent, QXmlEditData *appData);
    ~ConfigureGeneralIndentation();
    void doAccept();

private:
    Ui::ConfigureGeneralIndentation *ui;

    void init();
    void accept();

private slots:
    void on_chkNoIndent_stateChanged(int /*state*/);
    void on_cmdPredefinedAttributes_clicked();
    void on_attrNoIndendation_clicked(bool checked);
    void on_attrNewLineAt_clicked(bool checked);
};

#endif // CONFIGUREGENERALINDENTATION_H
