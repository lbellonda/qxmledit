/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#ifndef STARTACTIONSENGINE_H
#define STARTACTIONSENGINE_H

#include "xmlEdit.h"

class ApplicationData;
class StartParams;

class StartActionsExecutor {

public:
    StartActionsExecutor();
    virtual ~StartActionsExecutor();

    virtual void startActionShowUserTypePanel() = 0 ;
    virtual bool startActionShowGuidedOperationsPanel() = 0 ;
    virtual void startActionSetupFirstAccessForPreferences() =0 ;
    virtual void startActionTriggersWelcomeDialog() = 0 ;
    virtual void startActionLoadFile(const QString &fileName) = 0 ;
};


class StartActionsEngine : public QObject
{
    ApplicationData *_data;
    StartActionsExecutor *_executor;
    Q_OBJECT
public:
    explicit StartActionsEngine(ApplicationData *data, StartActionsExecutor *executor);
    virtual ~StartActionsEngine();

    bool execute(StartParams &startParams);

signals:

public slots:
};

#endif // STARTACTIONSENGINE_H
