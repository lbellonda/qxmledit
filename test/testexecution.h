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

#ifndef TESTEXECUTION_H
#define TESTEXECUTION_H

#include <QObject>

#include "config.h"
#include "mainwindow.h"
#include "utils.h"

class TestExecutionCustomization
{
public:
    virtual bool execTest(MainWindow *mainWindow, Regola *regola, Element *selectedElement, const int operationType) = 0 ;
    virtual QList<int> getSelection(const int operationType) = 0 ;
};


class TestExecution : public QObject
{
    Q_OBJECT

    bool _showXML;
public:
    explicit TestExecution(QObject *parent = 0);

    bool execTest(const QString &inputFileName, const QString &outputFileName, TestExecutionCustomization &transformer, const int operationType);

signals:

public slots:

};

#endif // TESTEXECUTION_H
