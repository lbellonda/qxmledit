/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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
#ifndef EDITSNIPPET_H
#define EDITSNIPPET_H

#include <QDialog>
#include <QHash>
#include "snippet.h"

namespace Ui
{
class EditSnippet;
}

class EditSnippet : public QDialog
{
    Q_OBJECT
public:
    EditSnippet(const bool isInsert, Snippet *snippet, QWidget *parent = 0);
    virtual ~EditSnippet();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EditSnippet *ui;
    Snippet* _snippet ;
    bool _isInsert;


    bool areDataOK();
    void setUpData();
    void accept();
};

bool editSnippet(const bool isInsert, Snippet *snippet, QWidget *parent);

#endif // EDITSNIPPET_H
