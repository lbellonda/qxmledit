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

#include "anonalgfreqcount.h"

//-----------------------------------
AlgStatRandomProvider::AlgStatRandomProvider()
{
    //
}

AlgStatRandomProvider::~AlgStatRandomProvider()
{
    //
}

//-----------------------------------

AnonAlgFreqCount::AnonAlgFreqCount()
{
    _randomProvider = this ;
    _ready = false;
    _total = 0 ;
}

AnonAlgFreqCount::~AnonAlgFreqCount()
{
    EMPTYPTRLIST(_children.values(), AnonAlgFreqTarget);
}

void AnonAlgFreqCount::setRandomProvider(AlgStatRandomProvider *randomProvider)
{
    if(NULL != randomProvider) {
        _randomProvider = randomProvider;
    }
}

int AnonAlgFreqCount::total()
{
    return _total;
}

QHash<int, AnonAlgFreqTarget*> AnonAlgFreqCount::children()
{
    return _children;
}

AnonAlgFreqTarget* AnonAlgFreqCount::childForSize(const int size)
{
    if(!_children.contains(size)) {
        AnonAlgFreqTarget *target = newChild();
        if(NULL == target) {
            return NULL;
        }
        insertTarget(size, target);
    }
    return _children[size];
}

void AnonAlgFreqCount::newHit(AnonAlgFreqTarget* target)
{
    _ready = false;
    _total++;
    target->newHit();
}

void AnonAlgFreqCount::insertTarget(const int id, AnonAlgFreqTarget* target)
{
    _ready = false;
    _children.insert(id, target);
    target->setId(_children.size());
    target->setKey(QString("%1").arg(id));
}

int AnonAlgFreqCount::isReady()
{
    return _ready;
}

void AnonAlgFreqCount::prepare()
{
    if(_ready) {
        return;
    }
    double currentIntervalValue = 0 ;
    AnonAlgFreqTarget *lastTarget = NULL;
    QList<int> keys = _children.keys();
    qSort(keys);
    foreach(const int key, keys) {
        AnonAlgFreqTarget *target = _children.value(key);
        if(_total != 0) {
            double percent = (double)target->count() / (double)_total;
            target->setRandomValueInterval0To1(currentIntervalValue, currentIntervalValue + percent);
            currentIntervalValue += percent;
        } else {
            target->setRandomValueInterval0To1(0, 1);
        }
        lastTarget = target;
    }
    if(NULL != lastTarget) {
        lastTarget->setLastInInterval0To1();
    }
    _ready = true ;
}

double AnonAlgFreqCount::chooseValue()
{
    const int value = qrand();
    return ((double)value) / RAND_MAX;
}

AnonAlgFreqTarget* AnonAlgFreqCount::randomChoice()
{
    prepare();
    double randomValue0To1 = _randomProvider->chooseValue();
    // find the nearest integer
    QList<int> keys = _children.keys();
    qSort(keys);
    foreach(const int key, keys) {
        AnonAlgFreqTarget *target = _children.value(key);
        if(target->isInInterval0To1(randomValue0To1)) {
            return target;
        }
    }
    return NULL;
}
