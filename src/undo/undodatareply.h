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


#ifndef UNDODATAREPLY_H
#define UNDODATAREPLY_H

#include "xmlEdit.h"
#include <QTemporaryFile>
#include "undocommand.h"
#include "writeelementundoobserver.h"
#include "regola.h"

class UndoDataReply : public UndoCommand
{
private:
    //--
    QTemporaryFile *_undoData;
protected:
    TargetSelection::Type _targetSelection;

    virtual bool innerUndo();
    virtual bool innerRedo();
    virtual bool innerRedoSpecific(QList<Element *>elements, WriteElementUndoObserver *observer) = 0 ;
    bool preInnerRedoSpecific(WriteElementUndoObserver *observer);
public:
    UndoDataReply(QTreeWidget *theWidget, Regola *newRegola, QList<int> path, const TargetSelection::Type targetSelection, QUndoCommand *parent = NULL);
    virtual ~UndoDataReply();
    bool observe(QDataStream &writeDataStream, ElementUndoInfo *info);
    //--
    static const int DATASTREAM_VERSION = QDataStream::Qt_4_0;
};

#endif // UNDODATAREPLY_H
