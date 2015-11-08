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


#ifndef ELBASECOMMAND_H
#define ELBASECOMMAND_H

#include    "undocommand.h"

class Element ;

class ElBaseCommand : public UndoCommand
{
protected:
    bool _firstShot;
    Element *_element;
    Element *_lastOpElement ;
    bool _selectParent;
    bool _hilite;

    void loseElement();

public:
    explicit ElBaseCommand(QTreeWidget *theWidget, Regola *newRegola, Element *element, QList<int> newPath, QUndoCommand *parent = NULL) ;
    virtual ~ElBaseCommand();

    virtual void undo() = 0 ;
    virtual void redo() = 0 ;


    bool selectParent() const;
    void setSelectParent(const bool selectParent);

    bool hilite() const;
    void setHilite(bool hilite);

protected:
    void removeElement();
    void insertElement();
    void insertElementObj(Element *element, const bool isUndo);
    void setCurrentElement(Element *newElement);
    Element *removeElementAndReturnIt();
    void replaceElement(const bool isUndo);
    void replaceElementInPlace(const bool isUndo, Element *source);
    void updateElementObj(Element *current, Element *newValues, const bool isUndo);
};

#endif // ELBASECOMMAND_H
