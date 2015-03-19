/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "testsnippethandler.h"
#include "ui_snippethandler.h"


TestSnippetHandler::TestSnippetHandler(const QString &newFileData, ApplicationData *data, const bool setEditMode, QWidget *parent) : SnippetHandler(data, setEditMode, parent)
{
    _fileData = newFileData;
}

TestSnippetHandler::~TestSnippetHandler()
{
    //
}

void TestSnippetHandler::accept()
{
    ui->snippets->setCurrentItem(ui->snippets->topLevelItem(0));
    SnippetHandler::accept();
    if( result() != QDialog::Accepted) {
        reject();
    }
}

void TestSnippetHandler::loadSnippets()
{
    Snippet *snippet = new Snippet();
    if( NULL != snippet ) {
        if(snippet->read(_fileData)) {
            _snippets.insert(snippet->id(), snippet);
        } else {
            delete snippet;
        }
    }
}

Snippet *TestSnippetHandler::getSelSnippet()
{
    if( _snippets.values().size()>0 ) {
        return  _snippets.values().at(0);
    }
    return NULL;
}

void TestSnippetHandler::initComplete()
{
    if(_snippets.count()>0) {
        accept();
    } else {
        reject();
    }
}

