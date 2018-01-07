/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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


#ifndef UNDOCOMMANDGROUP_H
#define UNDOCOMMANDGROUP_H

#include <QUndoCommand>
#include <QTreeWidget>

class Regola;
class QTreeWidget ;

class UndoCommandGroup : public QUndoCommand
{
    QList<int> _selectedPath;
    Regola *_target;
    QTreeWidget *_treeWidget;
    bool _isHiliteAll;
    bool _isCloseUnrelated;
public:
    explicit UndoCommandGroup();
    virtual ~UndoCommandGroup();

    virtual void undo();
    virtual void redo();

    //---
    QList<int> selectedPath() const;
    void setSelectedPath(const QList<int> &selectedPath);
    Regola *target() const;
    void setTarget(Regola *target);
    QTreeWidget *treeWidget() const;
    void setTreeWidget(QTreeWidget *treeWidget);
    bool isHiliteAll() const;
    void setIsHiliteAll(bool isHiliteAll);
    bool isCloseUnrelated() const;
    void setIsCloseUnrelated(bool isCloseUnrelated);
};

#endif // UNDOCOMMANDGROUP_H
