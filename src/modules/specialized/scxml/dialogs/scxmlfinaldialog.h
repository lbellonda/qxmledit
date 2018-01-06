/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#ifndef SCXMLFINALDIALOG_H
#define SCXMLFINALDIALOG_H

#include "modules/specialized/scxml/dialogs/basescxmleditdata.h"

namespace Ui
{
class SCXMLFinalDialog;
}

class SCXMLFinalDialog : public QDialog
{
    Q_OBJECT
    BaseSCXMLEditData p;
    BaseDData *d;

    void setupInsert();
    void setupEdit();
    void setupCommon();

public:
    explicit SCXMLFinalDialog(QWidget *parent, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                              Element *toModifyElement, Element *selectedElement, Element *parentElement);
    ~SCXMLFinalDialog();

    void accept();

private:
    Ui::SCXMLFinalDialog *ui;
};

#endif // SCXMLFINALDIALOG_H
