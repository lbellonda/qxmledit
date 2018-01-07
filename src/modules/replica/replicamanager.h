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


#ifndef REPLICAMANAGER_H
#define REPLICAMANAGER_H

#include <QObject>

#include "regola.h"

class ReplicaCommand;

class ReplicaManager : public QObject
{
    Q_OBJECT

    bool applyReplicaToElement(Regola *regola, ReplicaCommand *cmd, Element *element, const int startIndex, const int numSiblings);
    int totalWidth(int count, int base);
    int base(ReplicaCommand *cmd);
    QString formatNumber(const int index, const bool isPadded, const int totalWidth);
    QString formatAlpha(const int index, const bool isPadded, const int totalWidth);
    QString makeId(ReplicaCommand *cmd, int index, int totalWidth);
public:
    explicit ReplicaManager(QObject *parent = 0);
    ~ReplicaManager();

    bool apply(QTreeWidget *widget, Regola *regola, Element *selected, ReplicaCommand *cmd, const int maxNum = -1);
#ifdef  QXMLEDIT_TEST
    friend class TestReplica;
#endif
signals:

public slots:

};

#endif // REPLICAMANAGER_H
