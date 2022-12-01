/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2022 by Luca Bellonda and individual contributors       *
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

#ifndef ANONALGFREQCOUNT_H
#define ANONALGFREQCOUNT_H

#include <xmlEdit.h>
#include "anonalgfreqtarget.h"
#include "modules/anonymize/anonbase.h"

class AlgStatRandomProvider
{
public:
    AlgStatRandomProvider();
    virtual ~AlgStatRandomProvider();

    // Between 0 and 1
    virtual double chooseValue() = 0;
};

class AnonAlgFreqCount : public AlgStatRandomProvider
{
    bool _ready;
    int _total;
    QHash<int, AnonAlgFreqTarget*> _children;
    /** @brief: this field not automatically deallocated */
    AlgStatRandomProvider *_randomProvider;

protected:
    AnonAlgFreqTarget* childForSize(const int size);
    virtual AnonAlgFreqTarget* newChild() = 0;
    void newHit(AnonAlgFreqTarget *target);
    void insertTarget(const int id, AnonAlgFreqTarget* target);
    int isReady();
    void prepare();
    AnonAlgFreqTarget* randomChoice();
    double chooseValue();
public:
    AnonAlgFreqCount();
    virtual ~AnonAlgFreqCount();

    int total();
    QHash<int, AnonAlgFreqTarget*> children();
    void setRandomProvider(AlgStatRandomProvider *randomProvider);

#ifdef QXMLEDIT_TEST
    friend class TestAnonymize;
#endif
};

#endif // ANONALGFREQCOUNT_H
