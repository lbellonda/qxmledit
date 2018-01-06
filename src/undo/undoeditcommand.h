/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#ifndef UNDOEDITCOMMAND_H
#define UNDOEDITCOMMAND_H

#include <QApplication>
#include "undocommand.h"
class Element;

class UndoEditCommand : public UndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(UndoEditCommand)
    bool _firstShot;
    Element *_modifiedElement;
    Element *_originalElement;

    void makeACopy(Element *source);
public:
    UndoEditCommand(QTreeWidget *theWidget, Regola *newRegola, QList<int> path);
    virtual ~UndoEditCommand();

    virtual void undo();
    virtual void redo();

    void setOriginalElement(Element *beforeEdit);
    void setModifiedElement(Element *afterEdit);
};

#endif // UNDOEDITCOMMAND_H
