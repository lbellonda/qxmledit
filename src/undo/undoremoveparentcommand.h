/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#ifndef UNDOREMOVEPARENTCOMMAND_H
#define UNDOREMOVEPARENTCOMMAND_H

#include "undocommand.h"

class Element ;
class Regola ;
class Attribute;

class UndoRemoveParentCommand : public UndoCommand
{
    QString _tag;
    QList<Attribute*> _attributes;
    int _insPos ;
    int _insLen;
    void reset();
public:
    explicit UndoRemoveParentCommand(QTreeWidget *theWidget, Regola *newRegola, QList<int> newPath);
    virtual ~UndoRemoveParentCommand();

    virtual void undo() ;
    virtual void redo() ;

};
#endif // UNDOREMOVEPARENTCOMMAND_H
