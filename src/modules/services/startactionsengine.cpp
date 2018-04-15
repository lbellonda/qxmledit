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

#include "startactionsengine.h"
#include "applicationdata.h"
#include "StartParams.h"
#include "utils.h"

//------------------------------------------------------------------------
StartActionsExecutor::StartActionsExecutor()
{
    //
}

StartActionsExecutor::~StartActionsExecutor()
{
    //
}
//------------------------------------------------------------------------

StartActionsEngine::StartActionsEngine(ApplicationData *data, StartActionsExecutor *executor) : QObject(NULL)
{
    _data = data ;
    _executor = executor ;
}

StartActionsEngine::~StartActionsEngine()
{
    //
}

/**
 * @brief StartActionsEngine::execute executes the logic of first access
 */
bool StartActionsEngine::execute(StartParams &startParams)
{
    if((NULL == _executor) || (NULL == _data)) {
        Utils::error(NULL, tr(""));
        return false;
    }
    const bool wasFirstAccess = _data->isUserFirstAccess();
    if(wasFirstAccess) {
        _executor->startActionShowUserTypePanel();
        _data->fireUserFirstAccess();
    }
    if(_data->isUserGuidedOperation()) {
        // modeless
        _executor->startActionShowGuidedOperationsPanel();
    } else {
        // editor view detail level
        if(wasFirstAccess) {
            _executor->startActionSetupFirstAccessForPreferences();
        }
    }

    switch(startParams.type) {
    default:
    case StartParams::Nothing:
        if(!_data->isUserGuidedOperation()) {
            _executor->startActionTriggersWelcomeDialog();
        }
        break;
    case StartParams::OpenFile:
        _executor->startActionLoadFile(startParams.fileName);
        break;
    }
    return true ;

}




