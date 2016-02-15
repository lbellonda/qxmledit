/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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


#ifndef UNDODELETESIBLINGS_H
#define UNDODELETESIBLINGS_H

#include "undocommand.h"

#include "regola.h"

class DeleteSiblingsCommand : public UndoCommand
{
    RegolaDeleteSiblings::DeleteOptions _option;
    int _posBefore;
    QList<Element*> _beforeElements;
    int _posAfter;
    QList<Element*> _afterElements;

    //-----
    void reset();
    void restoreSiblingsBefore();
    void restoreSiblingsAfter();
    void updateRegola(const bool changed, Element *element);
    void deleteSiblings();
    void updateRegola(const bool changed);
    bool deleteAllSiblingsAfter(Element *selected);
    bool deleteAllSiblingsBefore(Element *selected);
    bool deleteAllSiblings(Element *selected);

public:
    DeleteSiblingsCommand(const RegolaDeleteSiblings::DeleteOptions newOption, QTreeWidget *theWidget, Regola *newRegola, QList<int> newPath);
    ~DeleteSiblingsCommand();

    virtual void undo() ;
    virtual void redo() ;

};

#endif // UNDODELETESIBLINGS_H
